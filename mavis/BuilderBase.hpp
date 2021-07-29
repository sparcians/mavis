#pragma once

#include "IFactory.h"
#include "InstMetaData.h"
#include "InstMetaDataRegistry.hpp"
#include "InstructionRegistry.hpp"
#include "AnnotationRegistry.hpp"
#include "DualKeyRegistry.hpp"
#include "DecoderExceptions.h"

namespace mavis
{

template<typename FactoryType, typename InstType, typename AnnotationType, typename AnnotationTypeAllocator>
class FactoryBuilderBase
{
    typedef std::shared_ptr<FactoryBuilderBase<FactoryType, InstType, AnnotationType, AnnotationTypeAllocator>>     PtrType;
    typedef AnnotationRegistry<AnnotationType,AnnotationTypeAllocator>          AnnotationRegistryType;
    typedef DualKeyRegistry<typename FactoryType::PtrType>                UIDStashType;

public:
    FactoryBuilderBase(const FileNameListType& anno_files,
                       AnnotationTypeAllocator & annotation_allocator,
                       const InstUIDList& uid_list = {},
                       const AnnotationOverrides & anno_overrides = {}) :
        inst_registry_(uid_list),
        anno_registry_(anno_files, annotation_allocator, anno_overrides)
    {}

    InstructionUniqueID registerInst(const std::string& mnemonic)
    {
        return inst_registry_.registerInst(mnemonic);
    }

    InstructionUniqueID findInstructionUID(const std::string& mnemonic) const
    {
        return inst_registry_.lookupUID(mnemonic);
    }

    const std::string& findInstructionMnemonic(const InstructionUniqueID uid) const
    {
        return inst_registry_.lookupMnemonic(uid);
    }

    const typename AnnotationType::PtrType& findAnnotation(const std::string& mnemonic,
                                                           bool suppress_exception = false) const
    {
        return anno_registry_.findAnnotation(mnemonic, suppress_exception);
    }

    InstMetaData::PtrType findMetaData(const std::string& mnemonic) const
    {
        return meta_registry_.lookup(mnemonic);
    }

    template<typename ...ArgTypes>
    InstMetaData::PtrType makeInstMetaData(ArgTypes&& ...args)
    {
        return meta_registry_.makeInstMetaData(std::forward<ArgTypes>(args)...);
    }

    const typename FactoryType::PtrType& findIFact(const std::string& mnemonic) const
    {
        const auto elem = registry_.find(mnemonic);
        if (elem == registry_.end()) {
            return not_found_;
        } else {
            return elem->second;
        }
    }

    const typename FactoryType::PtrType findIFact(const InstructionUniqueID uid)
    {
        if (uid_stash_.contains(uid)) {
            return uid_stash_.lookup(uid);
        } else {
            // Lazy add of ifact when first we fail to look up
            const std::string& mnemonic = findInstructionMnemonic(uid);
            const typename FactoryType::PtrType ifact = findIFact(mnemonic);
            if (ifact != nullptr) {
                uid_stash_.add(mnemonic, uid, ifact);
            }
            return ifact;
        }
    }

protected:
    std::map<std::string, typename FactoryType::PtrType>    registry_;
    typename FactoryType::PtrType                           not_found_;

    InstructionRegistry                                     inst_registry_;
    AnnotationRegistryType                                  anno_registry_;
    InstMetaDataRegistry                                    meta_registry_;
    UIDStashType                                            uid_stash_;
};

} // namespace mavis
