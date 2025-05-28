#pragma once

#include "Form.h"

namespace mavis
{

    /**
     * Stub for I_mv extractor (mv overlaid on addi)
     */
    class Form_I_mv
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"I_mv"};
    };

    /**
     * Stub for I_load extractor
     */
    class Form_I_load
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"I_load"};
    };

    /**
     * Stub for I_load_pair extractor
     */
    class Form_I_load_pair
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"I_load_pair"};
    };

    /**
     * Stub for C0_load extractor
     */
    class Form_C0_load
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C0_load"};
    };

    /**
     * Stub for C0_load_byte extractor (ZCB extension)
     */
    class Form_C0_load_byte
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C0_load_byte"};
    };

    /**
     * Stub for C0_load_half extractor (ZCB extension)
     */
    class Form_C0_load_half
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C0_load_half"};
    };

    /**
     * Stub for C0_load_word extractor
     */
    class Form_C0_load_word
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C0_load_word"};
    };

    /**
     * Stub for C0_load_word_pair extractor
     */
    class Form_C0_load_word_pair
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C0_load_word_pair"};
    };

    /**
     * Stub for C0_load_double extractor
     */
    class Form_C0_load_double
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C0_load_double"};
    };

    /**
     * Stub for Form_S_Pair extractor
     */
    class Form_S_Pair
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"S_pair"};
    };

    /**
     * Stub for C0_store extractor
     */
    class Form_C0_store
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C0_store"};
    };

    /**
     * Stub for C0_store_byte extractor (ZCB extension)
     */
    class Form_C0_store_byte
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C0_store_byte"};
    };

    /**
     * Stub for C0_store_half extractor (ZCB extension)
     */
    class Form_C0_store_half
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C0_store_half"};
    };

    /**
     * Stub for C0_store_word extractor
     */
    class Form_C0_store_word
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C0_store_word"};
    };

    /**
     * Stub for C0_store_word_pair extractor
     */
    class Form_C0_store_word_pair
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C0_store_word_pair"};
    };

    /**
     * Stub for C0_store_double extractor
     */
    class Form_C0_store_double
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C0_store_double"};
    };

    /**
     * Stub for C1_rsd extractor
     */
    class Form_C1_rsd
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C1_rsd"};
    };

    /**
     * Stub for C1_rsd_I0 extractor
     */
    class Form_C1_rsd_I0
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C1_rsd_I0"};
    };

    /**
     * Stub for C1_rsd_zext_I0 extractor
     */
    class Form_C1_rsd_zext_I0
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C1_rsd_zext_I0"};
    };

    /**
     * Stub for C1_rsd_Ineg1 extractor
     */
    class Form_C1_rsd_Ineg1
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C1_rsd_Ineg1"};
    };

    /**
     * Stub for C1_rsd_I0xFF extractor
     */
    class Form_C1_rsd_I0xFF
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C1_rsd_I0xFF"};
    };

    /**
     * Stub for CI_sp extractor
     */
    class Form_CI_sp
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"CI_sp"};
    };

    /**
     * Stub for CIW_sp extractor
     */
    class Form_CIW_sp
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"CIW_sp"};
    };

    /**
     * Stub for CJAL extractor
     */
    class Form_CJAL
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"CJAL"};
    };

    /**
     * Stub for CJALR extractor
     */
    class Form_CJALR
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"CJALR"};
    };

    /**
     * Stub for C2_add extractor
     */
    class Form_C2_add
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_add"};
    };

    /**
     * Stub for C2_mv extractor
     */
    class Form_C2_mv
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_mv"};
    };

    /**
     * Stub for C2_slli extractor
     */
    class Form_C2_slli
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_slli"};
    };

    /**
     * Stub for C2_sp extractor
     */
    class Form_C2_sp
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_sp"};
    };

    /**
     * Stub for C2_sp_load extractor
     */
    class Form_C2_sp_load
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_sp_load"};
    };

    /**
     * Stub for C2_sp_load_word extractor
     */
    class Form_C2_sp_load_word
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_sp_load_word"};
    };

    /**
     * Stub for C2_sp_load_word_pair extractor
     */
    class Form_C2_sp_load_word_pair
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_sp_load_word_pair"};
    };

    /**
     * Stub for C2_sp_load_double extractor
     */
    class Form_C2_sp_load_double
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_sp_load_double"};
    };

    /**
     * Stub for C2_sp_load_float_single extractor
     */
    class Form_C2_sp_load_float_single
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_sp_load_float_single"};
    };

    /**
     * Stub for C2_sp_load_float_double extractor
     */
    class Form_C2_sp_load_float_double
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_sp_load_float_double"};
    };

    /**
     * Stub for C2_sp_store_word extractor
     */
    class Form_C2_sp_store_word
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_sp_store_word"};
    };

    /**
     * Stub for C2_sp_store_word_pair extractor
     */
    class Form_C2_sp_store_word_pair
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_sp_store_word_pair"};
    };

    /**
     * Stub for C2_sp_store_double extractor
     */
    class Form_C2_sp_store_double
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"C2_sp_store_double"};
    };

    /**
     * Stub for CI_addi extractor
     */
    class Form_CI_addi
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"CI_addi"};
    };

    /**
     * Stub for CI_addiw extractor
     */
    class Form_CI_addiw
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"CI_addiw"};
    };

    /**
     * Stub for CI_rD_shifted extractor
     */
    class Form_CI_rD_shifted
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"CI_rD_shifted"};
    };

    /**
     * Stub for CIX_andi extractor
     */
    class Form_CIX_andi
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"CIX_andi"};
    };

    class Form_CMPP_push
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"CMPP_push"};
    };

    class Form_CMPP_popretz
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"CMPP_popretz"};
    };

    class Form_CMMV_mva01s
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"CMMV_mva01s"};
    };

    class Form_CMMV_mvsa01
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"CMMV_mvsa01"};
    };

    /**
     * Stub for V_load extractor
     */
    class Form_V_load
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"V_load"};
    };

    /**
     * Stub for V_store extractor
     */
    class Form_V_store
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"V_store"};
    };

    /**
     * Stub for V_simm extractor
     */
    class Form_V_simm
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"V_simm"};
    };

    /**
     * Stub for V_uimm extractor
     */
    class Form_V_uimm
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"V_uimm"};
    };

    /**
     * Stub for V_op extractor
     */
    class Form_V_op
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"V_op"};
    };

    /**
     * Stub for V_implied extractor
     */
    class Form_V_implied
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"V_implied"};
    };

    /**
     * Stub for V_op_implied extractor
     */
    class Form_V_op_implied
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"V_op_implied"};
    };

    /**
     * Stub for V_uimm_implied extractor
     */
    class Form_V_uimm_implied
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"V_uimm_implied"};
    };

    /**
     * Stub for HV_load extractor
     */
    class Form_HV_load
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"HV_load"};
    };

    /**
     * Stub for HV_store extractor
     */
    class Form_HV_store
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"HV_store"};
    };

    /**
     * Stub for NTL_hint extractor
     */
    class Form_NTL_hint
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"NTL_hint"};
    };

    /**
     * Stub for PF_hint extractor
     */
    class Form_PF_hint
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"PF_hint"};
    };

    /**
     * Stub for AMO_pair extractor
     */
    class Form_AMO_pair
    {
      public:
        using idType = uint32_t;
        static inline const std::string name{"AMO_pair"};
    };

} // namespace mavis
