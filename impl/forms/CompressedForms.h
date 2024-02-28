#pragma once

#include "mavis/Form.h"
#include <map>
#include <cinttypes>

namespace mavis {

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

} // namespace mavis
