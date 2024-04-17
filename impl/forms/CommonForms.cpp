#include "impl/forms/CommonForms.h"

namespace mavis {

/**
 * AMO-Form
 */
const char * Form_AMO::name {"AMO"};

const FieldsType Form_AMO::fields {
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

const std::map<std::string, const Field &> Form_AMO::fmap {
    {"func5",  Form_AMO::fields[Form_AMO::idType::FUNC5]},
    {"aq",     Form_AMO::fields[Form_AMO::idType::AQ]},
    {"wd",     Form_AMO::fields[Form_AMO::idType::WD]},
    {"rl",     Form_AMO::fields[Form_AMO::idType::RL]},
    {"vm",     Form_AMO::fields[Form_AMO::idType::VM]},
    {"rs2",    Form_AMO::fields[Form_AMO::idType::RS2]},
    {"rs1",    Form_AMO::fields[Form_AMO::idType::RS1]},
    {"func3",  Form_AMO::fields[Form_AMO::idType::FUNC3]},
    {"width",  Form_AMO::fields[Form_AMO::idType::WIDTH]},
    {"rd",     Form_AMO::fields[Form_AMO::idType::RD]},
    {"opcode", Form_AMO::fields[Form_AMO::idType::OPCODE]}
};

const std::map<std::string, Form_AMO::idType> Form_AMO::imap {
    {"func5",  Form_AMO::idType::FUNC5},
    {"aq",     Form_AMO::idType::AQ},
    {"wd",     Form_AMO::idType::WD},
    {"rl",     Form_AMO::idType::RL},
    {"vm",     Form_AMO::idType::VM},
    {"rs2",    Form_AMO::idType::RS2},
    {"rs1",    Form_AMO::idType::RS1},
    {"func3",  Form_AMO::idType::FUNC3},
    {"width",  Form_AMO::idType::WIDTH},
    {"rd",     Form_AMO::idType::RD},
    {"opcode", Form_AMO::idType::OPCODE}
};

const FieldsType Form_AMO::opcode_fields {
    Form_AMO::fields[Form_AMO::idType::OPCODE],
    Form_AMO::fields[Form_AMO::idType::FUNC3],
    Form_AMO::fields[Form_AMO::idType::FUNC5]
};

const ImmediateType Form_AMO::immediate_type = ImmediateType::NONE;

/**
 * B-Form
 */
const char * Form_B::name {"B"};

const FieldsType Form_B::fields {
    Field("imm7", 25, 7),
    Field("rs2", 20, 5),
    Field("rs1", 15, 5),
    Field("func3", 12, 3),
    Field("imm5", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_B::fmap {
    {"imm7",   Form_B::fields[Form_B::idType::IMM7]},
    {"rs2",    Form_B::fields[Form_B::idType::RS2]},
    {"rs1",    Form_B::fields[Form_B::idType::RS1]},
    {"func3",  Form_B::fields[Form_B::idType::FUNC3]},
    {"imm5",   Form_B::fields[Form_B::idType::IMM5]},
    {"opcode", Form_B::fields[Form_B::idType::OPCODE]}
};

const std::map<std::string, Form_B::idType> Form_B::imap {
    {"imm7",   Form_B::idType::IMM7},
    {"rs2",    Form_B::idType::RS2},
    {"rs1",    Form_B::idType::RS1},
    {"func3",  Form_B::idType::FUNC3},
    {"imm5",   Form_B::idType::IMM5},
    {"opcode", Form_B::idType::OPCODE}
};

const FieldsType Form_B::opcode_fields {
    Form_B::fields[Form_B::idType::OPCODE],
    Form_B::fields[Form_B::idType::FUNC3]
};

const ImmediateType Form_B::immediate_type = ImmediateType::SIGNED;

/**
 * CSR-Form (extraction only)
 */
const char * Form_CSR::name {"CSR"};

const FieldsType Form_CSR::fields {
    Field("csr", 20, 12),
    Field("rs1", 15, 5),
    Field("func3", 12, 3),
    Field("rd", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_CSR::fmap {
    {"csr",    Form_CSR::fields[Form_CSR::idType::CSR]},
    {"rs1",    Form_CSR::fields[Form_CSR::idType::RS1]},
    {"func3",  Form_CSR::fields[Form_CSR::idType::FUNC3]},
    {"rd",     Form_CSR::fields[Form_CSR::idType::RD]},
    {"opcode", Form_CSR::fields[Form_CSR::idType::OPCODE]}
};

const std::map<std::string, Form_CSR::idType> Form_CSR::imap {
    {"csr",    Form_CSR::idType::CSR},
    {"rs1",    Form_CSR::idType::RS1},
    {"func3",  Form_CSR::idType::FUNC3},
    {"rd",     Form_CSR::idType::RD},
    {"opcode", Form_CSR::idType::OPCODE}
};

const FieldsType Form_CSR::opcode_fields {
    Form_CSR::fields[Form_CSR::idType::OPCODE],
    Form_CSR::fields[Form_CSR::idType::FUNC3]
};

const ImmediateType Form_CSR::immediate_type = ImmediateType::NONE;

/**
 * CSRI-Form (extraction only)
 */
const char * Form_CSRI::name {"CSRI"};

const FieldsType Form_CSRI::fields {
    Field("csr", 20, 12),
    Field("uimm", 15, 5),
    Field("func3", 12, 3),
    Field("rd", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_CSRI::fmap {
    {"csr",    Form_CSRI::fields[Form_CSRI::idType::CSR]},
    {"uimm",   Form_CSRI::fields[Form_CSRI::idType::UIMM]},
    {"func3",  Form_CSRI::fields[Form_CSRI::idType::FUNC3]},
    {"rd",     Form_CSRI::fields[Form_CSRI::idType::RD]},
    {"opcode", Form_CSRI::fields[Form_CSRI::idType::OPCODE]}
};

const std::map<std::string, Form_CSRI::idType> Form_CSRI::imap {
    {"csr",    Form_CSRI::idType::CSR},
    {"uimm",   Form_CSRI::idType::UIMM},
    {"func3",  Form_CSRI::idType::FUNC3},
    {"rd",     Form_CSRI::idType::RD},
    {"opcode", Form_CSRI::idType::OPCODE}
};

const FieldsType Form_CSRI::opcode_fields {
    Form_CSRI::fields[Form_CSRI::idType::OPCODE],
    Form_CSRI::fields[Form_CSRI::idType::FUNC3]
};

const ImmediateType Form_CSRI::immediate_type = ImmediateType::UNSIGNED;

/**
 * FENCE-Form
 */
const char * Form_FENCE::name {"FENCE"};

const FieldsType Form_FENCE::fields {
    Field("fm", 28, 4),
    Field("pred", 24, 4),
    Field("succ", 20, 4),
    Field("rs1", 15, 5),
    Field("func3", 12, 3),
    Field("rd", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_FENCE::fmap {
    {"fm",     Form_FENCE::fields[Form_FENCE::idType::FM]},
    {"pred",   Form_FENCE::fields[Form_FENCE::idType::PRED]},
    {"succ",   Form_FENCE::fields[Form_FENCE::idType::SUCC]},
    {"rs1",    Form_FENCE::fields[Form_FENCE::idType::RS1]},
    {"func3",  Form_FENCE::fields[Form_FENCE::idType::FUNC3]},
    {"rd",     Form_FENCE::fields[Form_FENCE::idType::RD]},
    {"opcode", Form_FENCE::fields[Form_FENCE::idType::OPCODE]}
};

const std::map<std::string, Form_FENCE::idType> Form_FENCE::imap {
    {"fm",     Form_FENCE::idType::FM},
    {"pred",   Form_FENCE::idType::PRED},
    {"succ",   Form_FENCE::idType::SUCC},
    {"rs1",    Form_FENCE::idType::RS1},
    {"func3",  Form_FENCE::idType::FUNC3},
    {"rd",     Form_FENCE::idType::RD},
    {"opcode", Form_FENCE::idType::OPCODE}
};

const FieldsType Form_FENCE::opcode_fields {
    Form_FENCE::fields[Form_FENCE::idType::OPCODE],
    Form_FENCE::fields[Form_FENCE::idType::FUNC3]
};

const ImmediateType Form_FENCE::immediate_type = ImmediateType::NONE;

/**
 * I-Form
 */
const char * Form_I::name {"I"};

const FieldsType Form_I::fields {
    Field("imm", 20, 12),
    Field("rs1", 15, 5),
    Field("func3", 12, 3),
    Field("rd", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_I::fmap {
    {"imm",    Form_I::fields[Form_I::idType::IMM]},
    {"rs1",    Form_I::fields[Form_I::idType::RS1]},
    {"func3",  Form_I::fields[Form_I::idType::FUNC3]},
    {"rd",     Form_I::fields[Form_I::idType::RD]},
    {"opcode", Form_I::fields[Form_I::idType::OPCODE]}
};

const std::map<std::string, Form_I::idType> Form_I::imap {
    {"imm",    Form_I::idType::IMM},
    {"rs1",    Form_I::idType::RS1},
    {"func3",  Form_I::idType::FUNC3},
    {"rd",     Form_I::idType::RD},
    {"opcode", Form_I::idType::OPCODE}
};

const FieldsType Form_I::opcode_fields {
    Form_I::fields[Form_I::idType::OPCODE],
    Form_I::fields[Form_I::idType::FUNC3]
};

const ImmediateType Form_I::immediate_type = ImmediateType::SIGNED;

/**
 * ISH-Form
 */
const char * Form_ISH::name {"ISH"};

const FieldsType Form_ISH::fields {
    Field("func6", 26, 6),
    Field("shamt", 20, 6),
    Field("rs1", 15, 5),
    Field("func3", 12, 3),
    Field("rd", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_ISH::fmap {
    {"func6",  Form_ISH::fields[Form_ISH::idType::FUNC6]},
    {"shamt",  Form_ISH::fields[Form_ISH::idType::SHAMT]},
    {"rs1",    Form_ISH::fields[Form_ISH::idType::RS1]},
    {"func3",  Form_ISH::fields[Form_ISH::idType::FUNC3]},
    {"rd",     Form_ISH::fields[Form_ISH::idType::RD]},
    {"opcode", Form_ISH::fields[Form_ISH::idType::OPCODE]}
};

const std::map<std::string, Form_ISH::idType> Form_ISH::imap {
    {"func6",  Form_ISH::idType::FUNC6},
    {"shamt",  Form_ISH::idType::SHAMT},
    {"rs1",    Form_ISH::idType::RS1},
    {"func3",  Form_ISH::idType::FUNC3},
    {"rd",     Form_ISH::idType::RD},
    {"opcode", Form_ISH::idType::OPCODE}
};

const FieldsType Form_ISH::opcode_fields {
    Form_ISH::fields[Form_ISH::idType::OPCODE],
    Form_ISH::fields[Form_ISH::idType::FUNC3],
    Form_ISH::fields[Form_ISH::idType::FUNC6]
};

const ImmediateType Form_ISH::immediate_type = ImmediateType::SIGNED;

/**
 * ISHW-Form
 */
const char * Form_ISHW::name {"ISHW"};

const FieldsType Form_ISHW::fields {
    Field("func7", 25, 7),
    Field("shamtw", 20, 5),
    Field("rs1", 15, 5),
    Field("func3", 12, 3),
    Field("rd", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_ISHW::fmap {
    {"func7",  Form_ISHW::fields[Form_ISHW::idType::FUNC7]},
    {"shamtw", Form_ISHW::fields[Form_ISHW::idType::SHAMTW]},
    {"rs1",    Form_ISHW::fields[Form_ISHW::idType::RS1]},
    {"func3",  Form_ISHW::fields[Form_ISHW::idType::FUNC3]},
    {"rd",     Form_ISHW::fields[Form_ISHW::idType::RD]},
    {"opcode", Form_ISHW::fields[Form_ISHW::idType::OPCODE]}
};

const std::map<std::string, Form_ISHW::idType> Form_ISHW::imap {
    {"func7",  Form_ISHW::idType::FUNC7},
    {"shamtw", Form_ISHW::idType::SHAMTW},
    {"rs1",    Form_ISHW::idType::RS1},
    {"func3",  Form_ISHW::idType::FUNC3},
    {"rd",     Form_ISHW::idType::RD},
    {"opcode", Form_ISHW::idType::OPCODE}
};

const FieldsType Form_ISHW::opcode_fields {
    Form_ISHW::fields[Form_ISHW::idType::OPCODE],
    Form_ISHW::fields[Form_ISHW::idType::FUNC3],
    Form_ISHW::fields[Form_ISHW::idType::FUNC7]
};

const ImmediateType Form_ISHW::immediate_type = ImmediateType::SIGNED;

/**
 * J-Form
 */
const char * Form_J::name {"J"};

const FieldsType Form_J::fields { Field("imm20", 12, 20),
    Field("rd", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_J::fmap {
    {"imm20",  Form_J::fields[Form_J::idType::IMM20]},
    {"rd",     Form_J::fields[Form_J::idType::RD]},
    {"opcode", Form_J::fields[Form_J::idType::OPCODE]}
};

const std::map<std::string, Form_J::idType> Form_J::imap {
    {"imm20",  Form_J::idType::IMM20},
    {"rd",     Form_J::idType::RD},
    {"opcode", Form_J::idType::OPCODE}
};

const FieldsType Form_J::opcode_fields {
    Form_J::fields[Form_J::idType::OPCODE]
};

const ImmediateType Form_J::immediate_type = ImmediateType::SIGNED;

/**
 * R-Form
 */
const char * Form_R::name {"R"};

const FieldsType Form_R::fields {
    Field("func7", 25, 7),
    Field("rs2", 20, 5),
    Field("rs1", 15, 5),
    Field("func3", 12, 3),
    Field("rd", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_R::fmap {
    {"func7",  Form_R::fields[Form_R::idType::FUNC7]},
    {"rs2",    Form_R::fields[Form_R::idType::RS2]},
    {"rs1",    Form_R::fields[Form_R::idType::RS1]},
    {"func3",  Form_R::fields[Form_R::idType::FUNC3]},
    {"rd",     Form_R::fields[Form_R::idType::RD]},
    {"opcode", Form_R::fields[Form_R::idType::OPCODE]}
};

const std::map<std::string, Form_R::idType> Form_R::imap {
    {"func7",  Form_R::idType::FUNC7},
    {"rs2",    Form_R::idType::RS2},
    {"rs1",    Form_R::idType::RS1},
    {"func3",  Form_R::idType::FUNC3},
    {"rd",     Form_R::idType::RD},
    {"opcode", Form_R::idType::OPCODE}
};

const FieldsType Form_R::opcode_fields {
    Form_R::fields[Form_R::idType::OPCODE],
    Form_R::fields[Form_R::idType::FUNC3],
    Form_R::fields[Form_R::idType::FUNC7]
};

const ImmediateType Form_R::immediate_type = ImmediateType::NONE;

/**
 * Floating point R-Form
 */
const char * Form_Rfloat::name {"Rfloat"};

const FieldsType Form_Rfloat::fields {
    Field("func7", 25, 7),
    Field("rs2", 20, 5),
    Field("rs1", 15, 5),
    Field("rm", 12, 3),
    Field("rd", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_Rfloat::fmap {
    {"func7",  Form_Rfloat::fields[Form_Rfloat::idType::FUNC7]},
    {"rs2",    Form_Rfloat::fields[Form_Rfloat::idType::RS2]},
    {"rs1",    Form_Rfloat::fields[Form_Rfloat::idType::RS1]},
    {"rm",     Form_Rfloat::fields[Form_Rfloat::idType::RM]},
    {"rd",     Form_Rfloat::fields[Form_Rfloat::idType::RD]},
    {"opcode", Form_Rfloat::fields[Form_Rfloat::idType::OPCODE]}
};

const std::map<std::string, Form_Rfloat::idType> Form_Rfloat::imap {
    {"func7",  Form_Rfloat::idType::FUNC7},
    {"rs2",    Form_Rfloat::idType::RS2},
    {"rs1",    Form_Rfloat::idType::RS1},
    {"rm",     Form_Rfloat::idType::RM},
    {"rd",     Form_Rfloat::idType::RD},
    {"opcode", Form_Rfloat::idType::OPCODE}
};

const FieldsType Form_Rfloat::opcode_fields {
    Form_Rfloat::fields[Form_Rfloat::idType::OPCODE],
    Form_Rfloat::fields[Form_Rfloat::idType::FUNC7]
};

const ImmediateType Form_Rfloat::immediate_type = ImmediateType::NONE;

/**
 * R4-Form
 */
const char * Form_R4::name {"R4"};

const FieldsType Form_R4::fields {
    Field("rs3", 27, 5),
    Field("func2", 25, 2),
    Field("rs2", 20, 5),
    Field("rs1", 15, 5),
    Field("rm", 12, 3),
    Field("rd", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_R4::fmap {
    {"rs3",    Form_R4::fields[Form_R4::idType::RS3]},
    {"func2",  Form_R4::fields[Form_R4::idType::FUNC2]},
    {"rs2",    Form_R4::fields[Form_R4::idType::RS2]},
    {"rs1",    Form_R4::fields[Form_R4::idType::RS1]},
    {"rm",     Form_R4::fields[Form_R4::idType::RM]},
    {"rd",     Form_R4::fields[Form_R4::idType::RD]},
    {"opcode", Form_R4::fields[Form_R4::idType::OPCODE]}
};

const std::map<std::string, Form_R4::idType> Form_R4::imap {
    {"rs3",    Form_R4::idType::RS3},
    {"func2",  Form_R4::idType::FUNC2},
    {"rs2",    Form_R4::idType::RS2},
    {"rs1",    Form_R4::idType::RS1},
    {"rm",     Form_R4::idType::RM},
    {"rd",     Form_R4::idType::RD},
    {"opcode", Form_R4::idType::OPCODE}
};

const FieldsType Form_R4::opcode_fields {
    Form_R4::fields[Form_R4::idType::OPCODE],
    Form_R4::fields[Form_R4::idType::FUNC2]
};

const ImmediateType Form_R4::immediate_type = ImmediateType::NONE;

/**
 * S-Form
 */
const char * Form_S::name {"S"};

const FieldsType Form_S::fields {
    Field("imm7", 25, 7),
    Field("rs2", 20, 5),
    Field("rs1", 15, 5),
    Field("func3", 12, 3),
    Field("imm5", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_S::fmap {
    {"imm7",   Form_S::fields[Form_S::idType::IMM7]},
    {"rs2",    Form_S::fields[Form_S::idType::RS2]},
    {"rs1",    Form_S::fields[Form_S::idType::RS1]},
    {"func3",  Form_S::fields[Form_S::idType::FUNC3]},
    {"imm5",   Form_S::fields[Form_S::idType::IMM5]},
    {"opcode", Form_S::fields[Form_S::idType::OPCODE]}
};

const std::map<std::string, Form_S::idType> Form_S::imap {
    {"imm7",   Form_S::idType::IMM7},
    {"rs2",    Form_S::idType::RS2},
    {"rs1",    Form_S::idType::RS1},
    {"func3",  Form_S::idType::FUNC3},
    {"imm5",   Form_S::idType::IMM5},
    {"opcode", Form_S::idType::OPCODE}
};

const FieldsType Form_S::opcode_fields {
    Form_S::fields[Form_S::idType::OPCODE],
    Form_S::fields[Form_S::idType::FUNC3]
};

const ImmediateType Form_S::immediate_type = ImmediateType::SIGNED;

/**
 * U-Form
 */
const char * Form_U::name {"U"};

const FieldsType Form_U::fields {
    Field("imm20", 12, 20),
    Field("rd", 7, 5),
    Field("opcode", 0, 7)
};

const std::map<std::string, const Field &> Form_U::fmap {
    {"imm20",  Form_U::fields[Form_U::idType::IMM20]},
    {"rd",     Form_U::fields[Form_U::idType::RD]},
    {"opcode", Form_U::fields[Form_U::idType::OPCODE]}
};

const std::map<std::string, Form_U::idType> Form_U::imap {
    {"imm20",  Form_U::idType::IMM20},
    {"rd",     Form_U::idType::RD},
    {"opcode", Form_U::idType::OPCODE}
};

const FieldsType Form_U::opcode_fields {
    Form_U::fields[Form_U::idType::OPCODE]
};

const ImmediateType Form_U::immediate_type = ImmediateType::UNSIGNED;

} // namespace mavis
