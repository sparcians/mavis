#pragma once

#include <string>
#include <map>
#include <memory>
#include <array>
#include <cinttypes>
#include <regex>
#include <boost/json.hpp>

#include "DecoderExceptions.h"
#include "Form.h"
#include "Tag.hpp"
#include "MatchSet.hpp"

namespace mavis
{

    class InstMetaData
    {
      private:
        using json = boost::json::object;
        typedef typename std::vector<std::string> FieldNameListType;
        typedef typename std::vector<std::string> ISAExtListType;

      public:
        // TODO: Deprecate this...
        enum class ISA : uint32_t
        {
            RV32I,
            RV64I
        };

        enum class InstructionTypes : uint64_t
        {
            INT = 1ull << 0u,
            FLOAT = 1ull << 1u,
            ARITH = 1ull << 2u,
            MULTIPLY = 1ull << 3u,
            DIVIDE = 1ull << 4u,
            BRANCH = 1ull << 5u,
            PC = 1ull << 6u,
            CONDITIONAL = 1ull << 7u,
            JAL = 1ull << 8u,
            JALR = 1ull << 9u,
            LOAD = 1ull << 10u,
            STORE = 1ull << 11u,
            MAC = 1ull << 12u,
            SQRT = 1ull << 13u,
            CONVERT = 1ull << 14u,
            COMPARE = 1ull << 15u,
            MOVE = 1ull << 16u,
            CLASSIFY = 1ull << 17u,
            VECTOR = 1ull << 18u,
            MASKABLE = 1ull << 19u,
            UNIT_STRIDE = 1ull << 20u,
            STRIDE = 1ull << 21u,
            ORDERED_INDEXED = 1ull << 22u,
            UNORDERED_INDEXED = 1ull << 23u,
            SEGMENT = 1ull << 24u,
            FAULTFIRST = 1ull << 25u,
            WHOLE = 1ull << 26u,
            MASK = 1ull << 27u,
            WIDENING = 1ull << 28u,
            HYPERVISOR = 1ull << 29u,
            CRYPTO = 1ull << 30u,
            PREFETCH = 1ull << 56u,
            NTL = 1ull << 57u,
            HINT = 1ull << 58u,
            CACHE = 1ull << 59u,
            ATOMIC = 1ull << 60u,
            FENCE = 1ull << 61u,
            SYSTEM = 1ull << 62u,
            CSR = 1ull << 63u
        };

        enum class ISAExtensionIndex : uint64_t
        {
            A = 0, // Atomic
            B,     // Bitmanip
            C,     // Compressed
            D,     // Double precision
            F,     // Float (single)
            G,     // "General"
            H,     // Hypervisor
            I,     // Integer
            M,     // Multiply
            Q,     // Quad
            V,     // Vector
            __N    // LAST: number of enum values
        };

        enum class ISAExtension : uint64_t
        {
            A = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(
                    ISAExtensionIndex::A),
            B = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(
                    ISAExtensionIndex::B),
            C = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(
                    ISAExtensionIndex::C),
            D = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(
                    ISAExtensionIndex::D),
            F = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(
                    ISAExtensionIndex::F),
            G = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(
                    ISAExtensionIndex::G),
            H = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(
                    ISAExtensionIndex::H),
            I = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(
                    ISAExtensionIndex::I),
            M = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(
                    ISAExtensionIndex::M),
            Q = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(
                    ISAExtensionIndex::Q),
            V = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(ISAExtensionIndex::V)
        };

        enum class OperandTypes : uint32_t
        {
            WORD = 0,
            __FIRST = WORD,
            LONG,
            HALF,
            SINGLE,
            DOUBLE,
            QUAD,
            VECTOR,
            NONE,
            __N = NONE
        };

        // TODO: Should RS alias to RS1?
        enum class OperandFieldID : uint32_t
        {
            RS1 = 0, // NOTE: RS1 thru RS_MAX need to stay contiguous for ExtractorDirectInfo
            RS2,
            RS3,
            RS4,
            RS_MAX, // Max "architectural" RS

            FUSED_SD_0, // for fusion: store data operand 0
            FUSED_SD_1, // for fusion: store data operand 1
            TEMP_RS1,   // for temporal RS1 operand
            TEMP_RS2,   // for temporal RS2 operand
            TEMP_RS3,   // for temporal RS3 operand

            // Special source operands for cm.push instructions
            PUSH_RS1,
            PUSH_RS2,
            PUSH_RS3,
            PUSH_RS4,
            PUSH_RS5,
            PUSH_RS6,
            PUSH_RS7,
            PUSH_RS8,
            PUSH_RS9,
            PUSH_RS10,
            PUSH_RS11,
            PUSH_RS12,
            PUSH_RS13,

            RD, // NOTE: RD/RD1 thru RD_MAX need to stay contiguous for ExtractorDirectInfo
            RD1 = RD,
            RD2,
            RD_MAX, // Max "architectural" RD

            FUSED_RD_0, // for fusion: dest reg 0
            FUSED_RD_1, // for fusion: dest reg 1
            TEMP_RD,    // for temporal RD operand
            TEMP_RD2,   // for temporal RD2 operand

            // Special dest operands for cm.pop instructions
            POP_RD1,
            POP_RD2,
            POP_RD3,
            POP_RD4,
            POP_RD5,
            POP_RD6,
            POP_RD7,
            POP_RD8,
            POP_RD9,
            POP_RD10,
            POP_RD11,
            POP_RD12,
            POP_RD13,

            NONE,
            __N = NONE
        };

        typedef std::shared_ptr<InstMetaData> PtrType;

        // Make SpecialField a bitmap
        enum class SpecialField : uint32_t
        {
            AQ,  // AQ "acquire" bit in lr.w, sc.w, and atomics
            AVL,  // AVL "immediate" field in vsetivli
            CSR,  // CSR field in csr* instructions
            FM,  // FENCE mode bits
            NF,  // NF field in vector memory instructions
            PRED,  // FENCE predecessor bits
            RL,  // RL "release" bit in lr.w, sc.w, and atomics
            RM,  // RM "rounding mode" bit in FP instructions
            SUCC,  // FENCE successor bits
            VM,   // VM bit in vector insts
            WD,   // WD in vector atomic insts
            HINT, // HINT in prefetch operations
            STACK_ADJ, // Stack adjustment for zcmp instructions
            N_SPECIAL_FIELDS
        };
        // Map of special field to value
        using SpecialFieldsMap = std::map<SpecialField, uint32_t>;

        static const inline std::map<InstMetaData::SpecialField, const std::string> sf_to_string_map{
            {InstMetaData::SpecialField::AQ       , "aq"        },
            {InstMetaData::SpecialField::AVL      , "avl"       },
            {InstMetaData::SpecialField::CSR      , "csr"       },
            {InstMetaData::SpecialField::FM       , "fm"        },
            {InstMetaData::SpecialField::NF       , "nf"        },
            {InstMetaData::SpecialField::PRED     , "pred"      },
            {InstMetaData::SpecialField::RL       , "rl"        },
            {InstMetaData::SpecialField::RM       , "rm"        },
            {InstMetaData::SpecialField::SUCC     , "succ"      },
            {InstMetaData::SpecialField::VM       , "vm"        },
            {InstMetaData::SpecialField::WD       , "wd"        },
            {InstMetaData::SpecialField::HINT     , "hint"      },
            {InstMetaData::SpecialField::STACK_ADJ, "stack_adj" }
        };
        static_assert(static_cast<std::underlying_type_t<InstMetaData::SpecialField>>
                      (InstMetaData::SpecialField::N_SPECIAL_FIELDS) == 13,
                      "Modified SpecialField?  Need to update the map too");

      private:
        static const std::regex isa_ext_pattern_;

        static constexpr std::underlying_type_t<OperandFieldID> source_fields_ =
            //(1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::RS)) |
            (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::RS1))
            | (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::RS2))
            | (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::RS3))
            | (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(
                   OperandFieldID::FUSED_SD_0))
            | (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(
                   OperandFieldID::FUSED_SD_1));

        static constexpr std::underlying_type_t<OperandFieldID> dest_fields_ =
            (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::RD))
            | (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(
                   OperandFieldID::FUSED_RD_0))
            | (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(
                   OperandFieldID::FUSED_RD_1));

      public:
        /**
         * Construct from JSON information pertaining to extraction
         */
        InstMetaData(const json & inst, bool compressed = false,
                     const MatchSet<Tag> & tags = MatchSet<Tag>());

        /**
         * \brief Copy constructor
         * \param other
         */
        InstMetaData(const InstMetaData & other) = default;

        InstMetaData::PtrType clone() const { return std::make_shared<InstMetaData>(*this); }

        /**
         * Construct according to ISA (for custom instruction factories)
         * @param iset
         */
        // TODO: Deprecate this
        InstMetaData(ISA iset)
        {
            oper_type_.fill(InstMetaData::OperandTypes::NONE);

            switch (iset)
            {
                case ISA::RV32I:
                    // field_set_[static_cast<uint32_t>(OperandTypes::WORD)] = -1u;
                    setAllOperandsType_(OperandTypes::WORD);
                    inst_types_ = static_cast<std::underlying_type_t<InstructionTypes>>(
                        InstructionTypes::INT);
                    break;
                case ISA::RV64I:
                    // field_set_[static_cast<uint32_t>(OperandTypes::LONG)] = -1u;
                    setAllOperandsType_(OperandTypes::LONG);
                    inst_types_ = static_cast<std::underlying_type_t<InstructionTypes>>(
                        InstructionTypes::INT);
                    break;
            }
        }

        /**
         * \brief Merge information from another InstMetaData object
         * For now, we only merge in the instruction type and compression flag
         *
         * \param other
         */
        void merge(const InstMetaData::PtrType & other)
        {
            compressed_ = mavis::utils::notNull(other)->compressed_;
            inst_types_ |= other->inst_types_;
        }

        // This method is called Overlay::setBaseMetaData to handle JSON stanza items that
        // need to be merged from the overlay into the base
        void parseOverrides(const json & inst)
        {
            // Merge type information from the overlay instruction and the base
            parseTypeStanza_(inst);

            // Merge data size information as well
            parseDataSizeStanza_(inst);

            // Merge tags from the overlay instruction and the base
            if (const auto it = inst.find("tags"); it != inst.end())
            {
                tags_.merge(boost::json::value_to<std::vector<std::string>>(it->value()));
            }
        }

        static const std::string &  getInstructionTypeName(const InstructionTypes & inst_type);

        template <typename... ArgTypes> void setInstType(const ArgTypes &&... args)
        {
            const std::vector<InstructionTypes> itypes{args...};
            for (const auto & i : itypes)
            {
                inst_types_ |= static_cast<std::underlying_type_t<InstructionTypes>>(i);
            }
        }

        std::underlying_type_t<InstructionTypes> getInstType() const { return inst_types_; }

        bool isInstType(const InstructionTypes itype) const
        {
            return (static_cast<std::underlying_type_t<InstructionTypes>>(itype) & inst_types_)
                   == static_cast<std::underlying_type_t<InstructionTypes>>(itype);
        }

        template <typename... ArgTypes> bool isInstTypeAnyOf(ArgTypes &&... args) const
        {
            const std::vector<InstructionTypes> itypes{args...};
            for (const auto & i : itypes)
            {
                if (isInstType(i))
                {
                    return true;
                }
            }
            return false;
        }

        template <typename... ArgTypes> bool isInstTypeAllOf(ArgTypes &&... args) const
        {
            const std::vector<InstructionTypes> itypes{args...};
            for (const auto & i : itypes)
            {
                if (!isInstType(i))
                {
                    return false;
                }
            }
            return true;
        }

        template <typename... ArgTypes>
        bool isInstTypeSameAs(std::underlying_type_t<InstructionTypes> other_types,
                              ArgTypes &&... args) const
        {
            const std::vector<InstructionTypes> itypes{args...};
            if (itypes.empty())
            {
                return (inst_types_ == other_types);
            }

            for (const auto & i : itypes)
            {
                if ((static_cast<std::underlying_type_t<InstructionTypes>>(i) & inst_types_)
                    != (static_cast<std::underlying_type_t<InstructionTypes>>(i) & other_types))
                {
                    return false;
                }
            }
            return true;
        }

        std::underlying_type_t<ISAExtension> getISA() const { return isa_ext_; }

        bool isISA(const ISAExtension isa) const
        {
            return (isa_ext_ & static_cast<std::underlying_type_t<ISAExtension>>(isa))
                   == static_cast<std::underlying_type_t<ISAExtension>>(isa);
        }

        template <typename... ArgTypes> bool isISAAnyOf(ArgTypes &&... args) const
        {
            return (ISAtoBits_(std::forward<ArgTypes>(args)...) & isa_ext_) != 0;
        }

        template <typename... ArgTypes> bool isISAAllOf(ArgTypes &&... args) const
        {
            const std::underlying_type_t<ISAExtension> isa_bits =
                ISAtoBits_(std::forward<ArgTypes>(args)...);
            return (isa_bits & isa_ext_) == isa_bits;
        }

        template <typename... ArgTypes>
        bool isISASameAs(std::underlying_type_t<ISAExtension> other_isa, ArgTypes &&... args) const
        {
            const std::underlying_type_t<ISAExtension> check_bits =
                ISAtoBits_(std::forward<ArgTypes>(args)...);
            if (check_bits == 0)
            {
                return (isa_ext_ == other_isa);
            }
            return (isa_ext_ & other_isa & check_bits) == check_bits;
        }

        uint32_t getISAWidth(const ISAExtensionIndex isa) const
        {
            return isa_width_[static_cast<std::underlying_type_t<ISAExtensionIndex>>(isa)];
        }

        void setISAWidth(const ISAExtensionIndex isa, uint32_t width)
        {
            if ((width == 0) || ((width & (width - 1)) != 0)) [[unlikely]]
            {
                throw std::invalid_argument("ISA width must be a non-zero power of 2");
            }

            isa_width_[static_cast<std::underlying_type_t<ISAExtensionIndex>>(isa)] |= width;
        }

        bool isISAWidth(const ISAExtensionIndex isa, uint32_t width) const
        {
            const ISAExtension isa_bit_enum = static_cast<ISAExtension>(
                1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(isa));
            return isISA(isa_bit_enum) && ((width & getISAWidth(isa)) == width);
        }

        bool isNoneOperandType(const OperandTypes kind) const
        {
            return field_set_[static_cast<std::underlying_type_t<OperandTypes>>(kind)] == 0;
        }

        bool isAllOperandType(const OperandTypes kind) const
        {
            return field_set_[static_cast<std::underlying_type_t<OperandTypes>>(kind)] == -1u;
        }

        bool isOperandType(const OperandFieldID fid, const OperandTypes kind) const
        {
            return ((1u << static_cast<std::underlying_type_t<OperandFieldID>>(fid))
                    & field_set_[static_cast<std::underlying_type_t<OperandTypes>>(kind)])
                   != 0;
        }

        bool isAnySourceOperandType(const OperandTypes kind) const
        {
            return (source_fields_
                    & field_set_[static_cast<std::underlying_type_t<OperandTypes>>(kind)])
                   != 0;
        }

        bool isAnyDestOperandType(const OperandTypes kind) const
        {
            return (dest_fields_
                    & field_set_[static_cast<std::underlying_type_t<OperandTypes>>(kind)])
                   != 0;
        }

        OperandTypes getOperandType(const OperandFieldID fid) const
        {
            return oper_type_[static_cast<std::underlying_type_t<OperandFieldID>>(fid)];
        }

        // DirectInfo extractions don't provide an operand field ID, so we pick a default type
        OperandTypes getDefaultSourceType() const
        {
            for (auto i = static_cast<std::underlying_type_t<OperandTypes>>(OperandTypes::__FIRST);
                 i < static_cast<std::underlying_type_t<OperandTypes>>(OperandTypes::__N); ++i)
            {
                if (isAnySourceOperandType(static_cast<OperandTypes>(i)))
                {
                    return static_cast<OperandTypes>(i);
                }
            }
            return OperandTypes::NONE;
        }

        // DirectInfo extractions don't provide an operand field ID, so we pick a default type
        OperandTypes getDefaultDestType() const
        {
            for (auto i = static_cast<std::underlying_type_t<OperandTypes>>(OperandTypes::__FIRST);
                 i < static_cast<std::underlying_type_t<OperandTypes>>(OperandTypes::__N); ++i)
            {
                if (isAnyDestOperandType(static_cast<OperandTypes>(i)))
                {
                    return static_cast<OperandTypes>(i);
                }
            }
            return OperandTypes::NONE;
        }

        void addFixedFields(const std::vector<std::string> & flist)
        {
            for (const auto & field : flist)
            {
                OperandFieldID fid = getFieldID(field);
                if (fid != OperandFieldID::NONE)
                {
                    fixed_fields_[static_cast<std::underlying_type_t<OperandFieldID>>(
                        getFieldID(field))] = true;
                }
            }
        }

        bool isOperandFixed(const OperandFieldID fid) const
        {
            if (fid != OperandFieldID::NONE)
            {
                return fixed_fields_[static_cast<std::underlying_type_t<OperandFieldID>>(fid)];
            }
            else
            {
                // By convention, the NONE (non-existent) field is considered a fixed field
                return true;
            }
        }

        uint32_t getDataSize() const { return data_size_; }

        const MatchSet<Tag> & getTags() const { return tags_; }

        static const std::string & getFieldIDName(OperandFieldID fid);

        static OperandFieldID getFieldID(const std::string & fname);

      private:
        bool compressed_ = false;
        std::underlying_type_t<InstructionTypes> inst_types_ = 0;
        std::underlying_type_t<ISAExtension> isa_ext_ = 0;
        std::array<uint32_t, static_cast<size_t>(ISAExtensionIndex::__N)> isa_width_ = {
            0}; /// Width set (bits) for each of our ISA's
        std::array<uint32_t, static_cast<size_t>(OperandTypes::__N)> field_set_{
            0}; /// Maps operand type to bit set of fields
        std::array<OperandTypes, static_cast<size_t>(OperandFieldID::__N)>
            oper_type_; /// Maps field to operand type
        std::array<bool, static_cast<size_t>(OperandFieldID::__N)> fixed_fields_{
            false}; /// Is operand part of the encoding (fixed field)
        uint32_t data_size_ = 0;
        MatchSet<Tag> tags_;

        void parseTypeStanza_(const json & inst);

        void parseDataSizeStanza_(const json & inst);

        static std::underlying_type_t<OperandFieldID> getFieldIndex_(const std::string & fname);

        static inline uint64_t getFieldIDSet_(const std::vector<std::string> & flist)
        {
            uint64_t fset = 0;
            for (const auto & f : flist)
            {
                fset |= (1u << getFieldIndex_(f));
            }
            return fset;
        }

        inline void setOperandsType_(const FieldNameListType & flist, OperandTypes ot)
        {
            field_set_[static_cast<std::underlying_type_t<OperandTypes>>(ot)] =
                getFieldIDSet_(flist);
            for (const auto & f : flist)
            {
                oper_type_[getFieldIndex_(f)] = ot;
            }
        }

        inline void setAllOperandsType_(OperandTypes ot)
        {
            field_set_[static_cast<std::underlying_type_t<OperandTypes>>(ot)] = -1u;
            for (uint32_t fid = 0;
                 fid < static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::__N);
                 ++fid)
            {
                oper_type_[fid] = ot;
            }
        }

        // TODO: Refactor the InstType queries above to use this (similar to the ISA queries)
        template <typename... ArgTypes>
        static inline std::underlying_type_t<InstructionTypes> InstTypesToBits_(ArgTypes &&... args)
        {
            const std::vector<InstructionTypes> itype_list{args...};
            std::underlying_type_t<InstructionTypes> itype_bits = 0;
            std::for_each(
                itype_list.cbegin(), itype_list.cend(), [&](const auto & i)
                { itype_bits |= static_cast<std::underlying_type_t<InstructionTypes>>(i); });
            return itype_bits;
        }

        template <typename... ArgTypes>
        static inline std::underlying_type_t<ISAExtension> ISAtoBits_(ArgTypes &&... args)
        {
            const std::vector<ISAExtension> isa_list{args...};
            std::underlying_type_t<ISAExtension> isa_bits = 0;
            std::for_each(isa_list.cbegin(), isa_list.cend(), [&](const auto & i)
                          { isa_bits |= static_cast<std::underlying_type_t<ISAExtension>>(i); });
            return isa_bits;
        }
    };

} // namespace mavis
