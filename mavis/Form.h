#pragma once

#include "Field.h"
#include "DecoderExceptions.h"
#include "FormIF.h"
#include <ostream>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include <map>

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

private:
    static inline const std::string name_ {"AMO"};

    static inline const std::array<const Field, idType::__N> fields_ {
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

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func5",  fields_[idType::FUNC5]},
        {"aq",     fields_[idType::AQ]},
        {"wd",     fields_[idType::WD]},
        {"rl",     fields_[idType::RL]},
        {"vm",     fields_[idType::VM]},
        {"rs2",    fields_[idType::RS2]},
        {"rs1",    fields_[idType::RS1]},
        {"func3",  fields_[idType::FUNC3]},
        {"width",  fields_[idType::WIDTH]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
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

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3],
        fields_[idType::FUNC5]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_AMO:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_AMO &)
{
    Form_AMO::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"B"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("imm7", 25, 7),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("imm5", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"imm7",   fields_[idType::IMM7]},
        {"rs2",    fields_[idType::RS2]},
        {"rs1",    fields_[idType::RS1]},
        {"func3",  fields_[idType::FUNC3]},
        {"imm5",   fields_[idType::IMM5]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"imm7",   idType::IMM7},
        {"rs2",    idType::RS2},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"imm5",   idType::IMM5},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_B:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_B &)
{
    Form_B::print(os);
    return os;
}

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
        IMM2,
        RD,
        OPCODE,
        __N
    };

private:
    static inline const std::string name_ {"C0"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func3", 13, 3),
        Field("imm3", 10, 3),
        Field("rs1", 7, 3),
        Field("imm2", 5, 2),
        Field("rd", 2, 3),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func3",  fields_[idType::FUNC3]},
        {"imm3",   fields_[idType::IMM3]},
        {"rs1",    fields_[idType::RS1]},
        {"imm2",   fields_[idType::IMM2]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func3",  idType::FUNC3},
        {"imm3",   idType::IMM3},
        {"rs1",    idType::RS1},
        {"imm2",   idType::IMM2},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_C0:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_C0 &)
{
    Form_C0::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"C1"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func3", 13, 3),
        Field("func1", 12, 1),
        Field("func2", 10, 2),
        Field("rs1", 7, 3), // RD and RS1 are aliases for the same field
        Field("rd", 7, 3), // RD and RS1 are aliases for the same field
        Field("func2b", 5, 2),
        Field("rs2", 2, 3),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func3",  fields_[idType::FUNC3]},
        {"func1",  fields_[idType::FUNC1]},
        {"func2",  fields_[idType::FUNC2]},
        {"rs1",    fields_[idType::RS1]},
        {"rd",     fields_[idType::RD]},
        {"func2b", fields_[idType::FUNC2B]},
        {"rs2",    fields_[idType::RS2]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func3",  idType::FUNC3},
        {"func1",  idType::FUNC1},
        {"func2",  idType::FUNC2},
        {"rs1",    idType::RS1},
        {"rd",     idType::RD},
        {"func2b", idType::FUNC2B},
        {"rs2",    idType::RS2},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3],
        fields_[idType::FUNC2],
        fields_[idType::FUNC2B],
        fields_[idType::FUNC1],
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_C1:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_C1 &)
{
    Form_C1::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"C2"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func3", 13, 3),
        Field("func1", 12, 1),
        Field("shamt1", 12, 1), // Alias for FUNC1
        Field("rd", 7, 5),
        Field("rs1", 7, 5),     // Alias for RD
        Field("rs", 2, 5),
        Field("rs2", 2, 5),     // Alias for RS
        Field("shamt5", 2, 5),  // Alias for RS
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func3",  fields_[idType::FUNC3]},
        {"func1",  fields_[idType::FUNC1]},
        {"shamt1", fields_[idType::SHAMT1]},
        {"rd",     fields_[idType::RD]},
        {"rs1",    fields_[idType::RS1]},
        {"rs",     fields_[idType::RS]},
        {"rs2",    fields_[idType::RS2]},
        {"shamt5", fields_[idType::SHAMT5]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func3",  idType::FUNC3},
        {"func1",  idType::FUNC1},
        {"shamt1", idType::SHAMT1},
        {"rd",     idType::RD},
        {"rs1",    idType::RS1},
        {"rs",     idType::RS},
        {"rs2",    idType::RS2},
        {"shamt5", idType::SHAMT5},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3],
        fields_[idType::FUNC1]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_C2:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_C2 &)
{
    Form_C2::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"C2"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func3", 13, 3),
        Field("imm", 7, 6),
        Field("rs2", 2, 5),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func3",  fields_[idType::FUNC3]},
        {"imm",    fields_[idType::IMM]},
        {"rs2",    fields_[idType::RS2]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func3",  idType::FUNC3},
        {"imm",    idType::IMM},
        {"rs2",    idType::RS2},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_C2:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_C2_sp_store &)
{
    Form_C2_sp_store::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"CA"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func6", 10, 6),
        Field("rs1", 7, 3),
        Field("rd", 7, 3),
        Field("func2", 5, 2),
        Field("rs2", 2, 3),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func6",  fields_[idType::FUNC6]},
        {"rs1",    fields_[idType::RS1]},
        {"rd",     fields_[idType::RD]},
        {"func2",  fields_[idType::FUNC2]},
        {"rs2",    fields_[idType::RS2]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func6",  idType::FUNC6},
        {"rs1",    idType::RS1},
        {"rd",     idType::RD},
        {"func2",  idType::FUNC2},
        {"rs2",    idType::RS2},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC6],
        fields_[idType::FUNC2]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_CA:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_CA &)
{
    Form_CA::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"CB"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func3", 13, 3),
        Field("imm3", 10, 3),
        Field("rs1", 7, 3),
        Field("imm5", 2, 5),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func3",  fields_[idType::FUNC3]},
        {"imm3",   fields_[idType::IMM3]},
        {"rs1",    fields_[idType::RS1]},
        {"imm5",   fields_[idType::IMM5]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func3",  idType::FUNC3},
        {"imm3",   idType::IMM3},
        {"rs1",    idType::RS1},
        {"imm5",   idType::IMM5},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_CB:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_CB &)
{
    Form_CB::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"CI"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func3", 13, 3),
        Field("imm1", 12, 1),
        Field("rs1", 7, 5),
        Field("rd", 7, 5),
        Field("imm5", 2, 5),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func3",  fields_[idType::FUNC3]},
        {"imm1",   fields_[idType::IMM1]},
        {"rs1",    fields_[idType::RS1]},
        {"rd",     fields_[idType::RD]},
        {"imm5",   fields_[idType::IMM5]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func3",  idType::FUNC3},
        {"imm1",   idType::IMM1},
        {"rs1",    idType::RS1},
        {"rd",     idType::RD},
        {"imm5",   idType::IMM5},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_CI:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_CI &)
{
    Form_CI::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"CI_rD_only"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func3", 13, 3),
        Field("imm1", 12, 1),
        Field("rd", 7, 5),
        Field("imm5", 2, 5),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func3",  fields_[idType::FUNC3]},
        {"imm1",   fields_[idType::IMM1]},
        {"rd",     fields_[idType::RD]},
        {"imm5",   fields_[idType::IMM5]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func3",  idType::FUNC3},
        {"imm1",   idType::IMM1},
        {"rd",     idType::RD},
        {"imm5",   idType::IMM5},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_CI_rD_only:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_CI_rD_only &)
{
    Form_CI_rD_only::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"CIW"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func3", 13, 3),
        Field("imm8", 5, 8),
        Field("rd", 2, 3),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func3",  fields_[idType::FUNC3]},
        {"imm8",   fields_[idType::IMM8]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func3",  idType::FUNC3},
        {"imm8",   idType::IMM8},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_CIW:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_CIW &)
{
    Form_CIW::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"CIX"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func3", 13, 3),
        Field("shamt1", 12, 1),
        Field("func2", 10, 2),
        Field("rs1", 7, 3),
        Field("rd", 7, 3),
        Field("shamt5", 2, 5),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func3",  fields_[idType::FUNC3]},
        {"shamt1", fields_[idType::SHAMT1]},
        {"func2",  fields_[idType::FUNC2]},
        {"rs1",    fields_[idType::RS1]},
        {"rd",     fields_[idType::RD]},
        {"shamt5", fields_[idType::SHAMT5]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func3",  idType::FUNC3},
        {"shamt1", idType::SHAMT1},
        {"func2",  idType::FUNC2},
        {"rs1",    idType::RS1},
        {"rd",     idType::RD},
        {"shamt5", idType::SHAMT5},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3],
        fields_[idType::FUNC2]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_CIX:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_CIX &)
{
    Form_CIX::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"CJ"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func3", 13, 3),
        Field("imm11", 2, 11),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func3",  fields_[idType::FUNC3]},
        {"imm11",  fields_[idType::IMM11]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func3",  idType::FUNC3},
        {"imm11",  idType::IMM11},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_CJ:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_CJ &)
{
    Form_CJ::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"CJR"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func4", 12, 4),
        Field("rs1", 7, 5),
        Field("rs2", 2, 5),
        Field("opcode", 0, 2)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func4",  fields_[idType::FUNC4]},
        {"rs1",    fields_[idType::RS1]},
        {"rs2",    fields_[idType::RS2]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func4",  idType::FUNC4},
        {"rs1",    idType::RS1},
        {"rs2",    idType::RS2},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC4]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_CJR:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_CJR &)
{
    Form_CJR::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"CSR"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("csr", 20, 12),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"csr",    fields_[idType::CSR]},
        {"rs1",    fields_[idType::RS1]},
        {"func3",  fields_[idType::FUNC3]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"csr",    idType::CSR},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_CSR:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};


inline std::ostream &operator<<(std::ostream &os, const Form_CSR &)
{
    Form_CSR::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"CSRI"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("csr", 20, 12),
        Field("uimm", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"csr",    fields_[idType::CSR]},
        {"uimm",   fields_[idType::UIMM]},
        {"func3",  fields_[idType::FUNC3]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"csr",    idType::CSR},
        {"uimm",   idType::UIMM},
        {"func3",  idType::FUNC3},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_CSRI:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};


inline std::ostream &operator<<(std::ostream &os, const Form_CSRI &)
{
    Form_CSRI::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"FENCE"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("fm", 28, 4),
        Field("pred", 24, 4),
        Field("succ", 20, 4),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"fm",     fields_[idType::FM]},
        {"pred",   fields_[idType::PRED]},
        {"succ",   fields_[idType::SUCC]},
        {"rs1",    fields_[idType::RS1]},
        {"func3",  fields_[idType::FUNC3]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"fm",     idType::FM},
        {"pred",   idType::PRED},
        {"succ",   idType::SUCC},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_FENCE:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};


inline std::ostream &operator<<(std::ostream &os, const Form_FENCE &)
{
    Form_FENCE::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"I"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("imm", 20, 12),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"imm",    fields_[idType::IMM]},
        {"rs1",    fields_[idType::RS1]},
        {"func3",  fields_[idType::FUNC3]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"imm",    idType::IMM},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_I:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};


inline std::ostream &operator<<(std::ostream &os, const Form_I &)
{
    Form_I::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"ISH"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func6", 26, 6),
        Field("shamt", 20, 6),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func6",  fields_[idType::FUNC6]},
        {"shamt",  fields_[idType::SHAMT]},
        {"rs1",    fields_[idType::RS1]},
        {"func3",  fields_[idType::FUNC3]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func6",  idType::FUNC6},
        {"shamt",  idType::SHAMT},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3],
        fields_[idType::FUNC6]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_ISH:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};


inline std::ostream &operator<<(std::ostream &os, const Form_ISH &)
{
    Form_ISH::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"ISHW"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func7", 25, 7),
        Field("shamtw", 20, 5),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func7",  fields_[idType::FUNC7]},
        {"shamtw", fields_[idType::SHAMTW]},
        {"rs1",    fields_[idType::RS1]},
        {"func3",  fields_[idType::FUNC3]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func7",  idType::FUNC7},
        {"shamtw", idType::SHAMTW},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3],
        fields_[idType::FUNC7]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_ISHW:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};


inline std::ostream &operator<<(std::ostream &os, const Form_ISHW &)
{
    Form_ISHW::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"J"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("imm20", 12, 20),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"imm20",  fields_[idType::IMM20]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"imm20",  idType::IMM20},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_J:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};


inline std::ostream &operator<<(std::ostream &os, const Form_J &)
{
    Form_J::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"R"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func7", 25, 7),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func7",  fields_[idType::FUNC7]},
        {"rs2",    fields_[idType::RS2]},
        {"rs1",    fields_[idType::RS1]},
        {"func3",  fields_[idType::FUNC3]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func7",  idType::FUNC7},
        {"rs2",    idType::RS2},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3],
        fields_[idType::FUNC7]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_R:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_R &)
{
    Form_R::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"Rfloat"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func7", 25, 7),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("rm", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func7",  fields_[idType::FUNC7]},
        {"rs2",    fields_[idType::RS2]},
        {"rs1",    fields_[idType::RS1]},
        {"rm",     fields_[idType::RM]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func7",  idType::FUNC7},
        {"rs2",    idType::RS2},
        {"rs1",    idType::RS1},
        {"rm",     idType::RM},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC7]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_Rfloat:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_Rfloat &)
{
    Form_Rfloat::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"R4"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("rs3", 27, 5),
        Field("func2", 25, 2),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("rm", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"rs3",    fields_[idType::RS3]},
        {"func2",  fields_[idType::FUNC2]},
        {"rs2",    fields_[idType::RS2]},
        {"rs1",    fields_[idType::RS1]},
        {"rm",     fields_[idType::RM]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"rs3",    idType::RS3},
        {"func2",  idType::FUNC2},
        {"rs2",    idType::RS2},
        {"rs1",    idType::RS1},
        {"rm",     idType::RM},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC2]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_R4:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_R4 &)
{
    Form_R4::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"S"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("imm7", 25, 7),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("imm5", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"imm7",   fields_[idType::IMM7]},
        {"rs2",    fields_[idType::RS2]},
        {"rs1",    fields_[idType::RS1]},
        {"func3",  fields_[idType::FUNC3]},
        {"imm5",   fields_[idType::IMM5]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"imm7",   idType::IMM7},
        {"rs2",    idType::RS2},
        {"rs1",    idType::RS1},
        {"func3",  idType::FUNC3},
        {"imm5",   idType::IMM5},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_S:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_S &)
{
    Form_S::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"U"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("imm20", 12, 20),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"imm20",  fields_[idType::IMM20]},
        {"rd",     fields_[idType::RD]},
        {"opcode", fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"imm20",  idType::IMM20},
        {"rd",     idType::RD},
        {"opcode", idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_U:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Form_U &)
{
    Form_U::print(os);
    return os;
}

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
        FUNC4,
        VM,
        RS2,
        RS1,
        SIMM5,      // Alias for RS1 in vector-immediate (signed) instructions
        FUNC3,
        RD,
        OPCODE,
        __N
    };

private:
    static inline const std::string name_ {"V"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func1a", 31, 1),
        Field("func1b", 30, 1),
        Field("func4", 26, 4),
        Field("vm", 25, 1),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("simm5", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func1a",  fields_[idType::FUNC1A]},
        {"func1b",  fields_[idType::FUNC1B]},
        {"func4",   fields_[idType::FUNC4]},
        {"vm",      fields_[idType::VM]},
        {"rs2",     fields_[idType::RS2]},
        {"rs1",     fields_[idType::RS1]},
        {"simm5",   fields_[idType::SIMM5]},
        {"func3",   fields_[idType::FUNC3]},
        {"rd",      fields_[idType::RD]},
        {"opcode",  fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func1a",  idType::FUNC1A},
        {"func1b",  idType::FUNC1B},
        {"func4",   idType::FUNC4},
        {"vm",      idType::VM},
        {"rs2",     idType::RS2},
        {"rs1",     idType::RS1},
        {"simm5",   idType::SIMM5},
        {"func3",   idType::FUNC3},
        {"rd",      idType::RD},
        {"opcode",  idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3],
        fields_[idType::FUNC1A],
        fields_[idType::FUNC1B],
        fields_[idType::FUNC4]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_V:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};


inline std::ostream &operator<<(std::ostream &os, const Form_V &)
{
    Form_V::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"VF_mem"};

    static inline const std::array<const Field, idType::__N> fields_ {
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

    static inline const std::map<std::string, const Field &> fmap_ {
        {"nf",      fields_[idType::NF]},
        {"mewop",   fields_[idType::MEWOP]},
        {"vm",      fields_[idType::VM]},
        {"rs2",     fields_[idType::RS2]},
        {"rs1",     fields_[idType::RS1]},
        {"width",   fields_[idType::WIDTH]},
        {"rd",      fields_[idType::RD]},
        {"rs3",     fields_[idType::RS3]},
        {"opcode",  fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
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

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::WIDTH],
        fields_[idType::MEWOP]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_VF_mem:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};


inline std::ostream &operator<<(std::ostream &os, const Form_VF_mem &)
{
    Form_VF_mem::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"V_vsetvli"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func1", 31, 1),
        Field("imm11", 20, 11),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func1",   fields_[idType::FUNC1]},
        {"imm11",   fields_[idType::IMM11]},
        {"rs1",     fields_[idType::RS1]},
        {"func3",   fields_[idType::FUNC3]},
        {"rd",      fields_[idType::RD]},
        {"opcode",  fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func1",   idType::FUNC1},
        {"imm11",   idType::IMM11},
        {"rs1",     idType::RS1},
        {"func3",   idType::FUNC3},
        {"rd",      idType::RD},
        {"opcode",  idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3],
        fields_[idType::FUNC1]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_V_vsetvli:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};


inline std::ostream &operator<<(std::ostream &os, const Form_V_vsetvli &)
{
    Form_V_vsetvli::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"V_vsetivli"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func2", 30, 2),
        Field("imm10", 20, 10),
        Field("avl", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func2",   fields_[idType::FUNC2]},
        {"imm10",   fields_[idType::IMM10]},
        {"avl",     fields_[idType::AVL]},
        {"func3",   fields_[idType::FUNC3]},
        {"rd",      fields_[idType::RD]},
        {"opcode",  fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func2",   idType::FUNC2},
        {"imm10",   idType::IMM10},
        {"avl",     idType::AVL},
        {"func3",   idType::FUNC3},
        {"rd",      idType::RD},
        {"opcode",  idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3],
        fields_[idType::FUNC2]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return true;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_V_vsetivli:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};


inline std::ostream &operator<<(std::ostream &os, const Form_V_vsetivli &)
{
    Form_V_vsetivli::print(os);
    return os;
}

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

private:
    static inline const std::string name_ {"V_vsetvl"};

    static inline const std::array<const Field, idType::__N> fields_ {
        Field("func7", 25, 7),
        Field("rs2", 20, 5),
        Field("rs1", 15, 5),
        Field("func3", 12, 3),
        Field("rd", 7, 5),
        Field("opcode", 0, 7)
    };

    static inline const std::map<std::string, const Field &> fmap_ {
        {"func7",   fields_[idType::FUNC7]},
        {"rs2",     fields_[idType::RS2]},
        {"rs1",     fields_[idType::RS1]},
        {"func3",   fields_[idType::FUNC3]},
        {"rd",      fields_[idType::RD]},
        {"opcode",  fields_[idType::OPCODE]}
    };

    static inline const std::map<std::string, idType> imap_ {
        {"func7",   idType::FUNC7},
        {"rs2",     idType::RS2},
        {"rs1",     idType::RS1},
        {"func3",   idType::FUNC3},
        {"rd",      idType::RD},
        {"opcode",  idType::OPCODE}
    };

    static inline FormWrapperIF::OpcodeFieldsType opcode_fields_ {
        fields_[idType::OPCODE],
        fields_[idType::FUNC3],
        fields_[idType::FUNC7]
    };

public:
    static inline const std::string &getName()
    {
        return name_;
    }

    static inline const Field &getField(const idType fid)
    {
        return fields_[fid];
    }

    static inline const Field &getField(const std::string &fname)
    {
        const auto itr = fmap_.find(fname);
        if (itr == fmap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline uint32_t getFieldIndex(const std::string &fname)
    {
        const auto itr = imap_.find(fname);
        if (itr == imap_.end()) {
            throw BuildErrorUnknownFormField(name_, fname);
        }
        return itr->second;
    }

    static inline FormWrapperIF::OpcodeFieldsType &getOpcodeFields()
    {
        return opcode_fields_;
    }

    static inline bool hasImmediate()
    {
        return false;
    }

    static inline void print(std::ostream &os)
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Fields of Form_V_vsetvl:" << std::endl;
        for (const auto &f : fields_) {
            os << "\t" << f << std::endl;
        }
        os.flags(os_state);
    }
};


inline std::ostream &operator<<(std::ostream &os, const Form_V_vsetvl &)
{
    Form_V_vsetvl::print(os);
    return os;
}


} // namespace mavis
