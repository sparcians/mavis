#pragma once

#include "Form.h"

namespace mavis {

/**
 * Stub for I_mv extractor (mv overlaid on addi)
 */
class Form_I_mv
{
private:
    static inline const std::string name_ {"I_mv"};
};

/**
 * Stub for I_load extractor
 */
class Form_I_load
{
private:
    static inline const std::string name_ {"I_load"};
};

/**
 * Stub for C0_load extractor
 */
class Form_C0_load
{
private:
    static inline const std::string name_ {"C0_load"};
};

/**
 * Stub for C0_load_byte extractor (ZCB extension)
 */
class Form_C0_load_byte
{
private:
    static inline const std::string name_ {"C0_load_byte"};
};

/**
 * Stub for C0_load_half extractor (ZCB extension)
 */
class Form_C0_load_half
{
private:
    static inline const std::string name_ {"C0_load_half"};
};

/**
 * Stub for C0_load_word extractor
 */
class Form_C0_load_word
{
private:
    static inline const std::string name_ {"C0_load_word"};
};

/**
 * Stub for C0_load_double extractor
 */
class Form_C0_load_double
{
private:
    static inline const std::string name_ {"C0_load_double"};
};

/**
 * Stub for C0_store extractor
 */
class Form_C0_store
{
private:
    static inline const std::string name_ {"C0_store"};
};

/**
 * Stub for C0_store_byte extractor (ZCB extension)
 */
class Form_C0_store_byte
{
private:
    static inline const std::string name_ {"C0_store_byte"};
};

/**
 * Stub for C0_store_half extractor (ZCB extension)
 */
class Form_C0_store_half
{
private:
    static inline const std::string name_ {"C0_store_half"};
};

/**
 * Stub for C0_store_word extractor
 */
class Form_C0_store_word
{
private:
    static inline const std::string name_ {"C0_store_word"};
};

/**
 * Stub for C0_store_double extractor
 */
class Form_C0_store_double
{
private:
    static inline const std::string name_ {"C0_store_double"};
};

/**
 * Stub for C1_rsd extractor
 */
class Form_C1_rsd
{
private:
    static inline const std::string name_ {"C1_rsd"};
};

/**
 * Stub for C1_rsd_I0 extractor
 */
class Form_C1_rsd_I0
{
private:
    static inline const std::string name_ {"C1_rsd_I0"};
};

/**
 * Stub for C1_rsd_zext_I0 extractor
 */
class Form_C1_rsd_zext_I0
{
private:
    static inline const std::string name_ {"C1_rsd_zext_I0"};
};

/**
 * Stub for C1_rsd_Ineg1 extractor
 */
class Form_C1_rsd_Ineg1
{
private:
    static inline const std::string name_ {"C1_rsd_Ineg1"};
};

/**
 * Stub for C1_rsd_I0xFF extractor
 */
class Form_C1_rsd_I0xFF
{
private:
    static inline const std::string name_ {"C1_rsd_I0xFF"};
};

/**
 * Stub for CI_sp extractor
 */
class Form_CI_sp
{
private:
    static inline const std::string name_ {"CI_sp"};
};

/**
 * Stub for CIW_sp extractor
 */
class Form_CIW_sp
{
private:
    static inline const std::string name_ {"CIW_sp"};
};

/**
 * Stub for CJALR extractor
 */
class Form_CJALR
{
private:
    static inline const std::string name_ {"CJALR"};
};

/**
 * Stub for C2_add extractor
 */
class Form_C2_add
{
private:
    static inline const std::string name_ {"C2_add"};
};

/**
 * Stub for C2_mv extractor
 */
class Form_C2_mv
{
private:
    static inline const std::string name_ {"C2_mv"};
};

/**
 * Stub for C2_slli extractor
 */
class Form_C2_slli
{
private:
    static inline const std::string name_ {"C2_slli"};
};

/**
 * Stub for C2_sp extractor
 */
class Form_C2_sp
{
private:
    static inline const std::string name_ {"C2_sp"};
};

/**
 * Stub for C2_sp_load extractor
 */
class Form_C2_sp_load
{
private:
    static inline const std::string name_ {"C2_sp_load"};
};

/**
 * Stub for C2_sp_load_word extractor
 */
class Form_C2_sp_load_word
{
private:
    static inline const std::string name_ {"C2_sp_load_word"};
};

/**
 * Stub for C2_sp_load_double extractor
 */
class Form_C2_sp_load_double
{
private:
    static inline const std::string name_ {"C2_sp_load_double"};
};

/**
 * Stub for C2_sp_load_float_single extractor
 */
class Form_C2_sp_load_float_single
{
private:
    static inline const std::string name_ {"C2_sp_load_float_single"};
};

/**
 * Stub for C2_sp_load_float_double extractor
 */
class Form_C2_sp_load_float_double
{
private:
    static inline const std::string name_ {"C2_sp_load_float_double"};
};

/**
 * Stub for C2_sp_store_word extractor
 */
class Form_C2_sp_store_word
{
private:
    static inline const std::string name_ {"C2_sp_store_word"};
};

/**
 * Stub for C2_sp_store_double extractor
 */
class Form_C2_sp_store_double
{
private:
    static inline const std::string name_ {"C2_sp_store_double"};
};

/**
 * Stub for CI_addi extractor
 */
class Form_CI_addi
{
private:
    static inline const std::string name_ {"CI_addi"};
};

/**
 * Stub for CI_addiw extractor
 */
class Form_CI_addiw
{
private:
    static inline const std::string name_ {"CI_addiw"};
};

/**
 * Stub for CI_rD_shifted extractor
 */
class Form_CI_rD_shifted
{
private:
    static inline const std::string name_ {"CI_rD_shifted"};
};

/**
 * Stub for CIX_andi extractor
 */
class Form_CIX_andi
{
private:
    static inline const std::string name_ {"CIX_andi"};
};

/**
 * Stub for V_load extractor
 */
class Form_V_load
{
private:
    static inline const std::string name_ {"V_load"};
};

/**
 * Stub for V_store extractor
 */
class Form_V_store
{
private:
    static inline const std::string name_ {"V_store"};
};

/**
 * Stub for V_simm extractor
 */
class Form_V_simm
{
private:
    static inline const std::string name_ {"V_simm"};
};

/**
 * Stub for V_uimm extractor
 */
class Form_V_uimm
{
private:
    static inline const std::string name_ {"V_uimm"};
};

/**
 * Stub for V_op extractor
 */
class Form_V_op
{
private:
    static inline const std::string name_ {"V_op"};
};

/**
 * Stub for V_implied extractor
 */
class Form_V_implied
{
private:
    static inline const std::string name_ {"V_implied"};
};

/**
 * Stub for V_op_implied extractor
 */
class Form_V_op_implied
{
private:
    static inline const std::string name_ {"V_op_implied"};
};

/**
 * Stub for V_uimm_implied extractor
 */
class Form_V_uimm_implied
{
private:
    static inline const std::string name_ {"V_uimm_implied"};
};

/**
 * Stub for HV_load extractor
 */
class Form_HV_load
{
private:
    static inline const std::string name_ {"HV_load"};
};

/**
 * Stub for HV_store extractor
 */
class Form_HV_store
{
private:
    static inline const std::string name_ {"HV_store"};
};

/**
 * Stub for NTL_hint extractor
 */
class Form_NTL_hint
{
private:
    static inline const std::string name_ {"NTL_hint"};
};

/**
 * Stub for PF_hint extractor
 */
class Form_PF_hint
{
private:
    static inline const std::string name_ {"PF_hint"};
};

} // namespace mavis
