#pragma once

#include "mavis/DecoderExceptions.h"
#include "mavis/DecoderTypes.h"
#include "mavis/IFactoryBuilder.h"
#include "mavis/PseudoBuilder.hpp"
#include "mavis/DTable.h"
#include <map>

namespace mavis {

template<typename InstType, typename AnnotationType, typename AnnotationTypeAllocator>
class ContextRegistry
{
    using BuilderType = mavis::IFactoryBuilder<InstType, AnnotationType, AnnotationTypeAllocator>;
    using PseudoBuilderType = mavis::PseudoBuilder<InstType, AnnotationType, AnnotationTypeAllocator>;
    using DTableType = mavis::DTable<InstType, AnnotationType, AnnotationTypeAllocator>;

public:
    explicit ContextRegistry(const AnnotationTypeAllocator& anno_allocator) :
        annotation_allocator_(anno_allocator)
    {}

    ContextRegistry(const ContextRegistry&) = delete;
    ContextRegistry(ContextRegistry&&) = default;
    ContextRegistry& operator=(ContextRegistry&&) = default;

    void makeContext(const std::string& name, const FileNameListType& isa_files, const FileNameListType& anno_files,
                     const InstUIDList& uid_list = {}, const AnnotationOverrides & anno_overrides = {})
    {
        if (registry_.find(name) != registry_.end()) {
            throw ContextAlreadyExists(name);
        }

        registry_[name] = {annotation_allocator_, isa_files, anno_files, uid_list, anno_overrides};
    }

    void switchContext(const std::string& name)
    {
        const auto iter = registry_.find(name);
        if (iter == registry_.end()) {
            throw UnknownContext(name);
        }
        current_ = &iter->second;
    }

    bool hasContext(const std::string& name)
    {
        const auto iter = registry_.find(name);
        return iter != registry_.end();
    }

    typename BuilderType::PtrType getBuilder() const
    {
        assert(current_ != nullptr);
        assert(current_->builder != nullptr);
        return current_->builder;
    }

    typename PseudoBuilderType::PtrType getPseudoBuilder() const
    {
        assert(current_ != nullptr);
        assert(current_->pseudo_builder != nullptr);
        return current_->pseudo_builder;
    }

    typename DTableType::PtrType getDTable() const
    {
        assert(current_ != nullptr);
        assert(current_->dtrie != nullptr);
        return current_->dtrie;
    }

private:
    AnnotationTypeAllocator annotation_allocator_;

    struct Context {
        typename BuilderType::PtrType           builder;
        typename PseudoBuilderType::PtrType     pseudo_builder;
        typename DTableType::PtrType            dtrie;

        Context() = default;
        Context(const Context&) = default;

        Context(AnnotationTypeAllocator& anno_allocator, const FileNameListType& isa_files, const FileNameListType& anno_files,
                const InstUIDList& uid_list = {}, const AnnotationOverrides & anno_overrides = {})
        {
            builder = std::make_shared<BuilderType>(anno_files, anno_allocator, uid_list, anno_overrides);
            dtrie   = std::make_shared<DTableType>(builder);
            dtrie->configure(isa_files);

            pseudo_builder = std::make_shared<PseudoBuilderType>(anno_files, anno_allocator, uid_list);
            pseudo_builder->configure(isa_files);
        }
    };

    std::map<std::string, Context>     registry_;
    Context                            *current_ = nullptr;
};

} // namespace mavis
