
#include "impl/Form.h"
#include "FormCommon.h"
#include "FormRegistry.h"
#include <iostream>

namespace mavis {

    static const std::map<std::string, const FormBase*> form_registry_ = {
        {Form<Form_AMO>::getName(),        new Form<Form_AMO>()},
        {Form<Form_B>::getName(),          new Form<Form_B>()},
        {Form<Form_C0>::getName(),         new Form<Form_C0>()},
        {Form<Form_C1>::getName(),         new Form<Form_C1>()},
        {Form<Form_C2>::getName(),         new Form<Form_C2>()},
        {Form<Form_CA>::getName(),         new Form<Form_CA>()},
        {Form<Form_CB>::getName(),         new Form<Form_CB>()},
        {Form<Form_CI>::getName(),         new Form<Form_CI>()},
        {Form<Form_CI_rD_only>::getName(), new Form<Form_CI_rD_only>()},
        {Form<Form_CIW>::getName(),        new Form<Form_CIW>()},
        {Form<Form_CIX>::getName(),        new Form<Form_CIX>()},
        {Form<Form_CJ>::getName(),         new Form<Form_CJ>()},
        {Form<Form_CJR>::getName(),        new Form<Form_CJR>()},
        {Form<Form_CSR>::getName(),        new Form<Form_CSR>()},
        {Form<Form_CSRI>::getName(),       new Form<Form_CSRI>()},
        {Form<Form_FENCE>::getName(),      new Form<Form_FENCE>()},
        {Form<Form_I>::getName(),          new Form<Form_I>()},
        {Form<Form_ISH>::getName(),        new Form<Form_ISH>()},
        {Form<Form_ISHW>::getName(),       new Form<Form_ISHW>()},
        {Form<Form_J>::getName(),          new Form<Form_J>()},
        {Form<Form_R>::getName(),          new Form<Form_R>()},
        {Form<Form_Rfloat>::getName(),     new Form<Form_Rfloat>()},
        {Form<Form_R4>::getName(),         new Form<Form_R4>()},
        {Form<Form_S>::getName(),          new Form<Form_S>()},
        {Form<Form_U>::getName(),          new Form<Form_U>()},
        {Form<Form_V>::getName(),          new Form<Form_V>()},
        {Form<Form_VF_mem>::getName(),     new Form<Form_VF_mem>()},
        {Form<Form_V_vsetvl>::getName(),   new Form<Form_V_vsetvl>()},
        {Form<Form_V_vsetvli>::getName(),  new Form<Form_V_vsetvli>()},
        {Form<Form_V_vsetivli>::getName(), new Form<Form_V_vsetivli>()},
        {Form<Form_V_uimm6>::getName(),    new Form<Form_V_uimm6>()}
    };

    const FormBase* FormRegistry::findFormWrapper(const std::string& fname)
    {
        const auto itr = form_registry_.find(fname);
        if (itr != form_registry_.end()) {
            return itr->second;
        } else {
            return nullptr;
        }
    }

    const FormBase* FormRegistry::getFormWrapper(const std::string& fname)
    {
        const auto itr = form_registry_.find(fname);
        if (itr == form_registry_.end()) {
            throw BuildErrorUnknownForm(fname);
        }
        return itr->second;
    }

} // namespace mavis
