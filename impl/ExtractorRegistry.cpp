
#include "ExtractorIF.h"
#include "ExtractorDerived.tpp"
#include "ExtractorForms.tpp"
#include "ExtractorRegistry.h"

namespace mavis {

    const ExtractorIF::PtrType &ExtractorRegistry::getExtractor(const std::string &fname)
    {
        static const std::map<std::string, ExtractorIF::PtrType> EXTRACTOR_REGISTRY = {
            {Form_AMO::getName(),                       std::make_shared<Extractor<Form_AMO>>()},
            {Form_B::getName(),                         std::make_shared<Extractor<Form_B>>()},
            {Form_C0::getName(),                        std::make_shared<Extractor<Form_C0>>()},
            {Form_C0_load_double::getName(),            std::make_shared<Extractor<Form_C0_load_double>>()},
            {Form_C0_load_word::getName(),              std::make_shared<Extractor<Form_C0_load_word>>()},
            {Form_C0_load_byte::getName(),              std::make_shared<Extractor<Form_C0_load_byte>>()},
            {Form_C0_load_half::getName(),              std::make_shared<Extractor<Form_C0_load_half>>()},
            //{Form_C0_store::getName(),                  std::make_shared<Extractor<Form_C0_store>>()},
            {Form_C0_store_double::getName(),           std::make_shared<Extractor<Form_C0_store_double>>()},
            {Form_C0_store_word::getName(),             std::make_shared<Extractor<Form_C0_store_word>>()},
            {Form_C0_store_byte::getName(),             std::make_shared<Extractor<Form_C0_store_byte>>()},
            {Form_C0_store_half::getName(),             std::make_shared<Extractor<Form_C0_store_half>>()},
            {Form_C1_rsd::getName(),                    std::make_shared<Extractor<Form_C1_rsd>>()},
            {Form_C1_rsd_I0::getName(),                 std::make_shared<Extractor<Form_C1_rsd_I0>>()},
            {Form_C1_rsd_zext_I0::getName(),            std::make_shared<Extractor<Form_C1_rsd_zext_I0>>()},
            {Form_C1_rsd_Ineg1::getName(),              std::make_shared<Extractor<Form_C1_rsd_Ineg1>>()},
            {Form_C1_rsd_I0xFF::getName(),              std::make_shared<Extractor<Form_C1_rsd_I0xFF>>()},
            {Form_C2::getName(),                        std::make_shared<Extractor<Form_C2>>()},
            {Form_C2_add::getName(),                    std::make_shared<Extractor<Form_C2_add>>()},
            {Form_C2_mv::getName(),                     std::make_shared<Extractor<Form_C2_mv>>()},
            {Form_C2_slli::getName(),                   std::make_shared<Extractor<Form_C2_slli>>()},
            {Form_C2_sp::getName(),                     std::make_shared<Extractor<Form_C2_sp>>()},
            {Form_C2_sp_load_double::getName(),         std::make_shared<Extractor<Form_C2_sp_load_double>>()},
            {Form_C2_sp_load_float_single::getName(),   std::make_shared<Extractor<Form_C2_sp_load_float_single>>()},
            {Form_C2_sp_load_float_double::getName(),   std::make_shared<Extractor<Form_C2_sp_load_float_double>>()},
            {Form_C2_sp_load_word::getName(),           std::make_shared<Extractor<Form_C2_sp_load_word>>()},
            {Form_C2_sp_store_double::getName(),        std::make_shared<Extractor<Form_C2_sp_store_double>>()},
            {Form_C2_sp_store_word::getName(),          std::make_shared<Extractor<Form_C2_sp_store_word>>()},
            {Form_CA::getName(),                        std::make_shared<Extractor<Form_CA>>()},
            {Form_CB::getName(),                        std::make_shared<Extractor<Form_CB>>()},
            {Form_CI::getName(),                        std::make_shared<Extractor<Form_CI>>()},
            {Form_CI_addi::getName(),                   std::make_shared<Extractor<Form_CI_addi>>()},
            {Form_CI_addiw::getName(),                  std::make_shared<Extractor<Form_CI_addiw>>()},
            {Form_CI_rD_only::getName(),                std::make_shared<Extractor<Form_CI_rD_only>>()},
            {Form_CI_rD_shifted::getName(),             std::make_shared<Extractor<Form_CI_rD_shifted>>()},
            {Form_CI_sp::getName(),                     std::make_shared<Extractor<Form_CI_sp>>()},
            {Form_CIW::getName(),                       std::make_shared<Extractor<Form_CIW>>()},
            {Form_CIW_sp::getName(),                    std::make_shared<Extractor<Form_CIW_sp>>()},
            {Form_CIX::getName(),                       std::make_shared<Extractor<Form_CIX>>()},
            {Form_CIX_andi::getName(),                  std::make_shared<Extractor<Form_CIX_andi>>()},
            {Form_CJ::getName(),                        std::make_shared<Extractor<Form_CJ>>()},
            {Form_CJR::getName(),                       std::make_shared<Extractor<Form_CJR>>()},
            {Form_CJALR::getName(),                     std::make_shared<Extractor<Form_CJALR>>()},
            {Form_CSR::getName(),                       std::make_shared<Extractor<Form_CSR>>()},
            {Form_CSRI::getName(),                      std::make_shared<Extractor<Form_CSRI>>()},
            {Form_FENCE::getName(),                     std::make_shared<Extractor<Form_FENCE>>()},
            {Form_HV_load::getName(),                   std::make_shared<Extractor<Form_HV_load>>()},
            {Form_HV_store::getName(),                  std::make_shared<Extractor<Form_HV_store>>()},
            {Form_I::getName(),                         std::make_shared<Extractor<Form_I>>()},
            {Form_I_load::getName(),                    std::make_shared<Extractor<Form_I_load>>()},
            {Form_I_mv::getName(),                      std::make_shared<Extractor<Form_I_mv>>()},
            {Form_ISH::getName(),                       std::make_shared<Extractor<Form_ISH>>()},
            {Form_ISHW::getName(),                      std::make_shared<Extractor<Form_ISHW>>()},
            {Form_J::getName(),                         std::make_shared<Extractor<Form_J>>()},
            {Form_NTL_hint::getName(),                  std::make_shared<Extractor<Form_NTL_hint>>()},
            {Form_PF_hint::getName(),                   std::make_shared<Extractor<Form_PF_hint>>()},
            {Form_R::getName(),                         std::make_shared<Extractor<Form_R>>()},
            {Form_Rfloat::getName(),                    std::make_shared<Extractor<Form_Rfloat>>()},
            {Form_R4::getName(),                        std::make_shared<Extractor<Form_R4>>()},
            {Form_S::getName(),                         std::make_shared<Extractor<Form_S>>()},
            {Form_U::getName(),                         std::make_shared<Extractor<Form_U>>()},
            {Form_V::getName(),                         std::make_shared<Extractor<Form_V>>()},
            {Form_VF_mem::getName(),                    std::make_shared<Extractor<Form_VF_mem>>()},
            {Form_V_load::getName(),                    std::make_shared<Extractor<Form_V_load>>()},
            {Form_V_store::getName(),                   std::make_shared<Extractor<Form_V_store>>()},
            {Form_V_vsetvl::getName(),                  std::make_shared<Extractor<Form_V_vsetvl>>()},
            {Form_V_vsetvli::getName(),                 std::make_shared<Extractor<Form_V_vsetvli>>()},
            {Form_V_vsetivli::getName(),                std::make_shared<Extractor<Form_V_vsetivli>>()},
            {Form_V_simm::getName(),                    std::make_shared<Extractor<Form_V_simm>>()},
            {Form_V_uimm::getName(),                    std::make_shared<Extractor<Form_V_uimm>>()},
            {Form_V_uimm6::getName(),                   std::make_shared<Extractor<Form_V_uimm6>>()},
            {Form_V_op::getName(),                      std::make_shared<Extractor<Form_V_op>>()},
            {Form_V_implied::getName(),                 std::make_shared<Extractor<Form_V_implied>>()},
            {Form_V_op_implied::getName(),              std::make_shared<Extractor<Form_V_op_implied>>()},
            {Form_V_uimm_implied::getName(),            std::make_shared<Extractor<Form_V_uimm_implied>>()}
        };

        const auto itr = EXTRACTOR_REGISTRY.find(fname);
        if (itr == EXTRACTOR_REGISTRY.end()) {
            throw BuildErrorUnknownForm(fname);
        }
        return itr->second;
    }

} // namespace mavis
