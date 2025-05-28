#pragma once
// ------------------------------------------------------------------
// Mavis forms for AndeStar V5 extensions
// (c) 2024 Condor Computing Corp
//
// TODO: These are marked specifically AndeStar, in future see if
//       these can be inherited from a 'custom' base class and 
//       specialized for the AS instructions
// ------------------------------------------------------------------
#include "mavis/Form.h"
#include <map>
#include <cinttypes>

namespace mavis {

// ------------------------------------------------------------------
// AndeStar Form Custom 0
//
//   This form has a non-contiguous 18b immediate field
//   named as simply IMM, bit re-ordering is handled in
//   the extractor.
//
//   There is a 2bit opcode extension FUNC2.
//   The opcode is the predefined custom opcode (0x0b)
//
//   e.g. see ADDIGP
// ------------------------------------------------------------------
class Form_AndeStar_Custom_0
{
public:
    enum idType : uint32_t
    {
        IMM18 = 0,
        FUNC2,
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

// ------------------------------------------------------------------
// AndeStar Form Custom 0 LBYTE (EXTRACTION ONLY, xform)
//
//   for LBGP and LBUGP
// ------------------------------------------------------------------
class Form_AndeStar_Custom_0_LBYTE
{
public:
    enum idType : uint32_t
    {
        IMM17 = 0,      // 31
        IMM10_1,        // 30:21
        IMM11,          // 20
        IMM14_12,       // 19:17
        IMM16_15,       // 16:15
        IMM0,           // 14
        RD,             // 11:7
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
};

// ------------------------------------------------------------------
// AndeStar Form Custom 0 SBYTE (EXTRACTION ONLY, xform)
//
//   for SBGP
// ------------------------------------------------------------------
class Form_AndeStar_Custom_0_SBYTE
{
public:
    enum idType : uint32_t
    {
        IMM17 = 0,      // 31
        IMM10_5,        // 30:25
        RS2,            // 24:20
        IMM14_12,       // 19:17
        IMM16_15,       // 16:15
        IMM0,           // 14
        IMM4_1,         // 11:8
        IMM11,          // 7
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
};

// ------------------------------------------------------------------
// AndeStar Form Custom 1 (DECODE ONLY)
// ------------------------------------------------------------------
class Form_AndeStar_Custom_1
{
public:
    enum idType : uint32_t
    {
        FUNC3 = 0,  // 14:12
        OPCODE,     // 6:0
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
};

// ------------------------------------------------------------------
// AndeStar Form Custom 1_LOAD
//
//   This form has a non-contiguous 17b immediate field
//   named as simply IMM, bit re-ordering is handled in
//   the extractor.
//
//   There is a 3bit opcode extension FUNC3.
//   This is the load form, RD is present
//   The opcode is the predefined custom opcode (0x2b)
//
//   e.g. see LWGP
// ------------------------------------------------------------------
class Form_AndeStar_Custom_1_LOAD
{
public:
    enum idType : uint32_t
    {
        IMM17 = 0,
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

// ------------------------------------------------------------------
// AndeStar Form Custom 1 LHALFGP (EXTRACT ONLY, xform)
// ------------------------------------------------------------------
class Form_AndeStar_Custom_1_LHALFGP
{
public:
    enum idType : uint32_t
    {
        IMM17 = 0,  // 31
        IMM10_1,    // 30:21
        IMM11,      // 20
        IMM14_12,   // 19:17
        IMM16_15,   // 16:15
        RD,         // 11:7
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
};

// ------------------------------------------------------------------
// AndeStar Form Custom 1 LWORDGP (EXTRACT ONLY, xform)
// ------------------------------------------------------------------
class Form_AndeStar_Custom_1_LWORDGP
{
public:
    enum idType : uint32_t
    {
        IMM18 = 0,  // 31
        IMM10_2,    // 30:22
        IMM17,      // 21
        IMM11,      // 20
        IMM14_12,   // 19:17
        IMM16_15,   // 16:15
        RD,         // 11:7
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
};

// ------------------------------------------------------------------
// AndeStar Form Custom 1 LDOUBLEGP (EXTRACT ONLY, xform)
// ------------------------------------------------------------------
class Form_AndeStar_Custom_1_LDOUBLEGP
{
public:
    enum idType : uint32_t
    {
        IMM19 = 0,  // 31
        IMM10_3,    // 30:23
        IMM18_17,   // 22:21
        IMM11,      // 20
        IMM14_12,   // 19:17
        IMM16_15,   // 16:15
        RD,         // 11:7
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
};

// ------------------------------------------------------------------
// AndeStar Form Custom 1_STORE
//
//   This form has a non-contiguous 17b immediate field
//   named as simply IMM, bit re-ordering is handled in
//   the extractor.
//
//   There is a 3bit opcode extension FUNC3.
//   This is the store form, RS2 is present
//   The opcode is the predefined custom opcode (0x2b)
//
//   e.g. see SDGP
// ------------------------------------------------------------------
class Form_AndeStar_Custom_1_STORE
{
public:
    enum idType : uint32_t
    {
        IMM7=0,      // 31:25, { imm[19], imm[10:5]  }
        RS2,
        IMM5A,       // 19:15, { imm[14:12], imm[16:15] }
        FUNC3,
        IMM5B,       // 11:7,  { imm[4:3], imm[18:17], imm[11] }
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

// ------------------------------------------------------------------
// AndeStar Form Custom 1 SHALFGP (EXTRACT ONLY, xform)
// ------------------------------------------------------------------
class Form_AndeStar_Custom_1_SHALFGP
{
public:
    enum idType : uint32_t
    {
        IMM17 = 0,  // 31
        IMM10_5,    // 30:21
        RS2,        // 24:20
        IMM14_12,   // 19:17
        IMM16_15,   // 16:15
        IMM4_1,     // 11:8
        IMM11,      // 7
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
};

// ------------------------------------------------------------------
// AndeStar Form Custom 1 SWORDGP (EXTRACT ONLY, xform)
// ------------------------------------------------------------------
class Form_AndeStar_Custom_1_SWORDGP
{
public:
    enum idType : uint32_t
    {
        IMM18 = 0,  // 31
        IMM10_5,    // 30:25
        RS2,        // 24:20
        IMM14_12,   // 19:17
        IMM16_15,   // 16:15
        IMM4_2,     // 11:9
        IMM17,      // 8
        IMM11,      // 7
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
};

// ------------------------------------------------------------------
// AndeStar Form Custom 1 SDOUBLEGP (EXTRACT ONLY, xform)
// ------------------------------------------------------------------
class Form_AndeStar_Custom_1_SDOUBLEGP
{
public:
    enum idType : uint32_t
    {
        IMM19 = 0,  // 31
        IMM10_5,    // 30:25
        RS2,        // 24:20
        IMM14_12,   // 19:17
        IMM16_15,   // 16:15
        IMM4_3,     // 10:11
        IMM18_17,   // 9:8
        IMM11,      // 7
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;
};

// ------------------------------------------------------------------
// AndeStar Form Custom 2 (DECODE ONLY)
//
//    This form is used for decoding and lists only the encoding
//    fields. Use "xform" to extract these AndeStar_Custom_2
//    instructions
// ------------------------------------------------------------------
class Form_AndeStar_Custom_2
{
public:
    enum idType : uint32_t
    {
        FUNC1A = 0,  // 31
        FUNC1B,      // 30
        FUNC5,       // 29:25
        FUNC3,       // 14:12
        OPCODE,      // 6:0
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;

};

// ------------------------------------------------------------------
// AndeStar Form Custom 2 BBx (EXTRACTION ONLY xform)
//
//      For BBC and BBS
// ------------------------------------------------------------------
class Form_AndeStar_Custom_2_BBx
{
public:
    enum idType : uint32_t
    {
        IMM10 = 0,   // 31
        IMM9_5,      // 29:25
        CIMM4_0,     // 24:20
        RS1,         // 19:15
        IMM4_1,      // 11:8
        CIMM5,       // 7
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;

};

// ------------------------------------------------------------------
// AndeStar Form Custom 2 BxxC (EXTRACTION ONLY xform)
//
//      For BEQC and BNEC
// ------------------------------------------------------------------
class Form_AndeStar_Custom_2_BxxC
{
public:
    enum idType : uint32_t
    {
        IMM10 = 0,   // 31
        CIMM6,       // 30
        IMM9_5,      // 29:25
        CIMM4_0,     // 24:20
        RS1,         // 19:15
        IMM4_1,      // 11:8
        CIMM5,       // 7
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;

};

// ------------------------------------------------------------------
// AndeStar Form Custom 2 BFOx (EXTRACTION ONLY xform)
//
//      For BFOS and BFOZ
// ------------------------------------------------------------------
class Form_AndeStar_Custom_2_BFOx
{
public:
    enum idType : uint32_t
    {
        MSB = 0,     // 31:26
        LSB,         // 25:20
        RS1,         // 19:15
        RD,          // 11:7
        __N
    };

    static const char * name;
    static const FieldsType fields;
    static const std::map<std::string, const Field &> fmap;
    static const std::map<std::string, idType> imap;
    static const FieldsType opcode_fields;
    static const ImmediateType immediate_type;

};

// ------------------------------------------------------------------
// AndeStar Form Custom 2 XDEF (EXTRACTION ONLY default xform)
//
//      For LEA* and FIND instructions
// ------------------------------------------------------------------
class Form_AndeStar_Custom_2_XDEF
{
public:
    enum idType : uint32_t
    {
        RS2 = 0,     // 24:20
        RS1,         // 19:15
        RD,          // 11:7
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
