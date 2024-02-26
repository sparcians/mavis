#pragma once

// This file contains a few common Forms used in Mavis custom
// factories, etc.

#include "FormIF.h"

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

        static inline const char * name {"I"};

        static inline const std::array<const Field, idType::__N> fields {
            Field("imm", 20, 12),
            Field("rs1", 15, 5),
            Field("func3", 12, 3),
            Field("rd", 7, 5),
            Field("opcode", 0, 7)
        };

        static inline const std::map<std::string, const Field &> fmap {
            {"imm",    fields[idType::IMM]},
            {"rs1",    fields[idType::RS1]},
            {"func3",  fields[idType::FUNC3]},
            {"rd",     fields[idType::RD]},
            {"opcode", fields[idType::OPCODE]}
        };

        static inline const std::map<std::string, idType> imap {
            {"imm",    idType::IMM},
            {"rs1",    idType::RS1},
            {"func3",  idType::FUNC3},
            {"rd",     idType::RD},
            {"opcode", idType::OPCODE}
        };

        static inline FieldsType opcode_fields {
            fields[idType::OPCODE],
            fields[idType::FUNC3]
        };

        static inline ImmediateType getImmediateType()
        {
            return ImmediateType::SIGNED;
        }

        static inline void print(std::ostream &os)
        {
            std::ios_base::fmtflags os_state(os.flags());
            os << "Fields of Form_I:" << std::endl;
            for (const auto &f : fields) {
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

        static inline const char * name {"R"};

        static inline const std::array<const Field, idType::__N> fields {
            Field("func7", 25, 7),
            Field("rs2", 20, 5),
            Field("rs1", 15, 5),
            Field("func3", 12, 3),
            Field("rd", 7, 5),
            Field("opcode", 0, 7)
        };

        static inline const std::map<std::string, const Field &> fmap {
            {"func7",  fields[idType::FUNC7]},
            {"rs2",    fields[idType::RS2]},
            {"rs1",    fields[idType::RS1]},
            {"func3",  fields[idType::FUNC3]},
            {"rd",     fields[idType::RD]},
            {"opcode", fields[idType::OPCODE]}
        };

        static inline const std::map<std::string, idType> imap {
            {"func7",  idType::FUNC7},
            {"rs2",    idType::RS2},
            {"rs1",    idType::RS1},
            {"func3",  idType::FUNC3},
            {"rd",     idType::RD},
            {"opcode", idType::OPCODE}
        };

        static inline FieldsType opcode_fields {
            fields[idType::OPCODE],
            fields[idType::FUNC3],
            fields[idType::FUNC7]
        };

        static inline ImmediateType getImmediateType()
        {
            return ImmediateType::NONE;
        }

        static inline void print(std::ostream &os)
        {
            std::ios_base::fmtflags os_state(os.flags());
            os << "Fields of Form_R:" << std::endl;
            for (const auto &f : fields) {
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
