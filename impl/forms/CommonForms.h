#pragma once

#include "mavis/Form.h"
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
 * I-Form
 */
class Form_I
{
public:
    enum idType : uint32_t
    {
        IMM = 0,
        RS1,
        FUNC3,
        RD,
        OPCODE,
        __N
    };

    static inline const char * name {"I"};

    static inline FieldsType fields {
        Field("imm", 20, 12),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"imm",    fields[idType::IMM]},
        {"rs1",    fields[idType::RS1]},
        {"func3",  fields[idType::FUNC3]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"imm",    idType::IMM},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
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
 * R-Form
 */
class Form_R
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

    static inline const char * name {"R"};

    static inline FieldsType fields {
        Field("func7", 25, 7),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap {
        {"func7",  fields[idType::FUNC7]},
        {"rs2",    fields[idType::RS2]},
        {"rs1",    fields[idType::RS1]},
        {"func3",  fields[idType::FUNC3]},
        {"rd",     fields[idType::RD]},
        {"opcode", fields[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap {
        {"func7",  idType::FUNC7},
        {"rs2",    idType::RS2},
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

    static inline ImmediateType immediate_type = ImmediateType::NONE;
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
} // namespace mavis
