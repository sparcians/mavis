#include "mavis/Extractor.h"
#include "mavis/ExtractorRegistry.h"
#include "mavis/GenericRegistry.h"
#include "mavis/Utils.h"
#include "impl/forms/ExtractorForms.h"
#include "impl/forms/ExtractorDerived.h"

#ifdef INTERNAL_EXTRACTOR_REGISTRY
#include INTERNAL_EXTRACTOR_REGISTRY_HEADER
#else
#define INTERNAL_EXTRACTOR_REGISTRY void
#endif

namespace mavis
{

    const ExtractorIF::PtrType & ExtractorRegistry::getExtractor(const std::string & fname)
    {
        // clang-format off
        using extractor_registry = GenericRegistry<
            INTERNAL_EXTRACTOR_REGISTRY,
            Extractor,
            Form_AMO,
            Form_AMO_pair,
            Form_B,
            Form_C0,
            Form_C0_load_double,
            Form_C0_load_word,
            Form_C0_load_word_pair,
            Form_C0_load_byte,
            Form_C0_load_half,
            //Form_C0_store,
            Form_C0_store_double,
            Form_C0_store_word,
            Form_C0_store_word_pair,
            Form_C0_store_byte,
            Form_C0_store_half,
            Form_C1_rsd,
            Form_C1_rsd_I0,
            Form_C1_rsd_zext_I0,
            Form_C1_rsd_Ineg1,
            Form_C1_rsd_I0xFF,
            Form_C2,
            Form_C2_add,
            Form_C2_mv,
            Form_C2_slli,
            Form_C2_sp,
            Form_C2_sp_load_double,
            Form_C2_sp_load_float_single,
            Form_C2_sp_load_float_double,
            Form_C2_sp_load_word,
            Form_C2_sp_load_word_pair,
            Form_C2_sp_store_double,
            Form_C2_sp_store_word,
            Form_C2_sp_store_word_pair,
            Form_CA,
            Form_CB,
            Form_CI,
            Form_CI_addi,
            Form_CI_addiw,
            Form_CI_rD_only,
            Form_CI_rD_shifted,
            Form_CI_sp,
            Form_CIW,
            Form_CIW_sp,
            Form_CIX,
            Form_CIX_andi,
            Form_CJ,
            Form_CJAL,
            Form_CJR,
            Form_CJALR,
            Form_CMPP,
            Form_CMPP_push,
            Form_CMPP_popretz,
            Form_CMMV_mva01s,
            Form_CMMV_mvsa01,
            Form_CMJT,
            Form_CSR,
            Form_CSRI,
            Form_FENCE,
            Form_HV_load,
            Form_HV_store,
            Form_I,
            Form_I_load,
            Form_I_load_pair,
            Form_I_mv,
            Form_ISH,
            Form_ISHW,
            Form_J,
            Form_MOP,
            Form_MOPC,
            Form_NTL_hint,
            Form_PF_hint,
            Form_R,
            Form_Rfloat,
            Form_Rfloat_fli_h,
            Form_Rfloat_fli_s,
            Form_Rfloat_fli_d,
            Form_Rfloat_fli_q,
            Form_R4,
            Form_S,
            Form_S_Pair,
            Form_Shadow,
            Form_U,
            Form_V,
            Form_VF_mem,
            Form_V_load,
            Form_V_store,
            Form_V_vsetvl,
            Form_V_vsetvli,
            Form_V_vsetivli,
            Form_V_simm,
            Form_V_uimm,
            Form_V_uimm6,
            Form_V_op,
            Form_V_implied,
            Form_V_op_implied,
            Form_V_uimm_implied,
            Form_AES64KSI
        >;
        // clang-format on

        try
        {
            return extractor_registry::get(fname);
        }
        catch(const RegistryNotFoundException&)
        {
            throw BuildErrorUnknownForm(fname);
        }
    }

} // namespace mavis
