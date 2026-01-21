#include "impl/forms/CompressedForms.h"

namespace mavis
{

    /**
     * C0-Form: INTENDED FOR DECODING USE -- probably not useful for general extraction
     */
    const char* Form_C0::name{"C0"};

    const FieldsType Form_C0::fields{
        Field("func3", 13, 3), Field("imm3", 10, 3), Field("rs1", 7, 3),   Field("func2A", 6, 1),
        Field("imm2", 5, 2),   Field("rd", 2, 3),    Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_C0::fmap{
        {"func3",  Form_C0::fields[Form_C0::idType::FUNC3] },
        {"imm3",   Form_C0::fields[Form_C0::idType::IMM3]  },
        {"rs1",    Form_C0::fields[Form_C0::idType::RS1]   },
        {"func2A", Form_C0::fields[Form_C0::idType::FUNC2A]},
        {"imm2",   Form_C0::fields[Form_C0::idType::IMM2]  },
        {"rd",     Form_C0::fields[Form_C0::idType::RD]    },
        {"opcode", Form_C0::fields[Form_C0::idType::OPCODE]}
    };

    const std::map<std::string, Form_C0::idType> Form_C0::imap{
        {"func3",  Form_C0::idType::FUNC3 },
        {"imm3",   Form_C0::idType::IMM3  },
        {"rs1",    Form_C0::idType::RS1   },
        {"func2A", Form_C0::idType::FUNC2A},
        {"imm2",   Form_C0::idType::IMM2  },
        {"rd",     Form_C0::idType::RD    },
        {"opcode", Form_C0::idType::OPCODE}
    };

    const FieldsType Form_C0::opcode_fields{Form_C0::fields[Form_C0::idType::OPCODE],
                                            Form_C0::fields[Form_C0::idType::FUNC3],
                                            Form_C0::fields[Form_C0::idType::FUNC2A]};

    const ImmediateType Form_C0::immediate_type = ImmediateType::UNSIGNED;

    /**
     * C1-Form: INTENDED FOR DECODING USE -- probably not useful for general extraction
     */
    const char* Form_C1::name{"C1"};

    const FieldsType Form_C1::fields{
        Field("func3", 13, 3), Field("func1", 12, 1), Field("func2", 10, 2),
        Field("rs1", 7, 3), // RD and RS1 are aliases for the same field
        Field("rd", 7, 3),  // RD and RS1 are aliases for the same field
        Field("func2b", 5, 2), Field("rs2", 2, 3), Field("imm5", 2, 5), Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_C1::fmap{
        {"func3",  Form_C1::fields[Form_C1::idType::FUNC3] },
        {"func1",  Form_C1::fields[Form_C1::idType::FUNC1] },
        {"func2",  Form_C1::fields[Form_C1::idType::FUNC2] },
        {"rs1",    Form_C1::fields[Form_C1::idType::RS1]   },
        {"rd",     Form_C1::fields[Form_C1::idType::RD]    },
        {"func2b", Form_C1::fields[Form_C1::idType::FUNC2B]},
        {"rs2",    Form_C1::fields[Form_C1::idType::RS2]   },
        {"imm5",   Form_C1::fields[Form_C1::idType::IMM5]   },
        {"opcode", Form_C1::fields[Form_C1::idType::OPCODE]}
    };

    const std::map<std::string, Form_C1::idType> Form_C1::imap{
        {"func3",  Form_C1::idType::FUNC3 },
        {"func1",  Form_C1::idType::FUNC1 },
        {"func2",  Form_C1::idType::FUNC2 },
        {"rs1",    Form_C1::idType::RS1   },
        {"rd",     Form_C1::idType::RD    },
        {"func2b", Form_C1::idType::FUNC2B},
        {"rs2",    Form_C1::idType::RS2   },
        {"imm5",   Form_C1::idType::IMM5  },
        {"opcode", Form_C1::idType::OPCODE}
    };

    const FieldsType Form_C1::opcode_fields{
        Form_C1::fields[Form_C1::idType::OPCODE], Form_C1::fields[Form_C1::idType::FUNC3],
        Form_C1::fields[Form_C1::idType::FUNC2],  Form_C1::fields[Form_C1::idType::FUNC1],
        Form_C1::fields[Form_C1::idType::IMM5], Form_C1::fields[Form_C1::idType::FUNC2B],
    };

    const ImmediateType Form_C1::immediate_type = ImmediateType::NONE;

    /**
     * C2-Form: INTENDED FOR DECODING USE -- probably not useful for general extraction
     */
    const char* Form_C2::name{"C2"};

    const FieldsType Form_C2::fields{Field("func3", 13, 3),  Field("func1", 12, 1),
                                     Field("shamt1", 12, 1),                     // Alias for FUNC1
                                     Field("rd", 7, 5),      Field("rs1", 7, 5), // Alias for RD
                                     Field("rs", 2, 5),      Field("rs2", 2, 5), // Alias for RS
                                     Field("shamt5", 2, 5),                      // Alias for RS
                                     Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_C2::fmap{
        {"func3",  Form_C2::fields[Form_C2::idType::FUNC3] },
        {"func1",  Form_C2::fields[Form_C2::idType::FUNC1] },
        {"shamt1", Form_C2::fields[Form_C2::idType::SHAMT1]},
        {"rd",     Form_C2::fields[Form_C2::idType::RD]    },
        {"rs1",    Form_C2::fields[Form_C2::idType::RS1]   },
        {"rs",     Form_C2::fields[Form_C2::idType::RS]    },
        {"rs2",    Form_C2::fields[Form_C2::idType::RS2]   },
        {"shamt5", Form_C2::fields[Form_C2::idType::SHAMT5]},
        {"opcode", Form_C2::fields[Form_C2::idType::OPCODE]}
    };

    const std::map<std::string, Form_C2::idType> Form_C2::imap{
        {"func3",  Form_C2::idType::FUNC3 },
        {"func1",  Form_C2::idType::FUNC1 },
        {"shamt1", Form_C2::idType::SHAMT1},
        {"rd",     Form_C2::idType::RD    },
        {"rs1",    Form_C2::idType::RS1   },
        {"rs",     Form_C2::idType::RS    },
        {"rs2",    Form_C2::idType::RS2   },
        {"shamt5", Form_C2::idType::SHAMT5},
        {"opcode", Form_C2::idType::OPCODE}
    };

    const FieldsType Form_C2::opcode_fields{Form_C2::fields[Form_C2::idType::OPCODE],
                                            Form_C2::fields[Form_C2::idType::FUNC3],
                                            Form_C2::fields[Form_C2::idType::FUNC1]};

    const ImmediateType Form_C2::immediate_type = ImmediateType::NONE;

    /**
     * C2_sp_store-Form: INTENDED FOR EXTRACTION USE ONLY
     */
    const char* Form_C2_sp_store::name{"C2_sp_store"};

    const FieldsType Form_C2_sp_store::fields{Field("func3", 13, 3), Field("imm", 7, 6),
                                              Field("rs2", 2, 5), Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_C2_sp_store::fmap{
        {"func3",  Form_C2_sp_store::fields[Form_C2_sp_store::idType::FUNC3] },
        {"imm",    Form_C2_sp_store::fields[Form_C2_sp_store::idType::IMM]   },
        {"rs2",    Form_C2_sp_store::fields[Form_C2_sp_store::idType::RS2]   },
        {"opcode", Form_C2_sp_store::fields[Form_C2_sp_store::idType::OPCODE]}
    };

    const std::map<std::string, Form_C2_sp_store::idType> Form_C2_sp_store::imap{
        {"func3",  Form_C2_sp_store::idType::FUNC3 },
        {"imm",    Form_C2_sp_store::idType::IMM   },
        {"rs2",    Form_C2_sp_store::idType::RS2   },
        {"opcode", Form_C2_sp_store::idType::OPCODE}
    };

    const FieldsType Form_C2_sp_store::opcode_fields{
        Form_C2_sp_store::fields[Form_C2_sp_store::idType::OPCODE],
        Form_C2_sp_store::fields[Form_C2_sp_store::idType::FUNC3]};

    const ImmediateType Form_C2_sp_store::immediate_type = ImmediateType::UNSIGNED;

    /**
     * CA-Form
     */
    const char* Form_CA::name{"CA"};

    const FieldsType Form_CA::fields{Field("func6", 10, 6), Field("rs1", 7, 3),
                                     Field("rd", 7, 3),     Field("func2", 5, 2),
                                     Field("rs2", 2, 3),    Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_CA::fmap{
        {"func6",  Form_CA::fields[Form_CA::idType::FUNC6] },
        {"rs1",    Form_CA::fields[Form_CA::idType::RS1]   },
        {"rd",     Form_CA::fields[Form_CA::idType::RD]    },
        {"func2",  Form_CA::fields[Form_CA::idType::FUNC2] },
        {"rs2",    Form_CA::fields[Form_CA::idType::RS2]   },
        {"opcode", Form_CA::fields[Form_CA::idType::OPCODE]}
    };

    const std::map<std::string, Form_CA::idType> Form_CA::imap{
        {"func6",  Form_CA::idType::FUNC6 },
        {"rs1",    Form_CA::idType::RS1   },
        {"rd",     Form_CA::idType::RD    },
        {"func2",  Form_CA::idType::FUNC2 },
        {"rs2",    Form_CA::idType::RS2   },
        {"opcode", Form_CA::idType::OPCODE}
    };

    const FieldsType Form_CA::opcode_fields{Form_CA::fields[Form_CA::idType::OPCODE],
                                            Form_CA::fields[Form_CA::idType::FUNC6],
                                            Form_CA::fields[Form_CA::idType::FUNC2]};

    const ImmediateType Form_CA::immediate_type = ImmediateType::NONE;

    /**
     * CB-Form
     */
    const char* Form_CB::name{"CB"};

    const FieldsType Form_CB::fields{Field("func3", 13, 3), Field("imm3", 10, 3),
                                     Field("rs1", 7, 3), Field("imm5", 2, 5),
                                     Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_CB::fmap{
        {"func3",  Form_CB::fields[Form_CB::idType::FUNC3] },
        {"imm3",   Form_CB::fields[Form_CB::idType::IMM3]  },
        {"rs1",    Form_CB::fields[Form_CB::idType::RS1]   },
        {"imm5",   Form_CB::fields[Form_CB::idType::IMM5]  },
        {"opcode", Form_CB::fields[Form_CB::idType::OPCODE]}
    };

    const std::map<std::string, Form_CB::idType> Form_CB::imap{
        {"func3",  Form_CB::idType::FUNC3 },
        {"imm3",   Form_CB::idType::IMM3  },
        {"rs1",    Form_CB::idType::RS1   },
        {"imm5",   Form_CB::idType::IMM5  },
        {"opcode", Form_CB::idType::OPCODE}
    };

    const FieldsType Form_CB::opcode_fields{Form_CB::fields[Form_CB::idType::OPCODE],
                                            Form_CB::fields[Form_CB::idType::FUNC3]};

    const ImmediateType Form_CB::immediate_type = ImmediateType::SIGNED;

    /**
     * CI-Form
     */
    const char* Form_CI::name{"CI"};

    const FieldsType Form_CI::fields{Field("func3", 13, 3), Field("imm1", 12, 1),
                                     Field("rs1", 7, 5),    Field("rd", 7, 5),
                                     Field("imm5", 2, 5),   Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_CI::fmap{
        {"func3",  Form_CI::fields[Form_CI::idType::FUNC3] },
        {"imm1",   Form_CI::fields[Form_CI::idType::IMM1]  },
        {"rs1",    Form_CI::fields[Form_CI::idType::RS1]   },
        {"rd",     Form_CI::fields[Form_CI::idType::RD]    },
        {"imm5",   Form_CI::fields[Form_CI::idType::IMM5]  },
        {"opcode", Form_CI::fields[Form_CI::idType::OPCODE]}
    };

    const std::map<std::string, Form_CI::idType> Form_CI::imap{
        {"func3",  Form_CI::idType::FUNC3 },
        {"imm1",   Form_CI::idType::IMM1  },
        {"rs1",    Form_CI::idType::RS1   },
        {"rd",     Form_CI::idType::RD    },
        {"imm5",   Form_CI::idType::IMM5  },
        {"opcode", Form_CI::idType::OPCODE}
    };

    const FieldsType Form_CI::opcode_fields{Form_CI::fields[Form_CI::idType::OPCODE],
                                            Form_CI::fields[Form_CI::idType::FUNC3]};

    const ImmediateType Form_CI::immediate_type = ImmediateType::UNSIGNED;

    /**
     * CI_rD_only-Form (for c.li and c.lui)
     */
    const char* Form_CI_rD_only::name{"CI_rD_only"};

    const FieldsType Form_CI_rD_only::fields{Field("func3", 13, 3), Field("imm1", 12, 1),
                                             Field("rd", 7, 5), Field("imm5", 2, 5),
                                             Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_CI_rD_only::fmap{
        {"func3",  Form_CI_rD_only::fields[Form_CI_rD_only::idType::FUNC3] },
        {"imm1",   Form_CI_rD_only::fields[Form_CI_rD_only::idType::IMM1]  },
        {"rd",     Form_CI_rD_only::fields[Form_CI_rD_only::idType::RD]    },
        {"imm5",   Form_CI_rD_only::fields[Form_CI_rD_only::idType::IMM5]  },
        {"opcode", Form_CI_rD_only::fields[Form_CI_rD_only::idType::OPCODE]}
    };

    const std::map<std::string, Form_CI_rD_only::idType> Form_CI_rD_only::imap{
        {"func3",  Form_CI_rD_only::idType::FUNC3 },
        {"imm1",   Form_CI_rD_only::idType::IMM1  },
        {"rd",     Form_CI_rD_only::idType::RD    },
        {"imm5",   Form_CI_rD_only::idType::IMM5  },
        {"opcode", Form_CI_rD_only::idType::OPCODE}
    };

    const FieldsType Form_CI_rD_only::opcode_fields{
        Form_CI_rD_only::fields[Form_CI_rD_only::idType::OPCODE],
        Form_CI_rD_only::fields[Form_CI_rD_only::idType::FUNC3]};

    const ImmediateType Form_CI_rD_only::immediate_type = ImmediateType::SIGNED;

    /**
     * CIW-Form
     */
    const char* Form_CIW::name{"CIW"};

    const FieldsType Form_CIW::fields{Field("func3", 13, 3), Field("imm8", 5, 8), Field("rd", 2, 3),
                                      Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_CIW::fmap{
        {"func3",  Form_CIW::fields[Form_CIW::idType::FUNC3] },
        {"imm8",   Form_CIW::fields[Form_CIW::idType::IMM8]  },
        {"rd",     Form_CIW::fields[Form_CIW::idType::RD]    },
        {"opcode", Form_CIW::fields[Form_CIW::idType::OPCODE]}
    };

    const std::map<std::string, Form_CIW::idType> Form_CIW::imap{
        {"func3",  Form_CIW::idType::FUNC3 },
        {"imm8",   Form_CIW::idType::IMM8  },
        {"rd",     Form_CIW::idType::RD    },
        {"opcode", Form_CIW::idType::OPCODE}
    };

    const FieldsType Form_CIW::opcode_fields{Form_CIW::fields[Form_CIW::idType::OPCODE],
                                             Form_CIW::fields[Form_CIW::idType::FUNC3]};

    const ImmediateType Form_CIW::immediate_type = ImmediateType::SIGNED;

    /**
     * CIX-Form: Extension from spec for C.SRLI/C.SRAI/C.ANDI
     */
    const char* Form_CIX::name{"CIX"};

    const FieldsType Form_CIX::fields{
        Field("func3", 13, 3), Field("shamt1", 12, 1), Field("func2", 10, 2), Field("rs1", 7, 3),
        Field("rd", 7, 3),     Field("shamt5", 2, 5),  Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_CIX::fmap{
        {"func3",  Form_CIX::fields[Form_CIX::idType::FUNC3] },
        {"shamt1", Form_CIX::fields[Form_CIX::idType::SHAMT1]},
        {"func2",  Form_CIX::fields[Form_CIX::idType::FUNC2] },
        {"rs1",    Form_CIX::fields[Form_CIX::idType::RS1]   },
        {"rd",     Form_CIX::fields[Form_CIX::idType::RD]    },
        {"shamt5", Form_CIX::fields[Form_CIX::idType::SHAMT5]},
        {"opcode", Form_CIX::fields[Form_CIX::idType::OPCODE]}
    };

    const std::map<std::string, Form_CIX::idType> Form_CIX::imap{
        {"func3",  Form_CIX::idType::FUNC3 },
        {"shamt1", Form_CIX::idType::SHAMT1},
        {"func2",  Form_CIX::idType::FUNC2 },
        {"rs1",    Form_CIX::idType::RS1   },
        {"rd",     Form_CIX::idType::RD    },
        {"shamt5", Form_CIX::idType::SHAMT5},
        {"opcode", Form_CIX::idType::OPCODE}
    };

    const FieldsType Form_CIX::opcode_fields{Form_CIX::fields[Form_CIX::idType::OPCODE],
                                             Form_CIX::fields[Form_CIX::idType::FUNC3],
                                             Form_CIX::fields[Form_CIX::idType::FUNC2]};

    const ImmediateType Form_CIX::immediate_type = ImmediateType::UNSIGNED;

    /**
     * CJ-Form
     */
    const char* Form_CJ::name{"CJ"};

    const FieldsType Form_CJ::fields{Field("func3", 13, 3), Field("imm11", 2, 11),
                                     Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_CJ::fmap{
        {"func3",  Form_CJ::fields[Form_CJ::idType::FUNC3] },
        {"imm11",  Form_CJ::fields[Form_CJ::idType::IMM11] },
        {"opcode", Form_CJ::fields[Form_CJ::idType::OPCODE]}
    };

    const std::map<std::string, Form_CJ::idType> Form_CJ::imap{
        {"func3",  Form_CJ::idType::FUNC3 },
        {"imm11",  Form_CJ::idType::IMM11 },
        {"opcode", Form_CJ::idType::OPCODE}
    };

    const FieldsType Form_CJ::opcode_fields{Form_CJ::fields[Form_CJ::idType::OPCODE],
                                            Form_CJ::fields[Form_CJ::idType::FUNC3]};

    const ImmediateType Form_CJ::immediate_type = ImmediateType::SIGNED;

    /**
     * CJR-Form (for extraction only)
     */
    const char* Form_CJR::name{"CJR"};

    const FieldsType Form_CJR::fields{Field("func4", 12, 4), Field("rs1", 7, 5), Field("rs2", 2, 5),
                                      Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_CJR::fmap{
        {"func4",  Form_CJR::fields[Form_CJR::idType::FUNC4] },
        {"rs1",    Form_CJR::fields[Form_CJR::idType::RS1]   },
        {"rs2",    Form_CJR::fields[Form_CJR::idType::RS2]   },
        {"opcode", Form_CJR::fields[Form_CJR::idType::OPCODE]}
    };

    const std::map<std::string, Form_CJR::idType> Form_CJR::imap{
        {"func4",  Form_CJR::idType::FUNC4 },
        {"rs1",    Form_CJR::idType::RS1   },
        {"rs2",    Form_CJR::idType::RS2   },
        {"opcode", Form_CJR::idType::OPCODE}
    };

    const FieldsType Form_CJR::opcode_fields{Form_CJR::fields[Form_CJR::idType::OPCODE],
                                             Form_CJR::fields[Form_CJR::idType::FUNC4]};

    const ImmediateType Form_CJR::immediate_type = ImmediateType::NONE;

    /**
     * CMPP-Form
     */
    const char* Form_CMPP::name{"CMPP"};

    const FieldsType Form_CMPP::fields{
        Field("func3", 13, 3),  Field("func1", 12, 1), // func6 is split into func3, func1, and
                                                       // func2A
        Field("func2A", 10, 2), Field("func2", 8, 2),  Field("urlist", 4, 4),
        Field("spimm", 2, 2),   Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_CMPP::fmap{
        {"func3",  Form_CMPP::fields[Form_CMPP::idType::FUNC3] },
        {"func1",  Form_CMPP::fields[Form_CMPP::idType::FUNC1] },
        {"func2A", Form_CMPP::fields[Form_CMPP::idType::FUNC2A]},
        {"func2",  Form_CMPP::fields[Form_CMPP::idType::FUNC2] },
        {"urlist", Form_CMPP::fields[Form_CMPP::idType::URLIST]},
        {"spimm",  Form_CMPP::fields[Form_CMPP::idType::SPIMM] },
        {"opcode", Form_CMPP::fields[Form_CMPP::idType::OPCODE]}
    };

    const std::map<std::string, Form_CMPP::idType> Form_CMPP::imap{
        {"func3",  Form_CMPP::idType::FUNC3 },
        {"func1",  Form_CMPP::idType::FUNC1 },
        {"func2A", Form_CMPP::idType::FUNC2A},
        {"func2",  Form_CMPP::idType::FUNC2 },
        {"urlist", Form_CMPP::idType::URLIST},
        {"spimm",  Form_CMPP::idType::SPIMM },
        {"opcode", Form_CMPP::idType::OPCODE}
    };

    const FieldsType Form_CMPP::opcode_fields{
        Form_CMPP::fields[Form_CMPP::idType::OPCODE], Form_CMPP::fields[Form_CMPP::idType::FUNC3],
        Form_CMPP::fields[Form_CMPP::idType::FUNC2A], Form_CMPP::fields[Form_CMPP::idType::FUNC1],
        Form_CMPP::fields[Form_CMPP::idType::FUNC2]};

    const ImmediateType Form_CMPP::immediate_type = ImmediateType::SIGNED;

    /**
     * CMJT-Form
     */
    const char* Form_CMJT::name{"CMJT"};

    const FieldsType Form_CMJT::fields{
        Field("func3", 13, 3),
        Field("func1", 12, 1), // func6 is split into func3, func1, and func2A
        Field("func2A", 10, 2), Field("index", 2, 8), Field("opcode", 0, 2)};

    const std::map<std::string, const Field &> Form_CMJT::fmap{
        {"func3",  Form_CMJT::fields[Form_CMJT::idType::FUNC3] },
        {"func1",  Form_CMJT::fields[Form_CMJT::idType::FUNC1] },
        {"func2A", Form_CMJT::fields[Form_CMJT::idType::FUNC2A]},
        {"index",  Form_CMJT::fields[Form_CMJT::idType::INDEX] },
        {"opcode", Form_CMJT::fields[Form_CMJT::idType::OPCODE]}
    };

    const std::map<std::string, Form_CMJT::idType> Form_CMJT::imap{
        {"func3",  Form_CMJT::idType::FUNC3 },
        {"func1",  Form_CMJT::idType::FUNC1 },
        {"func2A", Form_CMJT::idType::FUNC2A},
        {"index",  Form_CMJT::idType::INDEX },
        {"opcode", Form_CMJT::idType::OPCODE}
    };

    const FieldsType Form_CMJT::opcode_fields{
        Form_CMJT::fields[Form_CMJT::idType::OPCODE], Form_CMJT::fields[Form_CMJT::idType::FUNC3],
        Form_CMJT::fields[Form_CMJT::idType::FUNC2A], Form_CMJT::fields[Form_CMJT::idType::FUNC1]};

    const ImmediateType Form_CMJT::immediate_type = ImmediateType::UNSIGNED;

} // namespace mavis
