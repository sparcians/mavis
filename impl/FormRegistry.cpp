
#include "mavis/FormRegistry.h"
#include "mavis/GenericRegistry.h"
#include "impl/forms/CommonForms.h"
#include "impl/forms/CompressedForms.h"
#include "impl/forms/VectorForms.h"

#ifdef INTERNAL_FORM_REGISTRY
#include INTERNAL_FORM_REGISTRY_HEADER
#else
#define INTERNAL_FORM_REGISTRY void
#endif

namespace mavis
{
    const FormBase::PtrType & FormRegistry::findFormWrapper(const std::string & fname)
    {
        // clang-format off
        using form_registry = GenericRegistry<
            INTERNAL_FORM_REGISTRY,
            Form,
            Form_AMO,
            Form_B,
            Form_C0,
            Form_C1,
            Form_C2,
            Form_CA,
            Form_CB,
            Form_CI,
            Form_CI_rD_only,
            Form_CIW,
            Form_CIX,
            Form_CJ,
            Form_CJR,
            Form_CMPP,
            Form_CMJT,
            Form_CSR,
            Form_CSRI,
            Form_FENCE,
            Form_I,
            Form_ISH,
            Form_ISHW,
            Form_J,
            Form_R,
            Form_Rfloat,
            Form_R4,
            Form_S,
            Form_U,
            Form_V,
            Form_VF_mem,
            Form_V_vsetvl,
            Form_V_vsetvli,
            Form_V_vsetivli,
            Form_V_uimm6,
            Form_AES64KSI
        >;
        // clang-format on

        return form_registry::get(fname);
    }
} // namespace mavis
