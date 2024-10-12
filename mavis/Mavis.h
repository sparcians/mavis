#pragma once

#include "mavis/DecoderExceptions.h"
#include "mavis/DecoderTypes.h"
#include "mavis/DTable.h"
#include "mavis/ContextRegistry.hpp"
#include <memory>
#include <vector>
#include <string>
#include <iostream>

namespace mavis {

    class ExtractorDirectInfo;

    template<class InstType>
    struct SharedPtrAllocator
    {
        using InstTypePtr = std::shared_ptr<InstType>;

        template<typename ...Args>
        InstTypePtr operator()(Args&& ...args)
        {
            return std::make_shared<InstType>(std::forward<Args>(args)...);
        }
    };
}

/**
 * \brief Mavis decoder toplevel facade
 * \tparam InstType type of instructions to generate
 * \tparam AnnotationType type of micro-architectural info objects to use
 */
template<typename InstType,
         typename AnnotationType,
         typename InstTypeAllocator       = mavis::SharedPtrAllocator<InstType>,
         typename AnnotationTypeAllocator = mavis::SharedPtrAllocator<AnnotationType> >
class Mavis
{
public:

    using Opcode = mavis::Opcode;
    using FileNameListType = mavis::FileNameListType;
    using RegListType = mavis::ExtractorIF::RegListType;
    using OpInfoListType = mavis::OperandInfo::ElementList;
    using DecodeInfoType = typename mavis::IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType;
    using InstructionType = mavis::InstMetaData::InstructionTypes;
    using ExtractedInstType = mavis::OpcodeInfo::ExtractedInstTypes;
    using DirectInfoType = mavis::ExtractorDirectInfo;
    using DirectInfoBitMaskType = mavis::ExtractorDirectInfoBitMask;
    using ContextRegistryType = mavis::ContextRegistry<InstType, AnnotationType, AnnotationTypeAllocator>;
    using InstUIDList = mavis::InstUIDList;
    using AnnotationOverrides = mavis::AnnotationOverrides;

public:
    /**
     * \brief Construct the Mavis facade
     *
     * @param isa_files   Pointer to the ISA JSON file
     * @param anno_files  Pointer to the annotation JSON files (e.g. uArchInfo files, etc.)
     * @param uid_list    List of UIDs to associated with given instructions
     * @param anno_overrides A vector of NVP pairs to apply a change to the annotation (see below)
     * @param inst_allocator       Reference to a memory allocator to use when creating InstType objects
     * @param annotation_allocator Reference to a memory allocator to use when creating AnnotationType objects
     *
     * \note Keep in mind that the allocators are _copied_ into this class.
     *
     * Annotation overrides are expected to be the form (pairing):
     *  std::pair<std::string, std::string>
     *    "mnemonic"   : std::string
     *    "attr:value" : std::string
     *
     *  Example:
     *     anno_overrides = { std::make_pair("ctzw", "pipelined:false") };
     *     anno_overrides = { std::make_pair("ctzw", "pipes: [\"div\", \"sys\"]") };
     *
     */
    Mavis(const FileNameListType&        isa_files,
          const FileNameListType&        anno_files,
          const InstUIDList&             uid_list,
          const AnnotationOverrides &    anno_overrides,
          const mavis::MatchSet<mavis::Pattern>& inclusions,
          const mavis::MatchSet<mavis::Pattern>& exclusions,
          const InstTypeAllocator&       inst_allocator       = mavis::SharedPtrAllocator<InstType>(),
          const AnnotationTypeAllocator& annotation_allocator = mavis::SharedPtrAllocator<AnnotationType>()) :
        inst_allocator_(inst_allocator), annotation_allocator_(annotation_allocator),
        context_(annotation_allocator)
    {
        static_assert(std::is_same<typename InstTypeAllocator::InstTypePtr, typename InstType::PtrType>::value,
                      "You cannot use an InstTypeAllocator that returns a different pointer type than the InstType thinks it is");
        static_assert(std::is_same<typename AnnotationTypeAllocator::InstTypePtr, typename AnnotationType::PtrType>::value,
                      "You cannot use an AnnotationTypeAllocator that returns a different pointer type than the AnnotationTypeAllocator thinks it is");
        makeContext("BASE", isa_files, anno_files, uid_list, anno_overrides, inclusions, exclusions);
        switchContext("BASE");
    }

    Mavis(const FileNameListType&        isa_files,
          const FileNameListType&        anno_files,
          const InstUIDList&             uid_list,
          const AnnotationOverrides &    anno_overrides       = {},
          const InstTypeAllocator&       inst_allocator       = mavis::SharedPtrAllocator<InstType>(),
          const AnnotationTypeAllocator& annotation_allocator = mavis::SharedPtrAllocator<AnnotationType>()) :
        Mavis(isa_files, anno_files, uid_list, anno_overrides, {}, {}, inst_allocator, annotation_allocator)
    {}

    Mavis(const FileNameListType& isa_files,
          const FileNameListType& anno_files,
          const mavis::MatchSet<mavis::Pattern>& inclusions,
          const mavis::MatchSet<mavis::Pattern>& exclusions,
          const InstTypeAllocator&       inst_allocator       = mavis::SharedPtrAllocator<InstType>(),
          const AnnotationTypeAllocator& annotation_allocator = mavis::SharedPtrAllocator<AnnotationType>()) :
        Mavis(isa_files, anno_files, {}, {}, inclusions, exclusions, inst_allocator, annotation_allocator)
    {}

    Mavis(const FileNameListType& isa_files,
          const FileNameListType& anno_files,
          const InstTypeAllocator&       inst_allocator       = mavis::SharedPtrAllocator<InstType>(),
          const AnnotationTypeAllocator& annotation_allocator = mavis::SharedPtrAllocator<AnnotationType>()) :
        Mavis(isa_files, anno_files, {}, {}, {}, {}, inst_allocator, annotation_allocator)
    {}

    void makeContext(const std::string& name, const FileNameListType& isa_files, const FileNameListType& anno_files,
                     const InstUIDList& uid_list = {}, const AnnotationOverrides & anno_overrides = {},
                     const mavis::MatchSet<mavis::Pattern>& inclusions = mavis::MatchSet<mavis::Pattern>(),
                     const mavis::MatchSet<mavis::Pattern>& exclusions = mavis::MatchSet<mavis::Pattern>())
    {
        context_.makeContext(name, isa_files, anno_files, uid_list, anno_overrides, inclusions, exclusions);
    }

    void switchContext(const std::string& name)
    {
        context_.switchContext(name);
        builder_ = context_.getBuilder();
        pseudo_builder_ = context_.getPseudoBuilder();
        dtrie_ = context_.getDTable();
    }

    bool hasContext(const std::string& name)
    {
        return context_.hasContext(name);
    }

    template<typename ...ArgTypes>
    typename InstType::PtrType makeInst(const mavis::Opcode icode, ArgTypes&& ... args)
    {
        return dtrie_->makeInst(icode, inst_allocator_, std::forward<ArgTypes>(args)...);
    }

    template<typename TraceInfoType, typename ...ArgTypes>
    typename InstType::PtrType makeInstFromTrace(const TraceInfoType& tinfo, ArgTypes&& ... args)
    {
        return dtrie_->makeInstFromTrace(tinfo, inst_allocator_, std::forward<ArgTypes>(args)...);
    }

    template<typename ...ArgTypes>
    typename InstType::PtrType makeInstDirectly(const mavis::ExtractorDirectInfoIF& user_info, ArgTypes&& ... args)
   { return dtrie_->makeInstDirectly(user_info, inst_allocator_, std::forward<ArgTypes>(args)...);
        }

    
    

    /**
     * @brief makePseudoInst -- create a pseudo instruction (InstType)
     * @tparam ArgTypes
     * @param ex_info Direct extractor
     * @param allocator InstType allocator
     * @param args InstType construction args
     * @return Pointer to constructed InstType
     */
    template<typename ...ArgTypes>
    typename InstType::PtrType makePseudoInst(const mavis::ExtractorDirectInfoIF &ex_info, ArgTypes &&... args)
    {
        typename InstType::PtrType inst = nullptr;
        // typename mavis::IFactoryIF<InstType, AnnotationType>::PtrType ifact = nullptr;
        typename mavis::IFactoryPseudo<InstType, AnnotationType>::PtrType ifact = nullptr;
        std::string mnemonic = ex_info.getMnemonic();
        const mavis::InstructionUniqueID uid = ex_info.getUID();

        // Try to look up the factory by UID (if present)
        if (uid != mavis::INVALID_UID) {
            mnemonic = lookupPseudoInstMnemonic(uid);
            ifact = pseudo_builder_->findIFact(uid);
        } else {
            // Look up the factory for the given mnemonic
            ifact = pseudo_builder_->findIFact(mnemonic);
        }

        if (ifact == nullptr) {
            throw mavis::UnknownPseudoMnemonic(mnemonic);
        } else {
            const typename mavis::IFactoryIF<InstType, AnnotationType>::IFactoryInfo::PtrType &info =
                ifact->getInfo(mnemonic, ex_info.clone());
            inst = inst_allocator_(info->opinfo, info->uinfo, std::forward<ArgTypes>(args)...);
        }

        return inst;
    }

    void morphInst(typename InstType::PtrType inst, const mavis::ExtractorDirectInfoIF& user_info) const
    {
        dtrie_->morphInst(inst, user_info);
    }

    // Not const because getInfo will cache instruction information
    DecodeInfoType getInfo(const mavis::Opcode icode)
    {
        return dtrie_->getInfo(icode);
    }

    // Not const because getInfo will cache instruction information
    bool isOpcodeInstType(Opcode icode, InstructionType itype)
    {
        const DecodeInfoType info = getInfo(icode);
        return info->opinfo->isInstType(itype);
    }

    // Not const because getInfo will cache instruction information
    bool isOpcodeExtractedInstType(Opcode icode, ExtractedInstType itype)
    {
        const DecodeInfoType info = getInfo(icode);
        return info->opinfo->isExtractedInstType(itype);
    }

    mavis::InstructionUniqueID lookupInstructionUniqueID(const std::string& mnemonic) const
    {
        return builder_->findInstructionUID(mnemonic);
    }

    mavis::InstructionUniqueID lookupPseudoInstUniqueID(const std::string& mnemonic) const
    {
        return pseudo_builder_->findInstructionUID(mnemonic);
    }

    const std::string& lookupInstructionMnemonic(const mavis::InstructionUniqueID uid) const
    {
        return builder_->findInstructionMnemonic(uid);
    }

    const std::string& lookupPseudoInstMnemonic(const mavis::InstructionUniqueID uid) const
    {
        return pseudo_builder_->findInstructionMnemonic(uid);
    }

    void setPseudoInstDisassembler(const mavis::InstructionUniqueID uid, const mavis::DisassemblerIF::PtrType& dasm)
    {
        pseudo_builder_->setDisassembler(uid, dasm);
    }

    mavis::Opcode getOpcode(const std::string& mnemonic) const
    {
        auto ifact = builder_->findIFact(mnemonic);
        if (ifact == nullptr) {
            throw mavis::UnknownMnemonic(mnemonic);
        } else {
            return ifact->getStencil();
        }
    }

    void flushCaches()
    {
        dtrie_->flushCaches();
    }

private:
    InstTypeAllocator       inst_allocator_;
    AnnotationTypeAllocator annotation_allocator_;
    ContextRegistryType     context_;

    typename mavis::IFactoryBuilder<InstType, AnnotationType, AnnotationTypeAllocator>::PtrType     builder_;
    typename mavis::PseudoBuilder<InstType, AnnotationType, AnnotationTypeAllocator>::PtrType       pseudo_builder_;
    typename mavis::DTable<InstType, AnnotationType, AnnotationTypeAllocator>::PtrType              dtrie_;

private:
    void print(std::ostream& os) const
    {
        os << *dtrie_;
    }

public:
    friend std::ostream& operator<<(std::ostream& os, const Mavis& facade)
    {
        facade.print(os);
        return os;
    }
};
