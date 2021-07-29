#pragma once

#include "IFactory.h"

namespace mavis {

template<typename InstType, typename AnnotationType>
class IFactory_NOP : public IFactory<InstType, AnnotationType>
{
public:
    IFactory_NOP(const InstMetaData::PtrType& einfo, const typename AnnotationType::PtrType pui = nullptr) :
        IFactory<InstType, AnnotationType>("nop", 0, einfo, pui)
    {
        // NOTE: if the annotation provided is NULL, we will register a nullptr in the parent IFactory's
        // annotation map for this instruction. That's ok. This allows IFactory::findAnnotation_() to
        // take the fast path through its code
        einfo->setInstType(InstMetaData::InstructionTypes::INT, InstMetaData::InstructionTypes::ARITH);
    }

    std::string getName() const override
    {
        //FormType    f;
        std::stringstream ss;
        ss << "CUSTOM IFactory('" << IFactory<InstType, AnnotationType>::name_
           << "'): type = " << Form_I::getName();
        return ss.str();
    }

private:
#if 0
    // Custom IFactories (e.g. for non-ISA instructions such as NOP) should override
    // this method to avoid throwing an exception for a missing annotation
    const typename AnnotationType::PtrType getAnnotation_(const std::string& mnemonic) const override
    {
        return IFactory<InstType, AnnotationType>::findAnnotation_(mnemonic);
    }
#endif
};

template<typename InstType, typename AnnotationType>
class IFactory_CMOV : public IFactory<InstType, AnnotationType>
{
public:
    IFactory_CMOV(const InstMetaData::PtrType& einfo, const typename AnnotationType::PtrType pui = nullptr) :
        IFactory<InstType, AnnotationType>("cmov", 0, einfo, pui)
    {
        // NOTE: if the annotation provided is NULL, we will register a nullptr in the parent IFactory's
        // annotation map for this instruction. That's ok. This allows IFactory::findAnnotation_() to
        // take the fast path through its code
        einfo->setInstType(InstMetaData::InstructionTypes::CONDITIONAL, InstMetaData::InstructionTypes::MOVE);
    }

    std::string getName() const override
    {
        //FormType    f;
        std::stringstream ss;
        ss << "CUSTOM IFactory('" << IFactory<InstType, AnnotationType>::name_
           << "'): type = " << Form_R::getName();
        return ss.str();
    }

private:
#if 0
    // Custom IFactories (e.g. for non-ISA instructions such as CMOV) should override
    // this method to avoid throwing an exception for a missing annotation
    const typename AnnotationType::PtrType getAnnotation_(const std::string& mnemonic) const override
    {
        return IFactory<InstType, AnnotationType>::findAnnotation_(mnemonic);
    }
#endif
};

} // namespace mavis
