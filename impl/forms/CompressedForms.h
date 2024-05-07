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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
};

} // namespace mavis
