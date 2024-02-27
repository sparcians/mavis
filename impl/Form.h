#pragma once

#include "Field.h"
#include "DecoderExceptions.h"
#include "FormIF.h"
#include "FormCommon.h"
#include <ostream>
#include <sstream>
#include <string>
#include <array>
#include <map>
#include <cinttypes>

namespace mavis {

// TODO: Need a copy of the RFloat form for FCVT (to allow for a special extractor)
/**
 * AMO-Form
 */
class Form_AMO
{
public:
    enum idType : uint32_t
    {
        FUNC5 = 0,
        AQ,
        WD,     // vector alias for AQ
        RL,
        VM,     // vector alias for RL
        RS2,
        RS1,
        FUNC3,
        WIDTH,  // vector alias for FUNC3
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"AMO"};

    static inline FieldsType fields {
        Field("func5", 27, 5),
        Field("aq", 26, 1),
        Field("wd", 26, 1),
        Field("rl", 25, 1),
        Field("vm", 25, 1),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("width", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func5",  fields[idType::FUNC5]},
        {"aq",     fields[idType::AQ]},
        {"wd",     fields[idType::WD]},
        {"rl",     fields[idType::RL]},
        {"vm",     fields[idType::VM]},
        {"rs2",    fields[idType::RS2]},
        {"rs1",    fields[idType::RS1]},
        {"func3",  fields[idType::FUNC3]},
        {"width",  fields[idType::WIDTH]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func5",  idType::FUNC5},
        {"aq",     idType::AQ},
        {"wd",     idType::WD},
        {"rl",     idType::RL},
        {"vm",     idType::VM},
        {"rs2",    idType::RS2},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"width",  idType::WIDTH},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3],
        fields[idType::FUNC5]
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};

/**
 * B-Form
 */
class Form_B
{
public:
    enum idType : uint32_t
    {
        IMM7 = 0,
        RS2,
        RS1,
        FUNC3,
        IMM5,
        OPCODE,
        __N
    };

    static inline const char * name {"B"};

    static inline FieldsType fields {
        Field("imm7", 25, 7),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("imm5", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"imm7",   fields[idType::IMM7]},
        {"rs2",    fields[idType::RS2]},
        {"rs1",    fields[idType::RS1]},
        {"func3",  fields[idType::FUNC3]},
        {"imm5",   fields[idType::IMM5]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"imm7",   idType::IMM7},
        {"rs2",    idType::RS2},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"imm5",   idType::IMM5},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3]
    };

    static inline ImmediateType immediate_type = ImmediateType::SIGNED;
};

/**
 * C0-Form: INTENDED FOR DECODING USE -- probably not useful for general extraction
 */
class Form_C0
{
public:
    enum idType : uint32_t
    {
        FUNC3 = 0,
        IMM3,
        RS1,
        FUNC2A,
        IMM2,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"C0"};

    static inline FieldsType fields {
        Field("func3", 13, 3),
        Field("imm3", 10, 3),
        Field("rs1", 7, 3),
        Field("func2A", 6, 1),
        Field("imm2", 5, 2),
        Field("rd", 2, 3),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func3",  fields[idType::FUNC3]},
        {"imm3",   fields[idType::IMM3]},
        {"rs1",    fields[idType::RS1]},
        {"func2A", fields[idType::FUNC2A]},
        {"imm2",   fields[idType::IMM2]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func3",  idType::FUNC3},
        {"imm3",   idType::IMM3},
        {"rs1",    idType::RS1},
        {"func2A", idType::FUNC2A},
        {"imm2",   idType::IMM2},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3],
        fields[idType::FUNC2A]
    };

    static inline ImmediateType immediate_type = ImmediateType::UNSIGNED;
};

/**
 * C1-Form: INTENDED FOR DECODING USE -- probably not useful for general extraction
 */
class Form_C1
{
public:
    enum idType : uint32_t
    {
        FUNC3 = 0,
        FUNC1,
        FUNC2,
        RS1,
        RD,
        FUNC2B,
        RS2,
        OPCODE,
        __N
    };

    static inline const char * name {"C1"};

    static inline FieldsType fields {
        Field("func3", 13, 3),
        Field("func1", 12, 1),
        Field("func2", 10, 2),
        Field("rs1", 7, 3), // RD and RS1 are aliases for the same field
        Field("rd", 7, 3), // RD and RS1 are aliases for the same field
        Field("func2b", 5, 2),
        Field("rs2", 2, 3),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func3",  fields[idType::FUNC3]},
        {"func1",  fields[idType::FUNC1]},
        {"func2",  fields[idType::FUNC2]},
        {"rs1",    fields[idType::RS1]},
        {"rd",     fields[idType::RD]},
        {"func2b", fields[idType::FUNC2B]},
        {"rs2",    fields[idType::RS2]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func3",  idType::FUNC3},
        {"func1",  idType::FUNC1},
        {"func2",  idType::FUNC2},
        {"rs1",    idType::RS1},
        {"rd",     idType::RD},
        {"func2b", idType::FUNC2B},
        {"rs2",    idType::RS2},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3],
        fields[idType::FUNC2],
        fields[idType::FUNC2B],
        fields[idType::FUNC1],
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};

/**
 * C2-Form: INTENDED FOR DECODING USE -- probably not useful for general extraction
 */
class Form_C2
{
public:
    enum idType : uint32_t
    {
        FUNC3 = 0,
        FUNC1,
        SHAMT1,
        RD,
        RS1,
        RS,
        RS2,
        SHAMT5,
        OPCODE,
        __N
    };

    static inline const char * name {"C2"};

    static inline FieldsType fields {
        Field("func3", 13, 3),
        Field("func1", 12, 1),
        Field("shamt1", 12, 1), // Alias for FUNC1
        Field("rd", 7, 5),
        Field("rs1", 7, 5),     // Alias for RD
        Field("rs", 2, 5),
        Field("rs2", 2, 5),     // Alias for RS
        Field("shamt5", 2, 5),  // Alias for RS
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func3",  fields[idType::FUNC3]},
        {"func1",  fields[idType::FUNC1]},
        {"shamt1", fields[idType::SHAMT1]},
        {"rd",     fields[idType::RD]},
        {"rs1",    fields[idType::RS1]},
        {"rs",     fields[idType::RS]},
        {"rs2",    fields[idType::RS2]},
        {"shamt5", fields[idType::SHAMT5]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func3",  idType::FUNC3},
        {"func1",  idType::FUNC1},
        {"shamt1", idType::SHAMT1},
        {"rd",     idType::RD},
        {"rs1",    idType::RS1},
        {"rs",     idType::RS},
        {"rs2",    idType::RS2},
        {"shamt5", idType::SHAMT5},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3],
        fields[idType::FUNC1]
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};

/**
 * C2_sp_store-Form: INTENDED FOR EXTRACTION USE ONLY
 */
class Form_C2_sp_store
{
public:
    enum idType : uint32_t
    {
        FUNC3 = 0,
        IMM,
        RS2,
        OPCODE,
        __N
    };

    static inline const char * name {"C2"};

    static inline FieldsType fields {
        Field("func3", 13, 3),
        Field("imm", 7, 6),
        Field("rs2", 2, 5),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func3",  fields[idType::FUNC3]},
        {"imm",    fields[idType::IMM]},
        {"rs2",    fields[idType::RS2]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func3",  idType::FUNC3},
        {"imm",    idType::IMM},
        {"rs2",    idType::RS2},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3]
    };

    static inline ImmediateType immediate_type = ImmediateType::UNSIGNED;
};

/**
 * CA-Form
 */
class Form_CA
{
public:
    enum idType : uint32_t
    {
        FUNC6 = 0,
        RS1,
        RD,
        FUNC2,
        RS2,
        OPCODE,
        __N
    };

    static inline const char * name {"CA"};

    static inline FieldsType fields {
        Field("func6", 10, 6),
        Field("rs1", 7, 3),
        Field("rd", 7, 3),
        Field("func2", 5, 2),
        Field("rs2", 2, 3),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func6",  fields[idType::FUNC6]},
        {"rs1",    fields[idType::RS1]},
        {"rd",     fields[idType::RD]},
        {"func2",  fields[idType::FUNC2]},
        {"rs2",    fields[idType::RS2]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func6",  idType::FUNC6},
        {"rs1",    idType::RS1},
        {"rd",     idType::RD},
        {"func2",  idType::FUNC2},
        {"rs2",    idType::RS2},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC6],
        fields[idType::FUNC2]
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};

/**
 * CB-Form
 */
class Form_CB
{
public:
    enum idType : uint32_t
    {
        FUNC3 = 0,
        IMM3,
        RS1,
        IMM5,
        OPCODE,
        __N
    };

    static inline const char * name {"CB"};

    static inline FieldsType fields {
        Field("func3", 13, 3),
        Field("imm3", 10, 3),
        Field("rs1", 7, 3),
        Field("imm5", 2, 5),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func3",  fields[idType::FUNC3]},
        {"imm3",   fields[idType::IMM3]},
        {"rs1",    fields[idType::RS1]},
        {"imm5",   fields[idType::IMM5]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func3",  idType::FUNC3},
        {"imm3",   idType::IMM3},
        {"rs1",    idType::RS1},
        {"imm5",   idType::IMM5},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3]
    };

    static inline ImmediateType immediate_type = ImmediateType::SIGNED;
};

/**
 * CI-Form
 */
class Form_CI
{
public:
    enum idType : uint32_t
    {
        FUNC3 = 0,
        IMM1,
        RS1,
        RD,
        IMM5,
        OPCODE,
        __N
    };

    static inline const char * name {"CI"};

    static inline FieldsType fields {
        Field("func3", 13, 3),
        Field("imm1", 12, 1),
        Field("rs1", 7, 5),
        Field("rd", 7, 5),
        Field("imm5", 2, 5),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func3",  fields[idType::FUNC3]},
        {"imm1",   fields[idType::IMM1]},
        {"rs1",    fields[idType::RS1]},
        {"rd",     fields[idType::RD]},
        {"imm5",   fields[idType::IMM5]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func3",  idType::FUNC3},
        {"imm1",   idType::IMM1},
        {"rs1",    idType::RS1},
        {"rd",     idType::RD},
        {"imm5",   idType::IMM5},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3]
    };

    static inline ImmediateType immediate_type = ImmediateType::UNSIGNED;
};

/**
 * CI_rD_only-Form (for c.li and c.lui)
 */
class Form_CI_rD_only
{
public:
    enum idType : uint32_t
    {
        FUNC3 = 0,
        IMM1,
        RD,
        IMM5,
        OPCODE,
        __N
    };

    static inline const char * name {"CI_rD_only"};

    static inline FieldsType fields {
        Field("func3", 13, 3),
        Field("imm1", 12, 1),
        Field("rd", 7, 5),
        Field("imm5", 2, 5),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func3",  fields[idType::FUNC3]},
        {"imm1",   fields[idType::IMM1]},
        {"rd",     fields[idType::RD]},
        {"imm5",   fields[idType::IMM5]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func3",  idType::FUNC3},
        {"imm1",   idType::IMM1},
        {"rd",     idType::RD},
        {"imm5",   idType::IMM5},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3]
    };

    static inline ImmediateType immediate_type = ImmediateType::SIGNED;
};

/**
 * CIW-Form
 */
class Form_CIW
{
public:
    enum idType : uint32_t
    {
        FUNC3 = 0,
        IMM8,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"CIW"};

    static inline FieldsType fields {
        Field("func3", 13, 3),
        Field("imm8", 5, 8),
        Field("rd", 2, 3),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func3",  fields[idType::FUNC3]},
        {"imm8",   fields[idType::IMM8]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func3",  idType::FUNC3},
        {"imm8",   idType::IMM8},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3]
    };

    static inline ImmediateType immediate_type = ImmediateType::SIGNED;
};

/**
 * CIX-Form: Extension from spec for C.SRLI/C.SRAI/C.ANDI
 */
class Form_CIX
{
public:
    enum idType : uint32_t
    {
        FUNC3 = 0,
        SHAMT1,
        FUNC2,
        RS1,
        RD,
        SHAMT5,
        OPCODE,
        __N
    };

    static inline const char * name {"CIX"};

    static inline FieldsType fields {
        Field("func3", 13, 3),
        Field("shamt1", 12, 1),
        Field("func2", 10, 2),
        Field("rs1", 7, 3),
        Field("rd", 7, 3),
        Field("shamt5", 2, 5),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func3",  fields[idType::FUNC3]},
        {"shamt1", fields[idType::SHAMT1]},
        {"func2",  fields[idType::FUNC2]},
        {"rs1",    fields[idType::RS1]},
        {"rd",     fields[idType::RD]},
        {"shamt5", fields[idType::SHAMT5]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func3",  idType::FUNC3},
        {"shamt1", idType::SHAMT1},
        {"func2",  idType::FUNC2},
        {"rs1",    idType::RS1},
        {"rd",     idType::RD},
        {"shamt5", idType::SHAMT5},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3],
        fields[idType::FUNC2]
    };

    static inline ImmediateType immediate_type = ImmediateType::UNSIGNED;
};

/**
 * CJ-Form
 */
class Form_CJ
{
public:
    enum idType : uint32_t
    {
        FUNC3 = 0,
        IMM11,
        OPCODE,
        __N
    };

    static inline const char * name {"CJ"};

    static inline FieldsType fields {
        Field("func3", 13, 3),
        Field("imm11", 2, 11),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func3",  fields[idType::FUNC3]},
        {"imm11",  fields[idType::IMM11]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func3",  idType::FUNC3},
        {"imm11",  idType::IMM11},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3]
    };

    static inline ImmediateType immediate_type = ImmediateType::SIGNED;
};

/**
 * CJR-Form (for extraction only)
 */
class Form_CJR
{
public:
    enum idType : uint32_t
    {
        FUNC4 = 0,
        RS1,
        RS2,
        OPCODE,
        __N
    };

    static inline const char * name {"CJR"};

    static inline FieldsType fields {
        Field("func4", 12, 4),
        Field("rs1", 7, 5),
        Field("rs2", 2, 5),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func4",  fields[idType::FUNC4]},
        {"rs1",    fields[idType::RS1]},
        {"rs2",    fields[idType::RS2]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func4",  idType::FUNC4},
        {"rs1",    idType::RS1},
        {"rs2",    idType::RS2},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC4]
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};

/**
 * CSR-Form (extraction only)
 */
class Form_CSR
{
public:
    enum idType : uint32_t
    {
        CSR = 0,
        RS1,
        FUNC3,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"CSR"};

    static inline FieldsType fields {
        Field("csr", 20, 12),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"csr",    fields[idType::CSR]},
        {"rs1",    fields[idType::RS1]},
        {"func3",  fields[idType::FUNC3]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"csr",    idType::CSR},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3]
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};


/**
 * CSRI-Form (extraction only)
 */
class Form_CSRI
{
public:
    enum idType : uint32_t
    {
        CSR = 0,
        UIMM,
        FUNC3,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"CSRI"};

    static inline FieldsType fields {
        Field("csr", 20, 12),
        Field("uimm", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"csr",    fields[idType::CSR]},
        {"uimm",   fields[idType::UIMM]},
        {"func3",  fields[idType::FUNC3]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"csr",    idType::CSR},
        {"uimm",   idType::UIMM},
        {"func3",  idType::FUNC3},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3]
    };

    static inline ImmediateType immediate_type = ImmediateType::UNSIGNED;
};


/**
 * FENCE-Form
 */
class Form_FENCE
{
public:
    enum idType : uint32_t
    {
        FM = 0,
        PRED,
        SUCC,
        RS1,
        FUNC3,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"FENCE"};

    static inline FieldsType fields {
        Field("fm", 28, 4),
        Field("pred", 24, 4),
        Field("succ", 20, 4),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"fm",     fields[idType::FM]},
        {"pred",   fields[idType::PRED]},
        {"succ",   fields[idType::SUCC]},
        {"rs1",    fields[idType::RS1]},
        {"func3",  fields[idType::FUNC3]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"fm",     idType::FM},
        {"pred",   idType::PRED},
        {"succ",   idType::SUCC},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3]
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};


/**
 * ISH-Form
 */
class Form_ISH
{
public:
    enum idType : uint32_t
    {
        FUNC6 = 0,
        SHAMT,
        RS1,
        FUNC3,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"ISH"};

    static inline FieldsType fields {
        Field("func6", 26, 6),
        Field("shamt", 20, 6),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func6",  fields[idType::FUNC6]},
        {"shamt",  fields[idType::SHAMT]},
        {"rs1",    fields[idType::RS1]},
        {"func3",  fields[idType::FUNC3]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func6",  idType::FUNC6},
        {"shamt",  idType::SHAMT},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3],
        fields[idType::FUNC6]
    };

    static inline ImmediateType immediate_type = ImmediateType::SIGNED;
};


/**
 * ISHW-Form
 */
class Form_ISHW
{
public:
    enum idType : uint32_t
    {
        FUNC7 = 0,
        SHAMTW,
        RS1,
        FUNC3,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"ISHW"};

    static inline FieldsType fields {
        Field("func7", 25, 7),
        Field("shamtw", 20, 5),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func7",  fields[idType::FUNC7]},
        {"shamtw", fields[idType::SHAMTW]},
        {"rs1",    fields[idType::RS1]},
        {"func3",  fields[idType::FUNC3]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func7",  idType::FUNC7},
        {"shamtw", idType::SHAMTW},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3],
        fields[idType::FUNC7]
    };

    static inline ImmediateType immediate_type = ImmediateType::SIGNED;
};


/**
 * J-Form
 */
class Form_J
{
public:
    enum idType : uint32_t
    {
        IMM20 = 0,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"J"};

    static inline FieldsType fields {
        Field("imm20", 12, 20),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"imm20",  fields[idType::IMM20]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"imm20",  idType::IMM20},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE]
    };

    static inline ImmediateType immediate_type = ImmediateType::SIGNED;
};


/**
 * Floating point R-Form
 */
class Form_Rfloat
{
public:
    enum idType : uint32_t
    {
        FUNC7 = 0,
        RS2,
        RS1,
        RM,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"Rfloat"};

    static inline FieldsType fields {
        Field("func7", 25, 7),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("rm", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func7",  fields[idType::FUNC7]},
        {"rs2",    fields[idType::RS2]},
        {"rs1",    fields[idType::RS1]},
        {"rm",     fields[idType::RM]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func7",  idType::FUNC7},
        {"rs2",    idType::RS2},
        {"rs1",    idType::RS1},
        {"rm",     idType::RM},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC7]
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};

/**
 * R4-Form
 */
class Form_R4
{
public:
    enum idType : uint32_t
    {
        RS3 = 0,
        FUNC2,
        RS2,
        RS1,
        RM,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"R4"};

    static inline FieldsType fields {
        Field("rs3", 27, 5),
        Field("func2", 25, 2),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("rm", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"rs3",    fields[idType::RS3]},
        {"func2",  fields[idType::FUNC2]},
        {"rs2",    fields[idType::RS2]},
        {"rs1",    fields[idType::RS1]},
        {"rm",     fields[idType::RM]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"rs3",    idType::RS3},
        {"func2",  idType::FUNC2},
        {"rs2",    idType::RS2},
        {"rs1",    idType::RS1},
        {"rm",     idType::RM},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC2]
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};

/**
 * S-Form
 */
class Form_S
{
public:
    enum idType : uint32_t
    {
        IMM7 = 0,
        RS2,
        RS1,
        FUNC3,
        IMM5,
        OPCODE,
        __N
    };

    static inline const char * name {"S"};

    static inline FieldsType fields {
        Field("imm7", 25, 7),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("imm5", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"imm7",   fields[idType::IMM7]},
        {"rs2",    fields[idType::RS2]},
        {"rs1",    fields[idType::RS1]},
        {"func3",  fields[idType::FUNC3]},
        {"imm5",   fields[idType::IMM5]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"imm7",   idType::IMM7},
        {"rs2",    idType::RS2},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"imm5",   idType::IMM5},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3]
    };

    static inline ImmediateType immediate_type = ImmediateType::SIGNED;
};

/**
 * U-Form
 */
class Form_U
{
public:
    enum idType : uint32_t
    {
        IMM20 = 0,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"U"};

    static inline FieldsType fields {
        Field("imm20", 12, 20),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"imm20",  fields[idType::IMM20]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"imm20",  idType::IMM20},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE]
    };

    static inline ImmediateType immediate_type = ImmediateType::UNSIGNED;
};

/*
 * Form_V (most vector insts, base opcode 0x57)
 */
class Form_V
{
public:
    enum idType : uint32_t
    {
        FUNC1A = 0,
        FUNC1B,
        FUNC3A,
        FUNC1C,
        VM,
        RS2,
        RS1,
        SIMM5,      // Alias for RS1 in vector-immediate (signed) instructions
        FUNC3B,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"V"};

    static inline FieldsType fields {
        Field("func1a", 31, 1),
        Field("func1b", 30, 1),
        Field("func3a", 27, 3),
        Field("func1c", 26, 1),
        Field("vm", 25, 1),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("simm5", 15, 5),
        Field("func3b", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func1a",  fields[idType::FUNC1A]},
        {"func1b",  fields[idType::FUNC1B]},
        {"func3a",  fields[idType::FUNC3A]},
        {"func1c",  fields[idType::FUNC1C]},
        {"vm",      fields[idType::VM]},
        {"rs2",     fields[idType::RS2]},
        {"rs1",     fields[idType::RS1]},
        {"simm5",   fields[idType::SIMM5]},
        {"func3b",  fields[idType::FUNC3B]},
        {"rd",      fields[idType::RD]},
        {"opcode",  fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func1a",  idType::FUNC1A},
        {"func1b",  idType::FUNC1B},
        {"func3a",  idType::FUNC3A},
        {"func1c",  idType::FUNC1C},
        {"vm",      idType::VM},
        {"rs2",     idType::RS2},
        {"rs1",     idType::RS1},
        {"simm5",   idType::SIMM5},
        {"func3b",  idType::FUNC3B},
        {"rd",      idType::RD},
        {"opcode",  idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3A],
        fields[idType::FUNC1A],
        fields[idType::FUNC1B],
        fields[idType::FUNC1C],
        fields[idType::FUNC3B]
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};


/*
 * Form_VF_mem (float and vector load form)
 */
class Form_VF_mem
{
public:
    enum idType : uint32_t
    {
        NF = 0,
        MEWOP,       // Concatenation of MEW and MOP
        VM,
        RS2,
        RS1,
        WIDTH,
        RD,
        RS3,         // Aliased with RD for stores (store data source register)
        OPCODE,
        __N
    };

    static inline const char * name {"VF_mem"};

    static inline FieldsType fields {
        Field("nf", 29, 3),
        Field("mewop", 26, 3),
        Field("vm", 25, 1),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("width", 12, 3),
        Field("rd", 7, 5),
        Field("rs3", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"nf",      fields[idType::NF]},
        {"mewop",   fields[idType::MEWOP]},
        {"vm",      fields[idType::VM]},
        {"rs2",     fields[idType::RS2]},
        {"rs1",     fields[idType::RS1]},
        {"width",   fields[idType::WIDTH]},
        {"rd",      fields[idType::RD]},
        {"rs3",     fields[idType::RS3]},
        {"opcode",  fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"nf",      idType::NF},
        {"mewop",   idType::MEWOP},
        {"vm",      idType::VM},
        {"rs2",     idType::RS2},
        {"rs1",     idType::RS1},
        {"width",   idType::WIDTH},
        {"rd",      idType::RD},
        {"rs3",     idType::RS3},
        {"opcode",  idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::WIDTH],
        fields[idType::MEWOP]
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};


/*
 * Form_V_vsetvli (for vsetvli instruction -- extraction only)
 */
class Form_V_vsetvli
{
public:
    enum idType : uint32_t
    {
        FUNC1 = 0,
        IMM11,
        RS1,
        FUNC3,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"V_vsetvli"};

    static inline FieldsType fields {
        Field("func1", 31, 1),
        Field("imm11", 20, 11),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func1",   fields[idType::FUNC1]},
        {"imm11",   fields[idType::IMM11]},
        {"rs1",     fields[idType::RS1]},
        {"func3",   fields[idType::FUNC3]},
        {"rd",      fields[idType::RD]},
        {"opcode",  fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func1",   idType::FUNC1},
        {"imm11",   idType::IMM11},
        {"rs1",     idType::RS1},
        {"func3",   idType::FUNC3},
        {"rd",      idType::RD},
        {"opcode",  idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3],
        fields[idType::FUNC1]
    };

    static inline ImmediateType immediate_type = ImmediateType::UNSIGNED;
};


/*
 * Form_V_vsetivli (for vsetivli instruction -- extraction only)
 */
class Form_V_vsetivli
{
public:
    enum idType : uint32_t
    {
        FUNC2 = 0,
        IMM10,
        AVL,
        FUNC3,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"V_vsetivli"};

    static inline FieldsType fields {
        Field("func2", 30, 2),
        Field("imm10", 20, 10),
        Field("avl", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func2",   fields[idType::FUNC2]},
        {"imm10",   fields[idType::IMM10]},
        {"avl",     fields[idType::AVL]},
        {"func3",   fields[idType::FUNC3]},
        {"rd",      fields[idType::RD]},
        {"opcode",  fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func2",   idType::FUNC2},
        {"imm10",   idType::IMM10},
        {"avl",     idType::AVL},
        {"func3",   idType::FUNC3},
        {"rd",      idType::RD},
        {"opcode",  idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3],
        fields[idType::FUNC2]
    };

    static inline ImmediateType immediate_type = ImmediateType::UNSIGNED;
};


/*
 * Form_V_vsetvl (for vsetvl instruction -- extraction only)
 */
class Form_V_vsetvl
{
public:
    enum idType : uint32_t
    {
        FUNC7 = 0,
        RS2,
        RS1,
        FUNC3,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"V_vsetvl"};

    static inline FieldsType fields {
        Field("func7", 25, 7),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func7",   fields[idType::FUNC7]},
        {"rs2",     fields[idType::RS2]},
        {"rs1",     fields[idType::RS1]},
        {"func3",   fields[idType::FUNC3]},
        {"rd",      fields[idType::RD]},
        {"opcode",  fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func7",   idType::FUNC7},
        {"rs2",     idType::RS2},
        {"rs1",     idType::RS1},
        {"func3",   idType::FUNC3},
        {"rd",      idType::RD},
        {"opcode",  idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3],
        fields[idType::FUNC7]
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};


/*
 * Form_V_uimm6 (most vector insts, base opcode 0x57)
 */
class Form_V_uimm6
{
public:
    enum idType : uint32_t
    {
        FUNC5 = 0,
        I5,
        VM,
        RS2,
        UIMM5,      // Alias for RS1 in vector-immediate (signed) instructions
        FUNC3,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"V_uimm6"};

    static inline FieldsType fields {
        Field("func5", 27, 5),
        Field("i5", 26, 1),
        Field("vm", 25, 1),
        Field("rs2", 20, 5),
        Field("uimm5", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func5",   fields[idType::FUNC5]},
        {"i5",      fields[idType::I5]},
        {"vm",      fields[idType::VM]},
        {"rs2",     fields[idType::RS2]},
        {"uimm5",   fields[idType::UIMM5]},
        {"func3",   fields[idType::FUNC3]},
        {"rd",      fields[idType::RD]},
        {"opcode",  fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func5",   idType::FUNC5},
        {"i5",      idType::I5},
        {"vm",      idType::VM},
        {"rs2",     idType::RS2},
        {"uimm5",   idType::UIMM5},
        {"func3",   idType::FUNC3},
        {"rd",      idType::RD},
        {"opcode",  idType::OPCODE}
    };

    static inline FieldsType opcode_fields {
        fields[idType::OPCODE],
        fields[idType::FUNC3],
        fields[idType::FUNC5]
    };

    static inline ImmediateType immediate_type = ImmediateType::NONE;
};
} // namespace mavis
