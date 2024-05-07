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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
};
} // namespace mavis
