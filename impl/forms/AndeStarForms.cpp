#include "impl/forms/AndeStarForms.h"

namespace mavis {

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_0 (see ADDIGP)
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_0::name {"AndeStar_Custom_0"};

const FieldsType Form_AndeStar_Custom_0::fields
{
    Field("imm18", 14, 18),
    Field("func2", 12,  2),
    Field("rd",     7,  5),
    Field("opcode", 0,  7)
};

const std::map<std::string, const Field &> Form_AndeStar_Custom_0::fmap
{
    {"imm18", Form_AndeStar_Custom_0::fields[Form_AndeStar_Custom_0::idType::IMM18]},
    {"func2", Form_AndeStar_Custom_0::fields[Form_AndeStar_Custom_0::idType::FUNC2]},
    {"rd",    Form_AndeStar_Custom_0::fields[Form_AndeStar_Custom_0::idType::RD]},
    {"opcode",Form_AndeStar_Custom_0::fields[Form_AndeStar_Custom_0::idType::OPCODE]}
};

const std::map<std::string,Form_AndeStar_Custom_0::idType>
                                            Form_AndeStar_Custom_0::imap
{
    {"imm18", Form_AndeStar_Custom_0::idType::IMM18},
    {"func2", Form_AndeStar_Custom_0::idType::FUNC2},
    {"rd",    Form_AndeStar_Custom_0::idType::RD},
    {"opcode",Form_AndeStar_Custom_0::idType::OPCODE}
};

const FieldsType Form_AndeStar_Custom_0::opcode_fields
{
    Form_AndeStar_Custom_0::fields[Form_AndeStar_Custom_0::idType::OPCODE],
    Form_AndeStar_Custom_0::fields[Form_AndeStar_Custom_0::idType::FUNC2]
};

const ImmediateType Form_AndeStar_Custom_0::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_0_LBYTE (EXTRACTION ONLY, xform)
//
//      for LBGP and LBUGP
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_0_LBYTE::name {"AndeStar_Custom_0_LBYTE"};

const FieldsType Form_AndeStar_Custom_0_LBYTE::fields
        {
                Field("imm17", 31, 1),
                Field("imm10_1", 21, 10),
                Field("imm11", 20,  1),
                Field("imm14_12", 17,  3),
                Field("imm16_15", 15,  2),
                Field("imm0", 14,  1),
                Field("rd",     7,  5)
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_0_LBYTE::fmap
        {
                {"imm17", Form_AndeStar_Custom_0_LBYTE::fields[Form_AndeStar_Custom_0_LBYTE::idType::IMM17]},
                {"imm10_1", Form_AndeStar_Custom_0_LBYTE::fields[Form_AndeStar_Custom_0_LBYTE::idType::IMM10_1]},
                {"imm11", Form_AndeStar_Custom_0_LBYTE::fields[Form_AndeStar_Custom_0_LBYTE::idType::IMM11]},
                {"imm14_12", Form_AndeStar_Custom_0_LBYTE::fields[Form_AndeStar_Custom_0_LBYTE::idType::IMM14_12]},
                {"imm16_15", Form_AndeStar_Custom_0_LBYTE::fields[Form_AndeStar_Custom_0_LBYTE::idType::IMM16_15]},
                {"imm0", Form_AndeStar_Custom_0_LBYTE::fields[Form_AndeStar_Custom_0_LBYTE::idType::IMM0]},
                {"rd",    Form_AndeStar_Custom_0_LBYTE::fields[Form_AndeStar_Custom_0_LBYTE::idType::RD]},
        };

const std::map<std::string,Form_AndeStar_Custom_0_LBYTE::idType>
        Form_AndeStar_Custom_0_LBYTE::imap
        {
                {"imm17", Form_AndeStar_Custom_0_LBYTE::idType::IMM17},
                {"imm10_1", Form_AndeStar_Custom_0_LBYTE::idType::IMM10_1},
                {"imm11", Form_AndeStar_Custom_0_LBYTE::idType::IMM11},
                {"imm14_12", Form_AndeStar_Custom_0_LBYTE::idType::IMM14_12},
                {"imm16_15", Form_AndeStar_Custom_0_LBYTE::idType::IMM16_15},
                {"imm0", Form_AndeStar_Custom_0_LBYTE::idType::IMM0},
                {"rd",    Form_AndeStar_Custom_0_LBYTE::idType::RD},
        };

const FieldsType Form_AndeStar_Custom_0_LBYTE::opcode_fields
        {};

const ImmediateType Form_AndeStar_Custom_0_LBYTE::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_0_SBYTE (EXTRACTION ONLY, xform)
//
//      for SBGP
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_0_SBYTE::name {"AndeStar_Custom_0_SBYTE"};

const FieldsType Form_AndeStar_Custom_0_SBYTE::fields
        {
                Field("imm17", 31, 1),
                Field("imm10_5", 25, 6),
                Field("rs2", 20,  5),
                Field("imm14_12", 17,  3),
                Field("imm16_15", 15,  2),
                Field("imm0", 14,  1),
                Field("imm4_1",     8,  4),
                Field("imm11",     7,  1),
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_0_SBYTE::fmap
        {
                {"imm17", Form_AndeStar_Custom_0_SBYTE::fields[Form_AndeStar_Custom_0_SBYTE::idType::IMM17]},
                {"imm10_5", Form_AndeStar_Custom_0_SBYTE::fields[Form_AndeStar_Custom_0_SBYTE::idType::IMM10_5]},
                {"rs2", Form_AndeStar_Custom_0_SBYTE::fields[Form_AndeStar_Custom_0_SBYTE::idType::RS2]},
                {"imm14_12", Form_AndeStar_Custom_0_SBYTE::fields[Form_AndeStar_Custom_0_SBYTE::idType::IMM14_12]},
                {"imm16_15", Form_AndeStar_Custom_0_SBYTE::fields[Form_AndeStar_Custom_0_SBYTE::idType::IMM16_15]},
                {"imm0", Form_AndeStar_Custom_0_SBYTE::fields[Form_AndeStar_Custom_0_SBYTE::idType::IMM0]},
                {"imm4_1", Form_AndeStar_Custom_0_SBYTE::fields[Form_AndeStar_Custom_0_SBYTE::idType::IMM4_1]},
                {"imm11", Form_AndeStar_Custom_0_SBYTE::fields[Form_AndeStar_Custom_0_SBYTE::idType::IMM11]},
        };

const std::map<std::string,Form_AndeStar_Custom_0_SBYTE::idType>
        Form_AndeStar_Custom_0_SBYTE::imap
        {
                {"imm17", Form_AndeStar_Custom_0_SBYTE::idType::IMM17},
                {"imm10_5", Form_AndeStar_Custom_0_SBYTE::idType::IMM10_5},
                {"rs2", Form_AndeStar_Custom_0_SBYTE::idType::RS2},
                {"imm14_12", Form_AndeStar_Custom_0_SBYTE::idType::IMM14_12},
                {"imm16_15", Form_AndeStar_Custom_0_SBYTE::idType::IMM16_15},
                {"imm0", Form_AndeStar_Custom_0_SBYTE::idType::IMM0},
                {"imm4_1", Form_AndeStar_Custom_0_SBYTE::idType::IMM4_1},
                {"imm11", Form_AndeStar_Custom_0_SBYTE::idType::IMM11},
        };

const FieldsType Form_AndeStar_Custom_0_SBYTE::opcode_fields
        {};

const ImmediateType Form_AndeStar_Custom_0_SBYTE::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_1 (DECODE ONLY)
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_1::name {"AndeStar_Custom_1"};

const FieldsType Form_AndeStar_Custom_1::fields
        {
                Field("func3",  12,  3),
                Field("opcode",  0,  7)
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_1::fmap
        {
                {"func3", Form_AndeStar_Custom_1::fields[Form_AndeStar_Custom_1::idType::FUNC3]},
                {"opcode",Form_AndeStar_Custom_1::fields[Form_AndeStar_Custom_1::idType::OPCODE]}
        };

const std::map<std::string,Form_AndeStar_Custom_1::idType> Form_AndeStar_Custom_1::imap
        {
                {"func3", Form_AndeStar_Custom_1::idType::FUNC3},
                {"opcode",Form_AndeStar_Custom_1::idType::OPCODE}
        };

const FieldsType Form_AndeStar_Custom_1::opcode_fields
        {
                Form_AndeStar_Custom_1::fields[Form_AndeStar_Custom_1::idType::OPCODE],
                Form_AndeStar_Custom_1::fields[Form_AndeStar_Custom_1::idType::FUNC3]
        };

const ImmediateType Form_AndeStar_Custom_1::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_1_LHALFGP (EXTRACT ONLY, xform)
//
//      For lhgp and lhugp
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_1_LHALFGP::name {"AndeStar_Custom_1_LHALFGP"};

const FieldsType Form_AndeStar_Custom_1_LHALFGP::fields
        {
                Field("imm17",  31, 1),
                Field("imm10_1",  21, 10),
                Field("imm11",  20, 1),
                Field("imm14_12",  17, 3),
                Field("imm16_15",  15, 2),
                Field("rd",  7, 5),
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_1_LHALFGP::fmap
        {
                {"imm17", Form_AndeStar_Custom_1_LHALFGP::fields[Form_AndeStar_Custom_1_LHALFGP::idType::IMM17]},
                {"imm10_1", Form_AndeStar_Custom_1_LHALFGP::fields[Form_AndeStar_Custom_1_LHALFGP::idType::IMM10_1]},
                {"imm11", Form_AndeStar_Custom_1_LHALFGP::fields[Form_AndeStar_Custom_1_LHALFGP::idType::IMM11]},
                {"imm14_12", Form_AndeStar_Custom_1_LHALFGP::fields[Form_AndeStar_Custom_1_LHALFGP::idType::IMM14_12]},
                {"imm16_15", Form_AndeStar_Custom_1_LHALFGP::fields[Form_AndeStar_Custom_1_LHALFGP::idType::IMM16_15]},
                {"rd",    Form_AndeStar_Custom_1_LHALFGP::fields[Form_AndeStar_Custom_1_LHALFGP::idType::RD]},
        };

const std::map<std::string,Form_AndeStar_Custom_1_LHALFGP::idType> Form_AndeStar_Custom_1_LHALFGP::imap
        {
                {"imm17", Form_AndeStar_Custom_1_LHALFGP::idType::IMM17},
                {"imm10_1", Form_AndeStar_Custom_1_LHALFGP::idType::IMM10_1},
                {"imm11", Form_AndeStar_Custom_1_LHALFGP::idType::IMM11},
                {"imm14_12", Form_AndeStar_Custom_1_LHALFGP::idType::IMM14_12},
                {"imm16_15", Form_AndeStar_Custom_1_LHALFGP::idType::IMM16_15},
                {"rd",    Form_AndeStar_Custom_1_LHALFGP::idType::RD},
        };

const FieldsType Form_AndeStar_Custom_1_LHALFGP::opcode_fields
        {};

const ImmediateType Form_AndeStar_Custom_1_LHALFGP::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_1_LWORDGP (EXTRACT ONLY, xform)
//
//      For lwgp and lwugp
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_1_LWORDGP::name {"AndeStar_Custom_1_LWORDGP"};

const FieldsType Form_AndeStar_Custom_1_LWORDGP::fields
        {
                Field("imm18",  31, 1),
                Field("imm10_2",  22, 9),
                Field("imm17",  21, 1),
                Field("imm11",  20, 1),
                Field("imm14_12",  17, 3),
                Field("imm16_15",  15, 2),
                Field("rd",  7, 5),
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_1_LWORDGP::fmap
        {
                {"imm18", Form_AndeStar_Custom_1_LWORDGP::fields[Form_AndeStar_Custom_1_LWORDGP::idType::IMM18]},
                {"imm10_2", Form_AndeStar_Custom_1_LWORDGP::fields[Form_AndeStar_Custom_1_LWORDGP::idType::IMM10_2]},
                {"imm17", Form_AndeStar_Custom_1_LWORDGP::fields[Form_AndeStar_Custom_1_LWORDGP::idType::IMM17]},
                {"imm11", Form_AndeStar_Custom_1_LWORDGP::fields[Form_AndeStar_Custom_1_LWORDGP::idType::IMM11]},
                {"imm14_12", Form_AndeStar_Custom_1_LWORDGP::fields[Form_AndeStar_Custom_1_LWORDGP::idType::IMM14_12]},
                {"imm16_15", Form_AndeStar_Custom_1_LWORDGP::fields[Form_AndeStar_Custom_1_LWORDGP::idType::IMM16_15]},
                {"rd",    Form_AndeStar_Custom_1_LWORDGP::fields[Form_AndeStar_Custom_1_LWORDGP::idType::RD]},
        };

const std::map<std::string,Form_AndeStar_Custom_1_LWORDGP::idType> Form_AndeStar_Custom_1_LWORDGP::imap
        {
                {"imm18", Form_AndeStar_Custom_1_LWORDGP::idType::IMM18},
                {"imm10_2", Form_AndeStar_Custom_1_LWORDGP::idType::IMM10_2},
                {"imm17", Form_AndeStar_Custom_1_LWORDGP::idType::IMM17},
                {"imm11", Form_AndeStar_Custom_1_LWORDGP::idType::IMM11},
                {"imm14_12", Form_AndeStar_Custom_1_LWORDGP::idType::IMM14_12},
                {"imm16_15", Form_AndeStar_Custom_1_LWORDGP::idType::IMM16_15},
                {"rd",    Form_AndeStar_Custom_1_LWORDGP::idType::RD},
        };

const FieldsType Form_AndeStar_Custom_1_LWORDGP::opcode_fields
        {};

const ImmediateType Form_AndeStar_Custom_1_LWORDGP::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_1_LDOUBLEGP (EXTRACT ONLY, xform)
//
//      For ldgp
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_1_LDOUBLEGP::name {"AndeStar_Custom_1_LDOUBLEGP"};

const FieldsType Form_AndeStar_Custom_1_LDOUBLEGP::fields
        {
                Field("imm19",  31, 1),
                Field("imm10_3",  23, 8),
                Field("imm18_17",  21, 2),
                Field("imm11",  20, 1),
                Field("imm14_12",  17, 3),
                Field("imm16_15",  15, 2),
                Field("rd",  7, 5),
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_1_LDOUBLEGP::fmap
        {
                {"imm19", Form_AndeStar_Custom_1_LDOUBLEGP::fields[Form_AndeStar_Custom_1_LDOUBLEGP::idType::IMM19]},
                {"imm10_3", Form_AndeStar_Custom_1_LDOUBLEGP::fields[Form_AndeStar_Custom_1_LDOUBLEGP::idType::IMM10_3]},
                {"imm18_17", Form_AndeStar_Custom_1_LDOUBLEGP::fields[Form_AndeStar_Custom_1_LDOUBLEGP::idType::IMM18_17]},
                {"imm11", Form_AndeStar_Custom_1_LDOUBLEGP::fields[Form_AndeStar_Custom_1_LDOUBLEGP::idType::IMM11]},
                {"imm14_12", Form_AndeStar_Custom_1_LDOUBLEGP::fields[Form_AndeStar_Custom_1_LDOUBLEGP::idType::IMM14_12]},
                {"imm16_15", Form_AndeStar_Custom_1_LDOUBLEGP::fields[Form_AndeStar_Custom_1_LDOUBLEGP::idType::IMM16_15]},
                {"rd",    Form_AndeStar_Custom_1_LDOUBLEGP::fields[Form_AndeStar_Custom_1_LDOUBLEGP::idType::RD]},
        };

const std::map<std::string,Form_AndeStar_Custom_1_LDOUBLEGP::idType> Form_AndeStar_Custom_1_LDOUBLEGP::imap
        {
                {"imm19", Form_AndeStar_Custom_1_LDOUBLEGP::idType::IMM19},
                {"imm10_3", Form_AndeStar_Custom_1_LDOUBLEGP::idType::IMM10_3},
                {"imm18_17", Form_AndeStar_Custom_1_LDOUBLEGP::idType::IMM18_17},
                {"imm11", Form_AndeStar_Custom_1_LDOUBLEGP::idType::IMM11},
                {"imm14_12", Form_AndeStar_Custom_1_LDOUBLEGP::idType::IMM14_12},
                {"imm16_15", Form_AndeStar_Custom_1_LDOUBLEGP::idType::IMM16_15},
                {"rd",    Form_AndeStar_Custom_1_LDOUBLEGP::idType::RD},
        };

const FieldsType Form_AndeStar_Custom_1_LDOUBLEGP::opcode_fields
        {};

const ImmediateType Form_AndeStar_Custom_1_LDOUBLEGP::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_1_LOAD (see LWGP)
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_1_LOAD::name {"AndeStar_Custom_1_LOAD"};

const FieldsType Form_AndeStar_Custom_1_LOAD::fields
{
    Field("imm17",  15, 17),
    Field("func3",  12,  3),
    Field("rd",      7,  5),
    Field("opcode",  0,  7)
};

const std::map<std::string, const Field &> Form_AndeStar_Custom_1_LOAD::fmap
{
    {"imm17", Form_AndeStar_Custom_1_LOAD::fields[Form_AndeStar_Custom_1_LOAD::idType::IMM17]},
    {"func3", Form_AndeStar_Custom_1_LOAD::fields[Form_AndeStar_Custom_1_LOAD::idType::FUNC3]},
    {"rd",    Form_AndeStar_Custom_1_LOAD::fields[Form_AndeStar_Custom_1_LOAD::idType::RD]},
    {"opcode",Form_AndeStar_Custom_1_LOAD::fields[Form_AndeStar_Custom_1_LOAD::idType::OPCODE]}
};

const std::map<std::string,Form_AndeStar_Custom_1_LOAD::idType> Form_AndeStar_Custom_1_LOAD::imap
{
    {"imm17", Form_AndeStar_Custom_1_LOAD::idType::IMM17},
    {"func3", Form_AndeStar_Custom_1_LOAD::idType::FUNC3},
    {"rd",    Form_AndeStar_Custom_1_LOAD::idType::RD},
    {"opcode",Form_AndeStar_Custom_1_LOAD::idType::OPCODE}
};

const FieldsType Form_AndeStar_Custom_1_LOAD::opcode_fields
{
    Form_AndeStar_Custom_1_LOAD::fields[Form_AndeStar_Custom_1_LOAD::idType::OPCODE],
    Form_AndeStar_Custom_1_LOAD::fields[Form_AndeStar_Custom_1_LOAD::idType::FUNC3]
};

const ImmediateType Form_AndeStar_Custom_1_LOAD::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_1_STORE (see SDGP)
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_1_STORE::name {"AndeStar_Custom_1_STORE"};

const FieldsType Form_AndeStar_Custom_1_STORE::fields
{
    Field("imm7",   25,  7),  // 31:25, { imm[19],    imm[10:5]           }
    Field("rs2",    20,  5),
    Field("imm5a",  15,  5),  // 19:15, { imm[14:12], imm[16:15]          }
    Field("func3",  12,  3),
    Field("imm5b",  15,  5),  // 11:7,  { imm[4:3],   imm[18:17], imm[11] }
    Field("opcode",  0,  7)
};

const std::map<std::string, const Field &> Form_AndeStar_Custom_1_STORE::fmap
{
    {"imm7",  Form_AndeStar_Custom_1_STORE::fields[Form_AndeStar_Custom_1_STORE::idType::IMM7]},
    {"rs2",   Form_AndeStar_Custom_1_STORE::fields[Form_AndeStar_Custom_1_STORE::idType::RS2]},
    {"imm5a", Form_AndeStar_Custom_1_STORE::fields[Form_AndeStar_Custom_1_STORE::idType::IMM5A]},
    {"func3", Form_AndeStar_Custom_1_STORE::fields[Form_AndeStar_Custom_1_STORE::idType::FUNC3]},
    {"imm5b", Form_AndeStar_Custom_1_STORE::fields[Form_AndeStar_Custom_1_STORE::idType::IMM5B]},
    {"opcode",Form_AndeStar_Custom_1_STORE::fields[Form_AndeStar_Custom_1_STORE::idType::OPCODE]}
};

const std::map<std::string,Form_AndeStar_Custom_1_STORE::idType> 
                                            Form_AndeStar_Custom_1_STORE::imap
{
    {"imm7",  Form_AndeStar_Custom_1_STORE::idType::IMM7},
    {"rs2",   Form_AndeStar_Custom_1_STORE::idType::RS2},
    {"imm5a", Form_AndeStar_Custom_1_STORE::idType::IMM5A},
    {"func3", Form_AndeStar_Custom_1_STORE::idType::FUNC3},
    {"imm5b", Form_AndeStar_Custom_1_STORE::idType::IMM5B},
    {"opcode",Form_AndeStar_Custom_1_STORE::idType::OPCODE}
};

const FieldsType Form_AndeStar_Custom_1_STORE::opcode_fields
{
    Form_AndeStar_Custom_1_STORE::fields[Form_AndeStar_Custom_1_STORE::idType::OPCODE],
    Form_AndeStar_Custom_1_STORE::fields[Form_AndeStar_Custom_1_STORE::idType::FUNC3]
};

const ImmediateType Form_AndeStar_Custom_1_STORE::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_1_SHALFGP (EXTRACT ONLY, xform)
//
//      For shgp
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_1_SHALFGP::name {"AndeStar_Custom_1_SHALFGP"};

const FieldsType Form_AndeStar_Custom_1_SHALFGP::fields
        {
                Field("imm17",  31, 1),
                Field("imm10_5",  25, 6),
                Field("rs2",  20, 5),
                Field("imm14_12",  17, 3),
                Field("imm16_15",  15, 2),
                Field("imm4_1",  8, 4),
                Field("imm11",  7, 1),
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_1_SHALFGP::fmap
        {
                {"imm17", Form_AndeStar_Custom_1_SHALFGP::fields[Form_AndeStar_Custom_1_SHALFGP::idType::IMM17]},
                {"imm10_5", Form_AndeStar_Custom_1_SHALFGP::fields[Form_AndeStar_Custom_1_SHALFGP::idType::IMM10_5]},
                {"rs2",    Form_AndeStar_Custom_1_SHALFGP::fields[Form_AndeStar_Custom_1_SHALFGP::idType::RS2]},
                {"imm14_12", Form_AndeStar_Custom_1_SHALFGP::fields[Form_AndeStar_Custom_1_SHALFGP::idType::IMM14_12]},
                {"imm16_15", Form_AndeStar_Custom_1_SHALFGP::fields[Form_AndeStar_Custom_1_SHALFGP::idType::IMM16_15]},
                {"imm4_1", Form_AndeStar_Custom_1_SHALFGP::fields[Form_AndeStar_Custom_1_SHALFGP::idType::IMM4_1]},
                {"imm11", Form_AndeStar_Custom_1_SHALFGP::fields[Form_AndeStar_Custom_1_SHALFGP::idType::IMM11]},
        };

const std::map<std::string,Form_AndeStar_Custom_1_SHALFGP::idType> Form_AndeStar_Custom_1_SHALFGP::imap
        {
                {"imm17", Form_AndeStar_Custom_1_SHALFGP::idType::IMM17},
                {"imm10_5", Form_AndeStar_Custom_1_SHALFGP::idType::IMM10_5},
                {"rs2",    Form_AndeStar_Custom_1_SHALFGP::idType::RS2},
                {"imm14_12", Form_AndeStar_Custom_1_SHALFGP::idType::IMM14_12},
                {"imm16_15", Form_AndeStar_Custom_1_SHALFGP::idType::IMM16_15},
                {"imm4_1", Form_AndeStar_Custom_1_SHALFGP::idType::IMM4_1},
                {"imm11", Form_AndeStar_Custom_1_SHALFGP::idType::IMM11},
        };

const FieldsType Form_AndeStar_Custom_1_SHALFGP::opcode_fields
        {};

const ImmediateType Form_AndeStar_Custom_1_SHALFGP::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_1_SWORDGP (EXTRACT ONLY, xform)
//
//      For swgp
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_1_SWORDGP::name {"AndeStar_Custom_1_SWORDGP"};

const FieldsType Form_AndeStar_Custom_1_SWORDGP::fields
        {
                Field("imm18",  31, 1),
                Field("imm10_5",  25, 6),
                Field("rs2",  20, 5),
                Field("imm14_12",  17, 3),
                Field("imm16_15",  15, 2),
                Field("imm4_2",  9, 3),
                Field("imm17",  8, 1),
                Field("imm11",  7, 1),
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_1_SWORDGP::fmap
        {
                {"imm18", Form_AndeStar_Custom_1_SWORDGP::fields[Form_AndeStar_Custom_1_SWORDGP::idType::IMM18]},
                {"imm10_5", Form_AndeStar_Custom_1_SWORDGP::fields[Form_AndeStar_Custom_1_SWORDGP::idType::IMM10_5]},
                {"rs2",    Form_AndeStar_Custom_1_SWORDGP::fields[Form_AndeStar_Custom_1_SWORDGP::idType::RS2]},
                {"imm14_12", Form_AndeStar_Custom_1_SWORDGP::fields[Form_AndeStar_Custom_1_SWORDGP::idType::IMM14_12]},
                {"imm16_15", Form_AndeStar_Custom_1_SWORDGP::fields[Form_AndeStar_Custom_1_SWORDGP::idType::IMM16_15]},
                {"imm4_2", Form_AndeStar_Custom_1_SWORDGP::fields[Form_AndeStar_Custom_1_SWORDGP::idType::IMM4_2]},
                {"imm17", Form_AndeStar_Custom_1_SWORDGP::fields[Form_AndeStar_Custom_1_SWORDGP::idType::IMM17]},
                {"imm11", Form_AndeStar_Custom_1_SWORDGP::fields[Form_AndeStar_Custom_1_SWORDGP::idType::IMM11]},
        };

const std::map<std::string,Form_AndeStar_Custom_1_SWORDGP::idType> Form_AndeStar_Custom_1_SWORDGP::imap
        {
                {"imm18", Form_AndeStar_Custom_1_SWORDGP::idType::IMM18},
                {"imm10_5", Form_AndeStar_Custom_1_SWORDGP::idType::IMM10_5},
                {"rs2",    Form_AndeStar_Custom_1_SWORDGP::idType::RS2},
                {"imm14_12", Form_AndeStar_Custom_1_SWORDGP::idType::IMM14_12},
                {"imm16_15", Form_AndeStar_Custom_1_SWORDGP::idType::IMM16_15},
                {"imm4_2", Form_AndeStar_Custom_1_SWORDGP::idType::IMM4_2},
                {"imm17", Form_AndeStar_Custom_1_SWORDGP::idType::IMM17},
                {"imm11", Form_AndeStar_Custom_1_SWORDGP::idType::IMM11},
        };

const FieldsType Form_AndeStar_Custom_1_SWORDGP::opcode_fields
        {};

const ImmediateType Form_AndeStar_Custom_1_SWORDGP::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_1_SDOUBLEGP (EXTRACT ONLY, xform)
//
//      For sdgp
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_1_SDOUBLEGP::name {"AndeStar_Custom_1_SDOUBLEGP"};

const FieldsType Form_AndeStar_Custom_1_SDOUBLEGP::fields
        {
                Field("imm19",  31, 1),
                Field("imm10_5",  25, 6),
                Field("rs2",  20, 5),
                Field("imm14_12",  17, 3),
                Field("imm16_15",  15, 2),
                Field("imm4_3",  10, 2),
                Field("imm18_17",  8, 2),
                Field("imm11",  7, 1),
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_1_SDOUBLEGP::fmap
        {
                {"imm19", Form_AndeStar_Custom_1_SDOUBLEGP::fields[Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM19]},
                {"imm10_5", Form_AndeStar_Custom_1_SDOUBLEGP::fields[Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM10_5]},
                {"rs2",    Form_AndeStar_Custom_1_SDOUBLEGP::fields[Form_AndeStar_Custom_1_SDOUBLEGP::idType::RS2]},
                {"imm14_12", Form_AndeStar_Custom_1_SDOUBLEGP::fields[Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM14_12]},
                {"imm16_15", Form_AndeStar_Custom_1_SDOUBLEGP::fields[Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM16_15]},
                {"imm4_3", Form_AndeStar_Custom_1_SDOUBLEGP::fields[Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM4_3]},
                {"imm18_17", Form_AndeStar_Custom_1_SDOUBLEGP::fields[Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM18_17]},
                {"imm11", Form_AndeStar_Custom_1_SDOUBLEGP::fields[Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM11]},
        };

const std::map<std::string,Form_AndeStar_Custom_1_SDOUBLEGP::idType> Form_AndeStar_Custom_1_SDOUBLEGP::imap
        {
                {"imm19", Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM19},
                {"imm10_5", Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM10_5},
                {"rs2",    Form_AndeStar_Custom_1_SDOUBLEGP::idType::RS2},
                {"imm14_12", Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM14_12},
                {"imm16_15", Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM16_15},
                {"imm4_3", Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM4_3},
                {"imm18_17", Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM18_17},
                {"imm11", Form_AndeStar_Custom_1_SDOUBLEGP::idType::IMM11},
        };

const FieldsType Form_AndeStar_Custom_1_SDOUBLEGP::opcode_fields
        {};

const ImmediateType Form_AndeStar_Custom_1_SDOUBLEGP::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_2 (DECODE ONLY FORM)
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_2::name {"AndeStar_Custom_2"};

const FieldsType Form_AndeStar_Custom_2::fields
        {
                Field("func1a", 31,  1),
                Field("func1b", 30,  1),
                Field("func5",  25,  5),
                Field("func3",  12,  3),
                Field("opcode",  0,  7)
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_2::fmap
        {
                {"func1a",  Form_AndeStar_Custom_2::fields[Form_AndeStar_Custom_2::idType::FUNC1A]},
                {"func1b",   Form_AndeStar_Custom_2::fields[Form_AndeStar_Custom_2::idType::FUNC1B]},
                {"func5", Form_AndeStar_Custom_2::fields[Form_AndeStar_Custom_2::idType::FUNC5]},
                {"func3", Form_AndeStar_Custom_2::fields[Form_AndeStar_Custom_2::idType::FUNC3]},
                {"opcode",Form_AndeStar_Custom_2::fields[Form_AndeStar_Custom_2::idType::OPCODE]}
        };

const std::map<std::string,Form_AndeStar_Custom_2::idType>
        Form_AndeStar_Custom_2::imap
        {
                {"func1a",  Form_AndeStar_Custom_2::idType::FUNC1A},
                {"func1b",   Form_AndeStar_Custom_2::idType::FUNC1B},
                {"func5", Form_AndeStar_Custom_2::idType::FUNC5},
                {"func3", Form_AndeStar_Custom_2::idType::FUNC3},
                {"opcode",Form_AndeStar_Custom_2::idType::OPCODE}
        };

const FieldsType Form_AndeStar_Custom_2::opcode_fields
        {
                Form_AndeStar_Custom_2::fields[Form_AndeStar_Custom_2::idType::OPCODE],
                Form_AndeStar_Custom_2::fields[Form_AndeStar_Custom_2::idType::FUNC3],
                Form_AndeStar_Custom_2::fields[Form_AndeStar_Custom_2::idType::FUNC5],
                Form_AndeStar_Custom_2::fields[Form_AndeStar_Custom_2::idType::FUNC1B],
                Form_AndeStar_Custom_2::fields[Form_AndeStar_Custom_2::idType::FUNC1A]
        };

const ImmediateType Form_AndeStar_Custom_2::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_2_BBx (For EXTRACTION ONLY, "xform")
//
//      For BBC and BBS
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_2_BBx::name {"AndeStar_Custom_2_BBx"};

const FieldsType Form_AndeStar_Custom_2_BBx::fields
        {
                Field("imm10", 31,  1),
                Field("imm9_5", 25,  5),
                Field("cimm4_0",  20,  5),
                Field("rs1",  15,  5),
                Field("imm4_1",  8,  4),
                Field("cimm5",  7,  1),
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_2_BBx::fmap
        {
                {"imm10",  Form_AndeStar_Custom_2_BBx::fields[Form_AndeStar_Custom_2_BBx::idType::IMM10]},
                {"imm9_5",   Form_AndeStar_Custom_2_BBx::fields[Form_AndeStar_Custom_2_BBx::idType::IMM9_5]},
                {"cimm4_0", Form_AndeStar_Custom_2_BBx::fields[Form_AndeStar_Custom_2_BBx::idType::CIMM4_0]},
                {"rs1", Form_AndeStar_Custom_2_BBx::fields[Form_AndeStar_Custom_2_BBx::idType::RS1]},
                {"imm4_1",Form_AndeStar_Custom_2_BBx::fields[Form_AndeStar_Custom_2_BBx::idType::IMM4_1]},
                {"cimm5",Form_AndeStar_Custom_2_BBx::fields[Form_AndeStar_Custom_2_BBx::idType::CIMM5]},
        };

const std::map<std::string,Form_AndeStar_Custom_2_BBx::idType>
        Form_AndeStar_Custom_2_BBx::imap
        {
                {"imm10",  Form_AndeStar_Custom_2_BBx::idType::IMM10},
                {"imm9_5",   Form_AndeStar_Custom_2_BBx::idType::IMM9_5},
                {"cimm4_0", Form_AndeStar_Custom_2_BBx::idType::CIMM4_0},
                {"rs1", Form_AndeStar_Custom_2_BBx::idType::RS1},
                {"imm4_1",Form_AndeStar_Custom_2_BBx::idType::IMM4_1},
                {"cimm5",Form_AndeStar_Custom_2_BBx::idType::CIMM5}
        };

const FieldsType Form_AndeStar_Custom_2_BBx::opcode_fields
        {};

const ImmediateType Form_AndeStar_Custom_2_BBx::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_2_BxxC (For EXTRACTION ONLY, "xform")
//
//      For BEQC and BNEC
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_2_BxxC::name {"AndeStar_Custom_2_BxxC"};

const FieldsType Form_AndeStar_Custom_2_BxxC::fields
        {
                Field("imm10", 31,  1),
                Field("cimm6", 30,  1),
                Field("imm9_5", 25,  5),
                Field("cimm4_0",  20,  5),
                Field("rs1",  15,  5),
                Field("imm4_1",  8,  4),
                Field("cimm5",  7,  1),
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_2_BxxC::fmap
        {
                {"imm10",  Form_AndeStar_Custom_2_BxxC::fields[Form_AndeStar_Custom_2_BxxC::idType::IMM10]},
                {"cimm6",  Form_AndeStar_Custom_2_BxxC::fields[Form_AndeStar_Custom_2_BxxC::idType::CIMM6]},
                {"imm9_5",   Form_AndeStar_Custom_2_BxxC::fields[Form_AndeStar_Custom_2_BxxC::idType::IMM9_5]},
                {"cimm4_0", Form_AndeStar_Custom_2_BxxC::fields[Form_AndeStar_Custom_2_BxxC::idType::CIMM4_0]},
                {"rs1", Form_AndeStar_Custom_2_BxxC::fields[Form_AndeStar_Custom_2_BxxC::idType::RS1]},
                {"imm4_1",Form_AndeStar_Custom_2_BxxC::fields[Form_AndeStar_Custom_2_BxxC::idType::IMM4_1]},
                {"cimm5",Form_AndeStar_Custom_2_BxxC::fields[Form_AndeStar_Custom_2_BxxC::idType::CIMM5]},
        };

const std::map<std::string,Form_AndeStar_Custom_2_BxxC::idType>
        Form_AndeStar_Custom_2_BxxC::imap
        {
                {"imm10",  Form_AndeStar_Custom_2_BxxC::idType::IMM10},
                {"cimm6",   Form_AndeStar_Custom_2_BxxC::idType::CIMM6},
                {"imm9_5",   Form_AndeStar_Custom_2_BxxC::idType::IMM9_5},
                {"cimm4_0", Form_AndeStar_Custom_2_BxxC::idType::CIMM4_0},
                {"rs1", Form_AndeStar_Custom_2_BxxC::idType::RS1},
                {"imm4_1",Form_AndeStar_Custom_2_BxxC::idType::IMM4_1},
                {"cimm5",Form_AndeStar_Custom_2_BxxC::idType::CIMM5}
        };

const FieldsType Form_AndeStar_Custom_2_BxxC::opcode_fields
        {};

const ImmediateType Form_AndeStar_Custom_2_BxxC::immediate_type = ImmediateType::SIGNED;

// ----------------------------------------------------------------------
// Form_AndeStar_Custom_2_BFOx (For EXTRACTION ONLY, "xform")
//
//      For BFOS and BFOZ
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_2_BFOx::name {"AndeStar_Custom_2_BFOx"};

const FieldsType Form_AndeStar_Custom_2_BFOx::fields
        {
                Field("msb", 26,  6),
                Field("lsb", 20,  6),
                Field("rs1",  15,  5),
                Field("rd",  7,  5),
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_2_BFOx::fmap
        {
                {"msb",  Form_AndeStar_Custom_2_BFOx::fields[Form_AndeStar_Custom_2_BFOx::idType::MSB]},
                {"lsb",  Form_AndeStar_Custom_2_BFOx::fields[Form_AndeStar_Custom_2_BFOx::idType::LSB]},
                {"rs1", Form_AndeStar_Custom_2_BFOx::fields[Form_AndeStar_Custom_2_BFOx::idType::RS1]},
                {"rd", Form_AndeStar_Custom_2_BFOx::fields[Form_AndeStar_Custom_2_BFOx::idType::RD]},
        };

const std::map<std::string,Form_AndeStar_Custom_2_BFOx::idType>
        Form_AndeStar_Custom_2_BFOx::imap
        {
                {"msb",  Form_AndeStar_Custom_2_BFOx::idType::MSB},
                {"lsb",   Form_AndeStar_Custom_2_BFOx::idType::LSB},
                {"rs1", Form_AndeStar_Custom_2_BFOx::idType::RS1},
                {"rd", Form_AndeStar_Custom_2_BFOx::idType::RD},
        };

const FieldsType Form_AndeStar_Custom_2_BFOx::opcode_fields
        {};

const ImmediateType Form_AndeStar_Custom_2_BFOx::immediate_type = ImmediateType::NONE;


// ----------------------------------------------------------------------
// Form_AndeStar_Custom_2_XDEF (For EXTRACTION ONLY, default "xform")
//
//      For LEA* and FIND instructions
// ----------------------------------------------------------------------
const char * Form_AndeStar_Custom_2_XDEF::name {"AndeStar_Custom_2_XDEF"};

const FieldsType Form_AndeStar_Custom_2_XDEF::fields
        {
                Field("rs2", 20,  5),
                Field("rs1",  15,  5),
                Field("rd",  7,  5),
        };

const std::map<std::string, const Field &> Form_AndeStar_Custom_2_XDEF::fmap
        {
                {"rs2", Form_AndeStar_Custom_2_XDEF::fields[Form_AndeStar_Custom_2_XDEF::idType::RS2]},
                {"rs1", Form_AndeStar_Custom_2_XDEF::fields[Form_AndeStar_Custom_2_XDEF::idType::RS1]},
                {"rd",  Form_AndeStar_Custom_2_XDEF::fields[Form_AndeStar_Custom_2_XDEF::idType::RD]},
        };

const std::map<std::string,Form_AndeStar_Custom_2_XDEF::idType>
        Form_AndeStar_Custom_2_XDEF::imap
        {
                {"rs2", Form_AndeStar_Custom_2_XDEF::idType::RS2},
                {"rs1", Form_AndeStar_Custom_2_XDEF::idType::RS1},
                {"rd",  Form_AndeStar_Custom_2_XDEF::idType::RD},
        };

const FieldsType Form_AndeStar_Custom_2_XDEF::opcode_fields
        {};

const ImmediateType Form_AndeStar_Custom_2_XDEF::immediate_type = ImmediateType::NONE;

} // namespace mavis
