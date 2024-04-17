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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;

    static const FieldsType fields;

    static const std::map<std::string, const Field &> fmap;

    static const std::map<std::string, idType> imap;

    static const FieldsType opcode_fields;

    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
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

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
};
} // namespace mavis
