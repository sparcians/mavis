#pragma once

#include "Extractor.h"
#include "ExtractorDerived.h"

namespace mavis {
/**
 * ExtractorRegistry
 */
class ExtractorRegistry
{
private:
    static inline const std::map<std::string, ExtractorIF::PtrType> registry_ = {
        {Form_AMO::getName(),               ExtractorIF::PtrType(new Extractor<Form_AMO>())},
        {Form_B::getName(),                 ExtractorIF::PtrType(new Extractor<Form_B>())},
        {Form_C0::getName(),                ExtractorIF::PtrType(new Extractor<Form_C0>())},
        {Form_C0_load_double::getName(),    ExtractorIF::PtrType(new Extractor<Form_C0_load_double>())},
        {Form_C0_load_word::getName(),      ExtractorIF::PtrType(new Extractor<Form_C0_load_word>())},
        //{Form_C0_store::getName(),          ExtractorIF::PtrType(new Extractor<Form_C0_store>())},
        {Form_C0_store_double::getName(),   ExtractorIF::PtrType(new Extractor<Form_C0_store_double>())},
        {Form_C0_store_word::getName(),     ExtractorIF::PtrType(new Extractor<Form_C0_store_word>())},
        {Form_C2::getName(),                ExtractorIF::PtrType(new Extractor<Form_C2>())},
        {Form_C2_add::getName(),            ExtractorIF::PtrType(new Extractor<Form_C2_add>())},
        {Form_C2_mv::getName(),             ExtractorIF::PtrType(new Extractor<Form_C2_mv>())},
        {Form_C2_slli::getName(),           ExtractorIF::PtrType(new Extractor<Form_C2_slli>())},
        {Form_C2_sp::getName(),             ExtractorIF::PtrType(new Extractor<Form_C2_sp>())},
        {Form_C2_sp_load_double::getName(), ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_double>())},
        {Form_C2_sp_load_float_single::getName(),  ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_float_single>())},
        {Form_C2_sp_load_float_double::getName(),  ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_float_double>())},
        {Form_C2_sp_load_word::getName(),   ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_word>())},
        {Form_C2_sp_store_double::getName(),ExtractorIF::PtrType(new Extractor<Form_C2_sp_store_double>())},
        {Form_C2_sp_store_word::getName(),  ExtractorIF::PtrType(new Extractor<Form_C2_sp_store_word>())},
        {Form_CA::getName(),                ExtractorIF::PtrType(new Extractor<Form_CA>())},
        {Form_CB::getName(),                ExtractorIF::PtrType(new Extractor<Form_CB>())},
        {Form_CI::getName(),                ExtractorIF::PtrType(new Extractor<Form_CI>())},
        {Form_CI_addi::getName(),           ExtractorIF::PtrType(new Extractor<Form_CI_addi>())},
        {Form_CI_addiw::getName(),          ExtractorIF::PtrType(new Extractor<Form_CI_addiw>())},
        {Form_CI_rD_only::getName(),        ExtractorIF::PtrType(new Extractor<Form_CI_rD_only>())},
        {Form_CI_rD_shifted::getName(),     ExtractorIF::PtrType(new Extractor<Form_CI_rD_shifted>())},
        {Form_CI_sp::getName(),             ExtractorIF::PtrType(new Extractor<Form_CI_sp>())},
        {Form_CIW::getName(),               ExtractorIF::PtrType(new Extractor<Form_CIW>())},
        {Form_CIW_sp::getName(),            ExtractorIF::PtrType(new Extractor<Form_CIW_sp>())},
        {Form_CIX::getName(),               ExtractorIF::PtrType(new Extractor<Form_CIX>())},
        {Form_CIX_andi::getName(),          ExtractorIF::PtrType(new Extractor<Form_CIX_andi>())},
        {Form_CJ::getName(),                ExtractorIF::PtrType(new Extractor<Form_CJ>())},
        {Form_CJR::getName(),               ExtractorIF::PtrType(new Extractor<Form_CJR>())},
        {Form_CJALR::getName(),             ExtractorIF::PtrType(new Extractor<Form_CJALR>())},
        {Form_CSR::getName(),               ExtractorIF::PtrType(new Extractor<Form_CSR>())},
        {Form_CSRI::getName(),              ExtractorIF::PtrType(new Extractor<Form_CSRI>())},
        {Form_FENCE::getName(),             ExtractorIF::PtrType(new Extractor<Form_FENCE>())},
        {Form_HV_load::getName(),           ExtractorIF::PtrType(new Extractor<Form_HV_load>())},
        {Form_HV_store::getName(),          ExtractorIF::PtrType(new Extractor<Form_HV_store>())},
        {Form_I::getName(),                 ExtractorIF::PtrType(new Extractor<Form_I>())},
        {Form_I_load::getName(),            ExtractorIF::PtrType(new Extractor<Form_I_load>())},
        {Form_I_mv::getName(),              ExtractorIF::PtrType(new Extractor<Form_I_mv>())},
        {Form_ISH::getName(),               ExtractorIF::PtrType(new Extractor<Form_ISH>())},
        {Form_ISHW::getName(),              ExtractorIF::PtrType(new Extractor<Form_ISHW>())},
        {Form_J::getName(),                 ExtractorIF::PtrType(new Extractor<Form_J>())},
        {Form_R::getName(),                 ExtractorIF::PtrType(new Extractor<Form_R>())},
        {Form_Rfloat::getName(),            ExtractorIF::PtrType(new Extractor<Form_Rfloat>())},
        {Form_R4::getName(),                ExtractorIF::PtrType(new Extractor<Form_R4>())},
        {Form_S::getName(),                 ExtractorIF::PtrType(new Extractor<Form_S>())},
        {Form_U::getName(),                 ExtractorIF::PtrType(new Extractor<Form_U>())},
        {Form_V::getName(),                 ExtractorIF::PtrType(new Extractor<Form_V>())},
        {Form_VF_mem::getName(),            ExtractorIF::PtrType(new Extractor<Form_VF_mem>())},
        {Form_V_load::getName(),            ExtractorIF::PtrType(new Extractor<Form_V_load>())},
        {Form_V_store::getName(),           ExtractorIF::PtrType(new Extractor<Form_V_store>())},
        {Form_V_vsetvl::getName(),          ExtractorIF::PtrType(new Extractor<Form_V_vsetvl>())},
        {Form_V_vsetvli::getName(),         ExtractorIF::PtrType(new Extractor<Form_V_vsetvli>())},
        {Form_V_vsetivli::getName(),        ExtractorIF::PtrType(new Extractor<Form_V_vsetivli>())},
        {Form_V_simm::getName(),            ExtractorIF::PtrType(new Extractor<Form_V_simm>())},
        {Form_V_op::getName(),              ExtractorIF::PtrType(new Extractor<Form_V_op>())}
    };

public:
    static inline const ExtractorIF::PtrType &getExtractor(const std::string &fname)
    {
        const auto itr = registry_.find(fname);
        if (itr == registry_.end()) {
            throw BuildErrorUnknownForm(fname);
        }
        return itr->second;
    }
};

} // namespace mavis
