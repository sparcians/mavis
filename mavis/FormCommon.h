#pragma once

// This file contains a few common Forms used in Mavis custom
// factories, etc.

#include <ostream>
#include <sstream>
#include <string>
#include <array>
#include <map>
#include <cinttypes>

namespace mavis
{
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
        static inline const char * name_ {"I"};

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
        static inline const char *getName()
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

        static inline ImmediateType getImmediateType()
        {
            return ImmediateType::SIGNED;
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
        static inline const char * name_ {"R"};

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
        static inline std::string getName()
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

        static inline ImmediateType getImmediateType()
        {
            return ImmediateType::NONE;
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
}
