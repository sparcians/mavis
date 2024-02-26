
#include "ExtractorIF.h"
#include "ExtractorDerived.tpp"
#include "ExtractorForms.tpp"
#include "ExtractorRegistry.h"

namespace mavis {

    const ExtractorIF::PtrType &ExtractorRegistry::getExtractor(const std::string &fname)
    {
        static const std::map<std::string, ExtractorIF::PtrType> EXTRACTOR_REGISTRY = {
            {Form<Form_AMO>::getName(),                      std::make_shared<Extractor<Form<Form_AMO>>>()},
            {Form<Form_B>::getName(),                        std::make_shared<Extractor<Form<Form_B>>>()},
            {Form<Form_C0>::getName(),                       std::make_shared<Extractor<Form<Form_C0>>>()},
            {Form<Form_C0_load_double>::getName(),           std::make_shared<Extractor<Form<Form_C0_load_double>>>()},
            {Form<Form_C0_load_word>::getName(),             std::make_shared<Extractor<Form<Form_C0_load_word>>>()},
            {Form<Form_C0_load_byte>::getName(),             std::make_shared<Extractor<Form<Form_C0_load_byte>>>()},
            {Form<Form_C0_load_half>::getName(),             std::make_shared<Extractor<Form<Form_C0_load_half>>>()},
            //{Form<Form_C0_store>::getName(),               std::make_shared<Extractor<Form<Form_C0_store>>>()},
            {Form<Form_C0_store_double>::getName(),          std::make_shared<Extractor<Form<Form_C0_store_double>>>()},
            {Form<Form_C0_store_word>::getName(),            std::make_shared<Extractor<Form<Form_C0_store_word>>>()},
            {Form<Form_C0_store_byte>::getName(),            std::make_shared<Extractor<Form<Form_C0_store_byte>>>()},
            {Form<Form_C0_store_half>::getName(),            std::make_shared<Extractor<Form<Form_C0_store_half>>>()},
            {Form<Form_C1_rsd>::getName(),                   std::make_shared<Extractor<Form<Form_C1_rsd>>>()},
            {Form<Form_C1_rsd_I0>::getName(),                std::make_shared<Extractor<Form<Form_C1_rsd_I0>>>()},
            {Form<Form_C1_rsd_zext_I0>::getName(),           std::make_shared<Extractor<Form<Form_C1_rsd_zext_I0>>>()},
            {Form<Form_C1_rsd_Ineg1>::getName(),             std::make_shared<Extractor<Form<Form_C1_rsd_Ineg1>>>()},
            {Form<Form_C1_rsd_I0xFF>::getName(),             std::make_shared<Extractor<Form<Form_C1_rsd_I0xFF>>>()},
            {Form<Form_C2>::getName(),                       std::make_shared<Extractor<Form<Form_C2>>>()},
            {Form<Form_C2_add>::getName(),                   std::make_shared<Extractor<Form<Form_C2_add>>>()},
            {Form<Form_C2_mv>::getName(),                    std::make_shared<Extractor<Form<Form_C2_mv>>>()},
            {Form<Form_C2_slli>::getName(),                  std::make_shared<Extractor<Form<Form_C2_slli>>>()},
            {Form<Form_C2_sp>::getName(),                    std::make_shared<Extractor<Form<Form_C2_sp>>>()},
            {Form<Form_C2_sp_load_double>::getName(),        std::make_shared<Extractor<Form<Form_C2_sp_load_double>>>()},
            {Form<Form_C2_sp_load_float_single>::getName(),  std::make_shared<Extractor<Form<Form_C2_sp_load_float_single>>>()},
            {Form<Form_C2_sp_load_float_double>::getName(),  std::make_shared<Extractor<Form<Form_C2_sp_load_float_double>>>()},
            {Form<Form_C2_sp_load_word>::getName(),          std::make_shared<Extractor<Form<Form_C2_sp_load_word>>>()},
            {Form<Form_C2_sp_store_double>::getName(),       std::make_shared<Extractor<Form<Form_C2_sp_store_double>>>()},
            {Form<Form_C2_sp_store_word>::getName(),         std::make_shared<Extractor<Form<Form_C2_sp_store_word>>>()},
            {Form<Form_CA>::getName(),                       std::make_shared<Extractor<Form<Form_CA>>>()},
            {Form<Form_CB>::getName(),                       std::make_shared<Extractor<Form<Form_CB>>>()},
            {Form<Form_CI>::getName(),                       std::make_shared<Extractor<Form<Form_CI>>>()},
            {Form<Form_CI_addi>::getName(),                  std::make_shared<Extractor<Form<Form_CI_addi>>>()},
            {Form<Form_CI_addiw>::getName(),                 std::make_shared<Extractor<Form<Form_CI_addiw>>>()},
            {Form<Form_CI_rD_only>::getName(),               std::make_shared<Extractor<Form<Form_CI_rD_only>>>()},
            {Form<Form_CI_rD_shifted>::getName(),            std::make_shared<Extractor<Form<Form_CI_rD_shifted>>>()},
            {Form<Form_CI_sp>::getName(),                    std::make_shared<Extractor<Form<Form_CI_sp>>>()},
            {Form<Form_CIW>::getName(),                      std::make_shared<Extractor<Form<Form_CIW>>>()},
            {Form<Form_CIW_sp>::getName(),                   std::make_shared<Extractor<Form<Form_CIW_sp>>>()},
            {Form<Form_CIX>::getName(),                      std::make_shared<Extractor<Form<Form_CIX>>>()},
            {Form<Form_CIX_andi>::getName(),                 std::make_shared<Extractor<Form<Form_CIX_andi>>>()},
            {Form<Form_CJ>::getName(),                       std::make_shared<Extractor<Form<Form_CJ>>>()},
            {Form<Form_CJR>::getName(),                      std::make_shared<Extractor<Form<Form_CJR>>>()},
            {Form<Form_CJALR>::getName(),                    std::make_shared<Extractor<Form<Form_CJALR>>>()},
            {Form<Form_CSR>::getName(),                      std::make_shared<Extractor<Form<Form_CSR>>>()},
            {Form<Form_CSRI>::getName(),                     std::make_shared<Extractor<Form<Form_CSRI>>>()},
            {Form<Form_FENCE>::getName(),                    std::make_shared<Extractor<Form<Form_FENCE>>>()},
            {Form<Form_HV_load>::getName(),                  std::make_shared<Extractor<Form<Form_HV_load>>>()},
            {Form<Form_HV_store>::getName(),                 std::make_shared<Extractor<Form<Form_HV_store>>>()},
            {Form<Form_I>::getName(),                        std::make_shared<Extractor<Form<Form_I>>>()},
            {Form<Form_I_load>::getName(),                   std::make_shared<Extractor<Form<Form_I_load>>>()},
            {Form<Form_I_mv>::getName(),                     std::make_shared<Extractor<Form<Form_I_mv>>>()},
            {Form<Form_ISH>::getName(),                      std::make_shared<Extractor<Form<Form_ISH>>>()},
            {Form<Form_ISHW>::getName(),                     std::make_shared<Extractor<Form<Form_ISHW>>>()},
            {Form<Form_J>::getName(),                        std::make_shared<Extractor<Form<Form_J>>>()},
            {Form<Form_NTL_hint>::getName(),                 std::make_shared<Extractor<Form<Form_NTL_hint>>>()},
            {Form<Form_PF_hint>::getName(),                  std::make_shared<Extractor<Form<Form_PF_hint>>>()},
            {Form<Form_R>::getName(),                        std::make_shared<Extractor<Form<Form_R>>>()},
            {Form<Form_Rfloat>::getName(),                   std::make_shared<Extractor<Form<Form_Rfloat>>>()},
            {Form<Form_R4>::getName(),                       std::make_shared<Extractor<Form<Form_R4>>>()},
            {Form<Form_S>::getName(),                        std::make_shared<Extractor<Form<Form_S>>>()},
            {Form<Form_U>::getName(),                        std::make_shared<Extractor<Form<Form_U>>>()},
            {Form<Form_V>::getName(),                        std::make_shared<Extractor<Form<Form_V>>>()},
            {Form<Form_VF_mem>::getName(),                   std::make_shared<Extractor<Form<Form_VF_mem>>>()},
            {Form<Form_V_load>::getName(),                   std::make_shared<Extractor<Form<Form_V_load>>>()},
            {Form<Form_V_store>::getName(),                  std::make_shared<Extractor<Form<Form_V_store>>>()},
            {Form<Form_V_vsetvl>::getName(),                 std::make_shared<Extractor<Form<Form_V_vsetvl>>>()},
            {Form<Form_V_vsetvli>::getName(),                std::make_shared<Extractor<Form<Form_V_vsetvli>>>()},
            {Form<Form_V_vsetivli>::getName(),               std::make_shared<Extractor<Form<Form_V_vsetivli>>>()},
            {Form<Form_V_simm>::getName(),                   std::make_shared<Extractor<Form<Form_V_simm>>>()},
            {Form<Form_V_uimm>::getName(),                   std::make_shared<Extractor<Form<Form_V_uimm>>>()},
            {Form<Form_V_uimm6>::getName(),                  std::make_shared<Extractor<Form<Form_V_uimm6>>>()},
            {Form<Form_V_op>::getName(),                     std::make_shared<Extractor<Form<Form_V_op>>>()},
            {Form<Form_V_implied>::getName(),                std::make_shared<Extractor<Form<Form_V_implied>>>()},
            {Form<Form_V_op_implied>::getName(),             std::make_shared<Extractor<Form<Form_V_op_implied>>>()},
            {Form<Form_V_uimm_implied>::getName(),           std::make_shared<Extractor<Form<Form_V_uimm_implied>>>()}
        };

        const auto itr = EXTRACTOR_REGISTRY.find(fname);
        if (itr == EXTRACTOR_REGISTRY.end()) {
            throw BuildErrorUnknownForm(fname);
        }
        return itr->second;
    }

} // namespace mavis
