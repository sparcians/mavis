#include "impl/forms/VectorForms.h"

namespace mavis
{

    /*
     * Form_V (most vector insts, base opcode 0x57)
     */
    const char* Form_V::name{"V"};

    const FieldsType Form_V::fields{
        Field("func1a", 31, 1), Field("func1b", 30, 1), Field("func3a", 27, 3),
        Field("func1c", 26, 1), Field("vm", 25, 1),     Field("rs2", 20, 5),
        Field("rs1", 15, 5),    Field("simm5", 15, 5),  Field("func3b", 12, 3),
        Field("rd", 7, 5),      Field("opcode", 0, 7)};

    const std::map<std::string, const Field &> Form_V::fmap{
        {"func1a", Form_V::fields[Form_V::idType::FUNC1A]},
        {"func1b", Form_V::fields[Form_V::idType::FUNC1B]},
        {"func3a", Form_V::fields[Form_V::idType::FUNC3A]},
        {"func1c", Form_V::fields[Form_V::idType::FUNC1C]},
        {"vm",     Form_V::fields[Form_V::idType::VM]    },
        {"rs2",    Form_V::fields[Form_V::idType::RS2]   },
        {"rs1",    Form_V::fields[Form_V::idType::RS1]   },
        {"simm5",  Form_V::fields[Form_V::idType::SIMM5] },
        {"func3b", Form_V::fields[Form_V::idType::FUNC3B]},
        {"rd",     Form_V::fields[Form_V::idType::RD]    },
        {"opcode", Form_V::fields[Form_V::idType::OPCODE]}
    };

    const std::map<std::string, Form_V::idType> Form_V::imap{
        {"func1a", Form_V::idType::FUNC1A},
        {"func1b", Form_V::idType::FUNC1B},
        {"func3a", Form_V::idType::FUNC3A},
        {"func1c", Form_V::idType::FUNC1C},
        {"vm",     Form_V::idType::VM    },
        {"rs2",    Form_V::idType::RS2   },
        {"rs1",    Form_V::idType::RS1   },
        {"simm5",  Form_V::idType::SIMM5 },
        {"func3b", Form_V::idType::FUNC3B},
        {"rd",     Form_V::idType::RD    },
        {"opcode", Form_V::idType::OPCODE}
    };

    const FieldsType Form_V::opcode_fields{
        Form_V::fields[Form_V::idType::OPCODE], Form_V::fields[Form_V::idType::FUNC3A],
        Form_V::fields[Form_V::idType::FUNC1A], Form_V::fields[Form_V::idType::FUNC1B],
        Form_V::fields[Form_V::idType::FUNC1C], Form_V::fields[Form_V::idType::FUNC3B]};

    const ImmediateType Form_V::immediate_type = ImmediateType::NONE;

    /*
     * Form_VF_mem (float and vector load form)
     */
    const char* Form_VF_mem::name{"VF_mem"};

    const FieldsType Form_VF_mem::fields{
        Field("nf", 29, 3),  Field("mewop", 26, 3), Field("vm", 25, 1),
        Field("rs2", 20, 5), Field("rs1", 15, 5),   Field("width", 12, 3),
        Field("rd", 7, 5),   Field("rs3", 7, 5),    Field("opcode", 0, 7)};

    const std::map<std::string, const Field &> Form_VF_mem::fmap{
        {"nf",     Form_VF_mem::fields[Form_VF_mem::idType::NF]    },
        {"mewop",  Form_VF_mem::fields[Form_VF_mem::idType::MEWOP] },
        {"vm",     Form_VF_mem::fields[Form_VF_mem::idType::VM]    },
        {"rs2",    Form_VF_mem::fields[Form_VF_mem::idType::RS2]   },
        {"rs1",    Form_VF_mem::fields[Form_VF_mem::idType::RS1]   },
        {"width",  Form_VF_mem::fields[Form_VF_mem::idType::WIDTH] },
        {"rd",     Form_VF_mem::fields[Form_VF_mem::idType::RD]    },
        {"rs3",    Form_VF_mem::fields[Form_VF_mem::idType::RS3]   },
        {"opcode", Form_VF_mem::fields[Form_VF_mem::idType::OPCODE]}
    };

    const std::map<std::string, Form_VF_mem::idType> Form_VF_mem::imap{
        {"nf",     Form_VF_mem::idType::NF    },
        {"mewop",  Form_VF_mem::idType::MEWOP },
        {"vm",     Form_VF_mem::idType::VM    },
        {"rs2",    Form_VF_mem::idType::RS2   },
        {"rs1",    Form_VF_mem::idType::RS1   },
        {"width",  Form_VF_mem::idType::WIDTH },
        {"rd",     Form_VF_mem::idType::RD    },
        {"rs3",    Form_VF_mem::idType::RS3   },
        {"opcode", Form_VF_mem::idType::OPCODE}
    };

    const FieldsType Form_VF_mem::opcode_fields{Form_VF_mem::fields[Form_VF_mem::idType::OPCODE],
                                                Form_VF_mem::fields[Form_VF_mem::idType::WIDTH],
                                                Form_VF_mem::fields[Form_VF_mem::idType::MEWOP]};

    const ImmediateType Form_VF_mem::immediate_type = ImmediateType::NONE;

    /*
     * Form_V_vsetvli (for vsetvli instruction -- extraction only)
     */
    const char* Form_V_vsetvli::name{"V_vsetvli"};

    const FieldsType Form_V_vsetvli::fields{Field("func1", 31, 1), Field("imm11", 20, 11),
                                            Field("rs1", 15, 5),   Field("func3", 12, 3),
                                            Field("rd", 7, 5),     Field("opcode", 0, 7)};

    const std::map<std::string, const Field &> Form_V_vsetvli::fmap{
        {"func1",  Form_V_vsetvli::fields[Form_V_vsetvli::idType::FUNC1] },
        {"imm11",  Form_V_vsetvli::fields[Form_V_vsetvli::idType::IMM11] },
        {"rs1",    Form_V_vsetvli::fields[Form_V_vsetvli::idType::RS1]   },
        {"func3",  Form_V_vsetvli::fields[Form_V_vsetvli::idType::FUNC3] },
        {"rd",     Form_V_vsetvli::fields[Form_V_vsetvli::idType::RD]    },
        {"opcode", Form_V_vsetvli::fields[Form_V_vsetvli::idType::OPCODE]}
    };

    const std::map<std::string, Form_V_vsetvli::idType> Form_V_vsetvli::imap{
        {"func1",  Form_V_vsetvli::idType::FUNC1 },
        {"imm11",  Form_V_vsetvli::idType::IMM11 },
        {"rs1",    Form_V_vsetvli::idType::RS1   },
        {"func3",  Form_V_vsetvli::idType::FUNC3 },
        {"rd",     Form_V_vsetvli::idType::RD    },
        {"opcode", Form_V_vsetvli::idType::OPCODE}
    };

    const FieldsType Form_V_vsetvli::opcode_fields{
        Form_V_vsetvli::fields[Form_V_vsetvli::idType::OPCODE],
        Form_V_vsetvli::fields[Form_V_vsetvli::idType::FUNC3],
        Form_V_vsetvli::fields[Form_V_vsetvli::idType::FUNC1]};

    const ImmediateType Form_V_vsetvli::immediate_type = ImmediateType::UNSIGNED;

    /*
     * Form_V_vsetivli (for vsetivli instruction -- extraction only)
     */
    const char* Form_V_vsetivli::name{"V_vsetivli"};

    const FieldsType Form_V_vsetivli::fields{Field("func2", 30, 2), Field("imm10", 20, 10),
                                             Field("avl", 15, 5),   Field("func3", 12, 3),
                                             Field("rd", 7, 5),     Field("opcode", 0, 7)};

    const std::map<std::string, const Field &> Form_V_vsetivli::fmap{
        {"func2",  Form_V_vsetivli::fields[Form_V_vsetivli::idType::FUNC2] },
        {"imm10",  Form_V_vsetivli::fields[Form_V_vsetivli::idType::IMM10] },
        {"avl",    Form_V_vsetivli::fields[Form_V_vsetivli::idType::AVL]   },
        {"func3",  Form_V_vsetivli::fields[Form_V_vsetivli::idType::FUNC3] },
        {"rd",     Form_V_vsetivli::fields[Form_V_vsetivli::idType::RD]    },
        {"opcode", Form_V_vsetivli::fields[Form_V_vsetivli::idType::OPCODE]}
    };

    const std::map<std::string, Form_V_vsetivli::idType> Form_V_vsetivli::imap{
        {"func2",  Form_V_vsetivli::idType::FUNC2 },
        {"imm10",  Form_V_vsetivli::idType::IMM10 },
        {"avl",    Form_V_vsetivli::idType::AVL   },
        {"func3",  Form_V_vsetivli::idType::FUNC3 },
        {"rd",     Form_V_vsetivli::idType::RD    },
        {"opcode", Form_V_vsetivli::idType::OPCODE}
    };

    const FieldsType Form_V_vsetivli::opcode_fields{
        Form_V_vsetivli::fields[Form_V_vsetivli::idType::OPCODE],
        Form_V_vsetivli::fields[Form_V_vsetivli::idType::FUNC3],
        Form_V_vsetivli::fields[Form_V_vsetivli::idType::FUNC2]};

    const ImmediateType Form_V_vsetivli::immediate_type = ImmediateType::UNSIGNED;

    /*
     * Form_V_vsetvl (for vsetvl instruction -- extraction only)
     */
    const char* Form_V_vsetvl::name{"V_vsetvl"};

    const FieldsType Form_V_vsetvl::fields{Field("func7", 25, 7), Field("rs2", 20, 5),
                                           Field("rs1", 15, 5),   Field("func3", 12, 3),
                                           Field("rd", 7, 5),     Field("opcode", 0, 7)};

    const std::map<std::string, const Field &> Form_V_vsetvl::fmap{
        {"func7",  Form_V_vsetvl::fields[Form_V_vsetvl::idType::FUNC7] },
        {"rs2",    Form_V_vsetvl::fields[Form_V_vsetvl::idType::RS2]   },
        {"rs1",    Form_V_vsetvl::fields[Form_V_vsetvl::idType::RS1]   },
        {"func3",  Form_V_vsetvl::fields[Form_V_vsetvl::idType::FUNC3] },
        {"rd",     Form_V_vsetvl::fields[Form_V_vsetvl::idType::RD]    },
        {"opcode", Form_V_vsetvl::fields[Form_V_vsetvl::idType::OPCODE]}
    };

    const std::map<std::string, Form_V_vsetvl::idType> Form_V_vsetvl::imap{
        {"func7",  Form_V_vsetvl::idType::FUNC7 },
        {"rs2",    Form_V_vsetvl::idType::RS2   },
        {"rs1",    Form_V_vsetvl::idType::RS1   },
        {"func3",  Form_V_vsetvl::idType::FUNC3 },
        {"rd",     Form_V_vsetvl::idType::RD    },
        {"opcode", Form_V_vsetvl::idType::OPCODE}
    };

    const FieldsType Form_V_vsetvl::opcode_fields{
        Form_V_vsetvl::fields[Form_V_vsetvl::idType::OPCODE],
        Form_V_vsetvl::fields[Form_V_vsetvl::idType::FUNC7]};

    const ImmediateType Form_V_vsetvl::immediate_type = ImmediateType::NONE;

    /*
     * Form_V_uimm6 (most vector insts, base opcode 0x57)
     */
    const char* Form_V_uimm6::name{"V_uimm6"};

    const FieldsType Form_V_uimm6::fields{
        Field("func5", 27, 5), Field("i5", 26, 1),    Field("vm", 25, 1), Field("rs2", 20, 5),
        Field("uimm5", 15, 5), Field("func3", 12, 3), Field("rd", 7, 5),  Field("opcode", 0, 7)};

    const std::map<std::string, const Field &> Form_V_uimm6::fmap{
        {"func5",  Form_V_uimm6::fields[Form_V_uimm6::idType::FUNC5] },
        {"i5",     Form_V_uimm6::fields[Form_V_uimm6::idType::I5]    },
        {"vm",     Form_V_uimm6::fields[Form_V_uimm6::idType::VM]    },
        {"rs2",    Form_V_uimm6::fields[Form_V_uimm6::idType::RS2]   },
        {"uimm5",  Form_V_uimm6::fields[Form_V_uimm6::idType::UIMM5] },
        {"func3",  Form_V_uimm6::fields[Form_V_uimm6::idType::FUNC3] },
        {"rd",     Form_V_uimm6::fields[Form_V_uimm6::idType::RD]    },
        {"opcode", Form_V_uimm6::fields[Form_V_uimm6::idType::OPCODE]}
    };

    const std::map<std::string, Form_V_uimm6::idType> Form_V_uimm6::imap{
        {"func5",  Form_V_uimm6::idType::FUNC5 },
        {"i5",     Form_V_uimm6::idType::I5    },
        {"vm",     Form_V_uimm6::idType::VM    },
        {"rs2",    Form_V_uimm6::idType::RS2   },
        {"uimm5",  Form_V_uimm6::idType::UIMM5 },
        {"func3",  Form_V_uimm6::idType::FUNC3 },
        {"rd",     Form_V_uimm6::idType::RD    },
        {"opcode", Form_V_uimm6::idType::OPCODE}
    };

    const FieldsType Form_V_uimm6::opcode_fields{Form_V_uimm6::fields[Form_V_uimm6::idType::OPCODE],
                                                 Form_V_uimm6::fields[Form_V_uimm6::idType::FUNC3],
                                                 Form_V_uimm6::fields[Form_V_uimm6::idType::FUNC5]};

    const ImmediateType Form_V_uimm6::immediate_type = ImmediateType::NONE;
} // namespace mavis
