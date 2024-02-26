#pragma once

#include "mavis/DTable.h" // Needed for CLION
#include "mavis/FormCommon.h"

namespace mavis {

/**
 * DTableBranchBuilder: this is used for doing special "manual" builds of branches. The class wrapper
 * to populate() allows us to partially specialize the template (e.g. specializing on Form but not on InstType)
 * (templated functions cannot be partially specialized in C++17)
 *
 * @tparam FormType
 * @param mnemonic
 * @param istencil The hard-coded bits of the instruction (opcode + func bits)
 */
template<typename InstType, typename AnnotationType, typename FormType>
class DTableBranchBuilder;

//typedef typename std::set<std::string> FieldNameSetType;

template<typename InstType, typename AnnotationType>
class DTableBranchBuilder<InstType, AnnotationType, Form_R>
{
public:
    // IFactoryIF<InstType, AnnotationType> *
    // operator()(const IFactoryIF<InstType, AnnotationType> *root, const Opcode istencil, const FieldNameSetType &ignore_set);
    static inline typename IFactoryIF<InstType, AnnotationType>::PtrType
    populate(typename IFactoryIF<InstType, AnnotationType>::PtrType root,
             Opcode istencil,
             const FieldNameSetType &ignore_set);
};

/**
 * DTable::build with special branch nodes
 * @tparam FormType
 * @param mnemonic
 * @param istencil
 */
template<typename InstType, typename AnnotationType, typename AnnotationTypeAllocator>
template<typename FormType>
void DTable<InstType, AnnotationType, AnnotationTypeAllocator>::buildSpecial_(const std::string &mnemonic, const Opcode istencil,
                                                                              const FieldNameListType &flist,
                                                                              const FieldNameSetType &ignore_set,
                                                                              const std::string &factory_name, const std::string &xpand_name,
                                                                              const ExtractorIF::PtrType &override_extractor,
                                                                              const InstMetaData::PtrType &einfo)
{
    //IFactoryIF<InstType, AnnotationType> *currNode = DTableBranchBuilder<InstType, FormType>()(root_, istencil, ignore_set);
    IFactoryIF<InstType, AnnotationType> *currNode = DTableBranchBuilder<InstType, AnnotationType, FormType>::populate(
        root_, istencil, ignore_set);
    //buildLeaf_<FormType>(currNode, mnemonic, istencil, flist, factory_name, override_extractor);
    const FormBase *form_wrap = forms_.getFormWrapper(FormType::getName());
    buildLeaf_(form_wrap, currNode, mnemonic, istencil, flist, factory_name, xpand_name, override_extractor, einfo);
}

// LEAVE THIS AS AN EXAMPLE...
/**
 * Form 'R' addIFactory
 * @param mnemonic
 * @param istencil
 */
template<typename InstType, typename AnnotationType>
typename IFactoryIF<InstType, AnnotationType>::PtrType
DTableBranchBuilder<InstType, AnnotationType, Form_R>::populate(
    typename IFactoryIF<InstType, AnnotationType>::PtrType root,
    const Opcode istencil, const FieldNameSetType &ignore_set)
{
    typename IFactoryIF<InstType, AnnotationType>::PtrType currNode = root;

    // At ROOT node...
    if (currNode->getNode(istencil) == nullptr) {
        currNode->addIFactory(istencil, typename IFactoryIF<InstType, AnnotationType>::PtrType(
            new IFactoryDenseComposite<InstType, AnnotationType>(Form<Form_R>::getField(Form_R::OPCODE))));
    }
    currNode = currNode->getNode(istencil); // Advance...
    assert(currNode->getField() != nullptr);
    assert(currNode->getField()->isEquivalent(Form<Form_R>::getField(Form_R::OPCODE)));

    // At OPCODE node...
    if (currNode->getNode(istencil) == nullptr) {
        currNode->addIFactory(istencil, typename IFactoryIF<InstType, AnnotationType>::PtrType(
            new IFactoryDenseComposite<InstType, AnnotationType>(Form<Form_R>::getField(Form_R::FUNC3))));
    }
    currNode = currNode->getNode(istencil); // Advance...
    assert(currNode->getField() != nullptr);
    assert(currNode->getField()->isEquivalent(Form<Form_R>::getField(Form_R::FUNC3)));

    // At FUNC3 node...
    if (ignore_set.find(Form<Form_R>::getField(Form_R::FUNC3).getName()) != ignore_set.end()) {
        if (currNode->getDefault() == nullptr) {
            currNode->addDefaultIFactory(typename IFactoryIF<InstType, AnnotationType>::PtrType(
                new IFactoryDenseComposite<InstType, AnnotationType>(Form<Form_R>::getField(Form_R::FUNC7))));
        }
        currNode = currNode->getDefault(); // Advance...
    } else {
        if (currNode->getNode(istencil) == nullptr) {
            currNode->addIFactory(istencil, typename IFactoryIF<InstType, AnnotationType>::PtrType(
                new IFactoryDenseComposite<InstType, AnnotationType>(Form<Form_R>::getField(Form_R::FUNC7))));
        }
        currNode = currNode->getNode(istencil); // Advance...
    }
    assert(currNode->getField() != nullptr);
    assert(currNode->getField()->isEquivalent(Form<Form_R>::getField(Form_R::FUNC7)));

    // At FUNC7 node...
    if (ignore_set.find(Form<Form_R>::getField(Form_R::FUNC7).getName()) != ignore_set.end()) {
        if (currNode->getDefault() == nullptr) {
            currNode->addDefaultIFactory(
                typename IFactoryIF<InstType, AnnotationType>::PtrType(
                    new IFactorySpecialCaseComposite<InstType, AnnotationType>()));
        }
        currNode = currNode->getDefault(); // Advance...
    } else {
        if (currNode->getNode(istencil) == nullptr) {
            currNode->addIFactory(istencil,
                                  typename IFactoryIF<InstType, AnnotationType>::PtrType(
                                      new IFactorySpecialCaseComposite<InstType, AnnotationType>()));
        }
        currNode = currNode->getNode(istencil); // Advance...
    }

    return currNode;
}

} // namespace mavis
