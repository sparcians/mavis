#pragma once

#include <map>
#include <fstream>
#include <boost/json.hpp>
#include "BuilderBase.hpp"
#include "Extractor.h"
#include "IFactoryPseudo.hpp"
#include "FormGeneric.hpp"

namespace mavis {

template<typename InstType, typename AnnotationType, typename AnnotationTypeAllocator>
class PseudoBuilder : public FactoryBuilderBase<IFactoryPseudo<InstType,AnnotationType>, InstType, AnnotationType, AnnotationTypeAllocator>
{
public:
    typedef std::shared_ptr<PseudoBuilder<InstType,AnnotationType,AnnotationTypeAllocator>>        PtrType;

private:
    typedef IFactoryPseudo<InstType, AnnotationType>           FactoryType;

    // We're inheriting from a templated base class, so we need to tell the compiler how to find
    // base class members we use here in the derived class. NOTE: We have to declare the "using"
    // reference here, outside the methods in order for it work.
    //
    // Another way to do help the compiler find base class members is to explicitly de-reference
    // them with "this->". I've used that method for other members in the code below. I've opted to
    // declare registry_ with "using" here instead, since registry_ is an std::map() and I'm doing
    // index operations on it. The syntax is cleaner for indexing registry_ this way.
    using FactoryBuilderBase<FactoryType, InstType, AnnotationType, AnnotationTypeAllocator>::registry_;

public:
    explicit PseudoBuilder(const FileNameListType& anno_files,
                           AnnotationTypeAllocator & annotation_allocator,
                           const InstUIDList& uid_list = {}) :
        FactoryBuilderBase<FactoryType,InstType,AnnotationType,AnnotationTypeAllocator>(anno_files, annotation_allocator, uid_list)
    {}

    PseudoBuilder(const PseudoBuilder&) = delete;

    /**
     * @brief configure
     * @param isa_files
     */
    void configure(const FileNameListType &isa_files)
    {
        // Look for pseudo instruction entries in the ISA files
        for (const auto &jfile : isa_files) {
            std::ifstream fs;

            try {
                fs.open(jfile);
            } catch (const std::ifstream::failure &ex) {
                throw BadISAFile(jfile);
            }

            boost::system::error_code ec;
            boost::json::value json = boost::json::parse(fs, ec);

            if(json.is_null() || ec) {
                throw boost::system::system_error(ec);
            }

            const auto& jobj = json.as_array();

            // Read in the pseudo instructions JSON file and process its fields
            for (const auto &inst_value : jobj) {
                const auto& inst = inst_value.as_object();
                std::string mnemonic;
                if (const auto it = inst.find("pseudo"); it != inst.end()) {
                    mnemonic = boost::json::value_to<std::string>(it->value());
                    InstMetaData::PtrType meta = this->makeInstMetaData(mnemonic, inst);
                    // TODO: Implement a "getDisassembler" method to retrieve the disassembler object
                    // associated with this pseudo-instruction... Maybe this is like makeInstMetaData()?
                    Disassembler::PtrType dasm = std::make_shared<Disassembler>();
                    FormGeneric::PtrType form = std::make_shared<FormGeneric>(inst, meta);
                    build_(mnemonic, meta, dasm, form);
                }
            }

            fs.close();
        }
    }

    // TODO: Move this to BuilderBase() once we have full support for Disassembler
    void setDisassembler(const InstructionUniqueID uid, const DisassemblerIF::PtrType& dasm)
    {
        assert(dasm != nullptr);
        typename FactoryType::PtrType ifact = this->findIFact(uid);
        if (ifact == nullptr) {
            throw UnknownPseudoUID(uid);
        }
        ifact->setDisassembler(dasm);
    }

private:
    /**
     * \brief Build an IFactory for the given instruction
     * \tparam ArgTypes
     * \param mnemonic
     * \param factory_name
     * \param xpand_name
     * \param stencil
     * \param meta
     * \param args
     * \return
     *
     * This method will build a new IFactory for the given instruction, or if the factory
     * already exists, will return that IFactory (possibly augmented with the new information)
     *
     * There are a couple of cases where the factory may already exist:
     *
     * 1. If the JSON for the instruction lists a specific factory to use ("factory" clause)
     * 2. If the JSON for the instruction indicates the instruction is compressed and needs to use
     *    an existing factory for expansion ("expand" clause)
     */
    typename IFactoryIF<InstType, AnnotationType>::PtrType build_(const std::string& mnemonic,
                                                                  const InstMetaData::PtrType& meta,
                                                                  const DisassemblerIF::PtrType& dasm,
                                                                  const FormGeneric::PtrType& form)
    {
        // Try to find the named IFactory
        typename FactoryType::PtrType ifact = this->findIFact(mnemonic);

        if (ifact == nullptr) {
            // We need to build a new factory...

            // Register the instruction and obtain a UID
            const InstructionUniqueID inst_uid = this->registerInst(mnemonic);

            // Try to find a matching annotation for the instruction's mnemonic...
            // NOTE: panno may still be NULL here. findAnnotation() will have thrown an
            // exception if there were annotation files provided
            typename AnnotationType::PtrType panno = this->findAnnotation(mnemonic);

            // Build a new factory
            ifact.reset(new FactoryType(mnemonic, inst_uid, meta, dasm, form, panno));

            // Register it in our builder registry
            registry_[mnemonic] = ifact;
        }

        return ifact;
    }
};

} // namespace mavis
