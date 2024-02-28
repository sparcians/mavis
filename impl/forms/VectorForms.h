#pragma once

#include "mavis/Form.h"
#include <map>
#include <cinttypes>

namespace mavis {

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
