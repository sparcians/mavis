#include "mavis/Extractor.h"
#include "mavis/ExtractorRegistry.h"
#include "impl/forms/ExtractorForms.h"
#include "impl/forms/ExtractorDerived.h"

namespace mavis
{

    const ExtractorIF::PtrType & ExtractorRegistry::getExtractor(const std::string & fname)
    {
        static const std::map<std::string, ExtractorIF::PtrType> EXTRACTOR_REGISTRY = {
            {Form_AMO::name, std::make_shared<Extractor<Form_AMO>>()},
            {Form_AMO_pair::name, std::make_shared<Extractor<Form_AMO_pair>>()},
            {Form_B::name, std::make_shared<Extractor<Form_B>>()},
            {Form_C0::name, std::make_shared<Extractor<Form_C0>>()},
            {Form_C0_load_double::name, std::make_shared<Extractor<Form_C0_load_double>>()},
            {Form_C0_load_word::name, std::make_shared<Extractor<Form_C0_load_word>>()},
            {Form_C0_load_word_pair::name, std::make_shared<Extractor<Form_C0_load_word_pair>>()},
            {Form_C0_load_byte::name, std::make_shared<Extractor<Form_C0_load_byte>>()},
            {Form_C0_load_half::name, std::make_shared<Extractor<Form_C0_load_half>>()},
            //{Form_C0_store::name,               std::make_shared<Extractor<Form_C0_store>>()},
            {Form_C0_store_double::name, std::make_shared<Extractor<Form_C0_store_double>>()},
            {Form_C0_store_word::name, std::make_shared<Extractor<Form_C0_store_word>>()},
            {Form_C0_store_word_pair::name, std::make_shared<Extractor<Form_C0_store_word_pair>>()},
            {Form_C0_store_byte::name, std::make_shared<Extractor<Form_C0_store_byte>>()},
            {Form_C0_store_half::name, std::make_shared<Extractor<Form_C0_store_half>>()},
            {Form_C1_rsd::name, std::make_shared<Extractor<Form_C1_rsd>>()},
            {Form_C1_rsd_I0::name, std::make_shared<Extractor<Form_C1_rsd_I0>>()},
            {Form_C1_rsd_zext_I0::name, std::make_shared<Extractor<Form_C1_rsd_zext_I0>>()},
            {Form_C1_rsd_Ineg1::name, std::make_shared<Extractor<Form_C1_rsd_Ineg1>>()},
            {Form_C1_rsd_I0xFF::name, std::make_shared<Extractor<Form_C1_rsd_I0xFF>>()},
            {Form_C2::name, std::make_shared<Extractor<Form_C2>>()},
            {Form_C2_add::name, std::make_shared<Extractor<Form_C2_add>>()},
            {Form_C2_mv::name, std::make_shared<Extractor<Form_C2_mv>>()},
            {Form_C2_slli::name, std::make_shared<Extractor<Form_C2_slli>>()},
            {Form_C2_sp::name, std::make_shared<Extractor<Form_C2_sp>>()},
            {Form_C2_sp_load_double::name, std::make_shared<Extractor<Form_C2_sp_load_double>>()},
            {Form_C2_sp_load_float_single::name,
             std::make_shared<Extractor<Form_C2_sp_load_float_single>>()},
            {Form_C2_sp_load_float_double::name,
             std::make_shared<Extractor<Form_C2_sp_load_float_double>>()},
            {Form_C2_sp_load_word::name, std::make_shared<Extractor<Form_C2_sp_load_word>>()},
            {Form_C2_sp_load_word_pair::name,
             std::make_shared<Extractor<Form_C2_sp_load_word_pair>>()},
            {Form_C2_sp_store_double::name, std::make_shared<Extractor<Form_C2_sp_store_double>>()},
            {Form_C2_sp_store_word::name, std::make_shared<Extractor<Form_C2_sp_store_word>>()},
            {Form_C2_sp_store_word_pair::name,
             std::make_shared<Extractor<Form_C2_sp_store_word_pair>>()},
            {Form_CA::name, std::make_shared<Extractor<Form_CA>>()},
            {Form_CB::name, std::make_shared<Extractor<Form_CB>>()},
            {Form_CI::name, std::make_shared<Extractor<Form_CI>>()},
            {Form_CI_addi::name, std::make_shared<Extractor<Form_CI_addi>>()},
            {Form_CI_addiw::name, std::make_shared<Extractor<Form_CI_addiw>>()},
            {Form_CI_rD_only::name, std::make_shared<Extractor<Form_CI_rD_only>>()},
            {Form_CI_rD_shifted::name, std::make_shared<Extractor<Form_CI_rD_shifted>>()},
            {Form_CI_sp::name, std::make_shared<Extractor<Form_CI_sp>>()},
            {Form_CIW::name, std::make_shared<Extractor<Form_CIW>>()},
            {Form_CIW_sp::name, std::make_shared<Extractor<Form_CIW_sp>>()},
            {Form_CIX::name, std::make_shared<Extractor<Form_CIX>>()},
            {Form_CIX_andi::name, std::make_shared<Extractor<Form_CIX_andi>>()},
            {Form_CJ::name, std::make_shared<Extractor<Form_CJ>>()},
            {Form_CJAL::name, std::make_shared<Extractor<Form_CJAL>>()},
            {Form_CJR::name, std::make_shared<Extractor<Form_CJR>>()},
            {Form_CJALR::name, std::make_shared<Extractor<Form_CJALR>>()},
            {Form_CSR::name, std::make_shared<Extractor<Form_CSR>>()},
            {Form_CSRI::name, std::make_shared<Extractor<Form_CSRI>>()},
            {Form_FENCE::name, std::make_shared<Extractor<Form_FENCE>>()},
            {Form_HV_load::name, std::make_shared<Extractor<Form_HV_load>>()},
            {Form_HV_store::name, std::make_shared<Extractor<Form_HV_store>>()},
            {Form_I::name, std::make_shared<Extractor<Form_I>>()},
            {Form_I_load::name, std::make_shared<Extractor<Form_I_load>>()},
            {Form_I_load_pair::name, std::make_shared<Extractor<Form_I_load_pair>>()},
            {Form_I_mv::name, std::make_shared<Extractor<Form_I_mv>>()},
            {Form_ISH::name, std::make_shared<Extractor<Form_ISH>>()},
            {Form_ISHW::name, std::make_shared<Extractor<Form_ISHW>>()},
            {Form_J::name, std::make_shared<Extractor<Form_J>>()},
            {Form_NTL_hint::name, std::make_shared<Extractor<Form_NTL_hint>>()},
            {Form_PF_hint::name, std::make_shared<Extractor<Form_PF_hint>>()},
            {Form_R::name, std::make_shared<Extractor<Form_R>>()},
            {Form_Rfloat::name, std::make_shared<Extractor<Form_Rfloat>>()},
            {Form_R4::name, std::make_shared<Extractor<Form_R4>>()},
            {Form_S::name, std::make_shared<Extractor<Form_S>>()},
            {Form_S_Pair::name, std::make_shared<Extractor<Form_S_Pair>>()},
            {Form_U::name, std::make_shared<Extractor<Form_U>>()},
            {Form_V::name, std::make_shared<Extractor<Form_V>>()},
            {Form_VF_mem::name, std::make_shared<Extractor<Form_VF_mem>>()},
            {Form_V_load::name, std::make_shared<Extractor<Form_V_load>>()},
            {Form_V_store::name, std::make_shared<Extractor<Form_V_store>>()},
            {Form_V_vsetvl::name, std::make_shared<Extractor<Form_V_vsetvl>>()},
            {Form_V_vsetvli::name, std::make_shared<Extractor<Form_V_vsetvli>>()},
            {Form_V_vsetivli::name, std::make_shared<Extractor<Form_V_vsetivli>>()},
            {Form_V_simm::name, std::make_shared<Extractor<Form_V_simm>>()},
            {Form_V_uimm::name, std::make_shared<Extractor<Form_V_uimm>>()},
            {Form_V_uimm6::name, std::make_shared<Extractor<Form_V_uimm6>>()},
            {Form_V_op::name, std::make_shared<Extractor<Form_V_op>>()},
            {Form_V_implied::name, std::make_shared<Extractor<Form_V_implied>>()},
            {Form_V_op_implied::name, std::make_shared<Extractor<Form_V_op_implied>>()},
            {Form_V_uimm_implied::name, std::make_shared<Extractor<Form_V_uimm_implied>>()}};

        const auto itr = EXTRACTOR_REGISTRY.find(fname);
        if (itr == EXTRACTOR_REGISTRY.end())
        {
            throw BuildErrorUnknownForm(fname);
        }
        return itr->second;
    }

} // namespace mavis
