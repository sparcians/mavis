#pragma once

#include "IFactory.h"
#include "FormGeneric.hpp"
#include "ExtractorWrap.hpp"

namespace mavis {

/**
 * IFactoryPseudo: Leaf node of composite pattern for PSEUDO instructions
 */
template<typename InstType, typename AnnotationType>
class IFactoryPseudo : public IFactoryIF<InstType, AnnotationType>
{
public:
    typedef std::shared_ptr<IFactoryPseudo> PtrType;

    IFactoryPseudo(const std::string& name, InstructionUniqueID uid,
                   const InstMetaData::PtrType& meta,
                   const DisassemblerIF::PtrType& dasm,
                   const FormGeneric::PtrType& form,
                   const typename AnnotationType::PtrType anno) :
        name_(name), meta_(meta), dasm_(dasm), form_(form), anno_(anno), uid_(uid)
    {}

    std::string getName() const override
    {
        std::stringstream ss;
        ss << "IFactoryPseudo('" << name_ << "')";
        return ss.str();
    }

    /**
     * \brief Version of getInfo which does not use the DecodedInstInfo cache. This is called by
     * DTable::makeInstDirectly (i.e. no opcode supplied)
     * \param mnemonic
     * \param icode
     * \param extractor
     * \return
     */
    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType getInfo(const std::string& mnemonic,
                                                                                 const ExtractorIF::PtrType& extractor)
    {
        const ExtractorWrap::PtrType& ext_wrap = std::make_shared<ExtractorWrap>(extractor, form_);
        const DecodedInstructionInfo::PtrType& new_dii = std::make_shared<DecodedInstructionInfo>(mnemonic, uid_,
                                                                                                  ext_wrap, meta_,
                                                                                                  Opcode(0));
        OpcodeInfo::PtrType optr = std::make_shared<OpcodeInfo>(Opcode(0), new_dii, ext_wrap, meta_, dasm_);
        return std::make_shared<typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo>(optr, anno_);
    }

    void setDisassembler(const DisassemblerIF::PtrType& dasm)
    {
        dasm_ = mavis::utils::notNull(dasm);
    }

    void print(std::ostream& os, const uint32_t) const override
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << getName() << std::endl;
        os.flags(os_state);
    }

private:
    const std::string name_; // factory name
    InstMetaData::PtrType meta_;
    DisassemblerIF::PtrType dasm_;
    FormGeneric::PtrType form_;
    typename AnnotationType::PtrType anno_;
    InstructionUniqueID uid_;

private:
    const Field* getField() const override
    {
        throw std::runtime_error("Unimplemented");
        return nullptr;
    }

    void addIFactory(const Opcode, const typename IFactoryIF<InstType, AnnotationType>::PtrType&) override
    {
        throw std::runtime_error("Unimplemented");
    }

    void addIFactory(const std::string&, const Opcode,
                     const typename IFactoryIF<InstType, AnnotationType>::PtrType&,
                     const ExtractorIF::PtrType&) override
    {
        throw std::runtime_error("Unimplemented");
    }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getNode(const Opcode) override
    { return nullptr; }

    typename IFactoryIF<InstType, AnnotationType>::PtrType getDefault() const override
    { return nullptr; }

    void addDefaultIFactory(const typename IFactoryIF<InstType, AnnotationType>::PtrType&) override
    {
        throw std::runtime_error("Unimplemented");
    }

    Opcode getStencil() const override
    { return 0; }

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType getInfo(Opcode) override
    {
        throw std::runtime_error("Unimplemented");
        return nullptr;
    }

    typename IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType
    getInfo(const std::string&, Opcode, const ExtractorIF::PtrType&) override
    {
        throw std::runtime_error("Unimplemented");
        return nullptr;
    }

    void flushCaches() override
    {}
};

} // namespace mavis
