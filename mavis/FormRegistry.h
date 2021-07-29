#pragma once

#include "Form.h"

namespace mavis {

/**
 * FormRegistry
 */
class FormRegistry
{
private:
    static inline const std::map<std::string, const FormWrapperIF*> registry_ = {
        {Form_AMO::getName(),        new FormWrapper<Form_AMO>()},
        {Form_B::getName(),          new FormWrapper<Form_B>()},
        {Form_C0::getName(),         new FormWrapper<Form_C0>()},
        {Form_C1::getName(),         new FormWrapper<Form_C1>()},
        {Form_C2::getName(),         new FormWrapper<Form_C2>()},
        {Form_CA::getName(),         new FormWrapper<Form_CA>()},
        {Form_CB::getName(),         new FormWrapper<Form_CB>()},
        {Form_CI::getName(),         new FormWrapper<Form_CI>()},
        {Form_CI_rD_only::getName(), new FormWrapper<Form_CI_rD_only>()},
        {Form_CIW::getName(),        new FormWrapper<Form_CIW>()},
        {Form_CIX::getName(),        new FormWrapper<Form_CIX>()},
        {Form_CJ::getName(),         new FormWrapper<Form_CJ>()},
        {Form_CJR::getName(),        new FormWrapper<Form_CJR>()},
        {Form_CSR::getName(),        new FormWrapper<Form_CSR>()},
        {Form_CSRI::getName(),       new FormWrapper<Form_CSRI>()},
        {Form_FENCE::getName(),      new FormWrapper<Form_FENCE>()},
        {Form_I::getName(),          new FormWrapper<Form_I>()},
        {Form_ISH::getName(),        new FormWrapper<Form_ISH>()},
        {Form_ISHW::getName(),       new FormWrapper<Form_ISHW>()},
        {Form_J::getName(),          new FormWrapper<Form_J>()},
        {Form_R::getName(),          new FormWrapper<Form_R>()},
        {Form_Rfloat::getName(),     new FormWrapper<Form_Rfloat>()},
        {Form_R4::getName(),         new FormWrapper<Form_R4>()},
        {Form_S::getName(),          new FormWrapper<Form_S>()},
        {Form_U::getName(),          new FormWrapper<Form_U>()},
        {Form_V::getName(),          new FormWrapper<Form_V>()},
        {Form_VF_mem::getName(),     new FormWrapper<Form_VF_mem>()},
        {Form_V_vsetvl::getName(),   new FormWrapper<Form_V_vsetvl>()},
        {Form_V_vsetvli::getName(),  new FormWrapper<Form_V_vsetvli>()},
        {Form_V_vsetivli::getName(), new FormWrapper<Form_V_vsetivli>()}
    };

public:
    static inline const FormWrapperIF* findFormWrapper(const std::string& fname)
    {
        const auto itr = registry_.find(fname);
        if (itr != registry_.end()) {
            return itr->second;
        } else {
            return nullptr;
        }
    }

    static inline const FormWrapperIF* getFormWrapper(const std::string& fname)
    {
        const auto itr = registry_.find(fname);
        if (itr == registry_.end()) {
            throw BuildErrorUnknownForm(fname);
        }
        return itr->second;
    }
};

} // namespace mavis
