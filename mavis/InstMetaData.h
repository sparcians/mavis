#pragma once

#include "json.hpp"
#include "DecoderExceptions.h"
#include "FormIF.h"
#include <memory>
#include <array>
#include <cinttypes>

namespace mavis {

class InstMetaData
{
private:
    using json = nlohmann::json;
    typedef typename std::vector<std::string> FieldNameListType;

public:
    // TODO: flesh this out
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
        STRIDE  = 1ull << 20u,
        INDEXED = 1ull << 21u,
        SEGMENT = 1ull << 22u,
        FAULTFIRST = 1ull << 23u,
        WHOLE = 1ull << 24u,
        MASK = 1ull << 25u,
        CACHE = 1ull << 59u,
        ATOMIC = 1ull << 60u,
        FENCE = 1ull << 61u,
        SYSTEM = 1ull << 62u,
        CSR = 1ull << 63u
    };

    enum class OperandTypes : uint32_t
    {
        WORD = 0,
        __FIRST = WORD,
        LONG,
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
        //RS = 0,
        RS1 = 0,
        RS2,
        RS3,
        FUSED_SD_0,       // for fusion: store data operand 0
        FUSED_SD_1,       // for fusion: store data operand 1
        RD,
        FUSED_RD_0,       // for fusion: dest reg 0
        FUSED_RD_1,       // for fusion: dest reg 1
        NONE,
        __N = NONE
    };

    typedef std::shared_ptr<InstMetaData> PtrType;

private:
    static inline std::map<std::string, InstructionTypes> tmap_ {
        {"int",        InstructionTypes::INT},
        {"float",      InstructionTypes::FLOAT},
        {"arith",      InstructionTypes::ARITH},
        {"mul",        InstructionTypes::MULTIPLY},
        {"div",        InstructionTypes::DIVIDE},
        {"branch",     InstructionTypes::BRANCH},
        {"pc",         InstructionTypes::PC},
        {"cond"  ,     InstructionTypes::CONDITIONAL},
        {"jal",        InstructionTypes::JAL},
        {"jalr",       InstructionTypes::JALR},
        {"load",       InstructionTypes::LOAD},
        {"store",      InstructionTypes::STORE},
        {"mac",        InstructionTypes::MAC},
        {"sqrt",       InstructionTypes::SQRT},
        {"convert",    InstructionTypes::CONVERT},
        {"compare",    InstructionTypes::COMPARE},
        {"move",       InstructionTypes::MOVE},
        {"classify",   InstructionTypes::CLASSIFY},
        {"vector",     InstructionTypes::VECTOR},
        {"maskable",   InstructionTypes::MASKABLE},
        {"stride",     InstructionTypes::STRIDE},
        {"indexed",    InstructionTypes::INDEXED},
        {"segment",    InstructionTypes::SEGMENT},
        {"faultfirst", InstructionTypes::FAULTFIRST},
        {"whole",      InstructionTypes::WHOLE},
        {"mask",       InstructionTypes::MASK},
        {"cache",      InstructionTypes::CACHE},
        {"atomic",     InstructionTypes::ATOMIC},
        {"fence",      InstructionTypes::FENCE},
        {"system",     InstructionTypes::SYSTEM},
        {"csr",        InstructionTypes::CSR}
    };

    static inline const std::map<std::string, OperandFieldID> ofimap_ {
        //{"rs",  OperandFieldID::RS},
        {"rs1", OperandFieldID::RS1},
        {"rs2", OperandFieldID::RS2},
        {"rs3", OperandFieldID::RS3},
        {"fused_sd_0", OperandFieldID::FUSED_SD_0},
        {"fused_sd_1", OperandFieldID::FUSED_SD_1},
        {"rd",  OperandFieldID::RD},
        {"fused_rd_0",  OperandFieldID::FUSED_RD_0},
        {"fused_rd_1",  OperandFieldID::FUSED_RD_1}
    };

    static inline const std::array<std::string, static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::__N)> ofinames_ {
        //"rs",
        "rs1",
        "rs2",
        "rs3",
        "fused_sd_0",
        "fused_sd_1",
        "rd",
        "fused_rd_0",
        "fused_rd_1"
    };

    static constexpr std::underlying_type_t<OperandFieldID> source_fields_ =
        //(1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::RS)) |
        (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::RS1)) |
        (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::RS2)) |
        (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::RS3)) |
        (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::FUSED_SD_0)) |
        (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::FUSED_SD_1));

    static constexpr std::underlying_type_t<OperandFieldID> dest_fields_ =
        (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::RD)) |
        (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::FUSED_RD_0)) |
        (1ull << static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::FUSED_RD_1));

public:
    /**
     * Construct from JSON information pertaining to extraction
     * @param inst
     * @param form
     */
    //InstMetaData(const json& inst, const FormWrapperIF* form, bool compressed = false) :
    InstMetaData(const json& inst, bool compressed = false) :
        compressed_(compressed)
    {
        oper_type_.fill(InstMetaData::OperandTypes::NONE);

        parseOverrides(inst);

        if (inst.find("data") != inst.end()) {
            data_size_ = inst["data"];
            // Check positive, power-of-2 or zero
            if ((data_size_ & (data_size_ - 1)) || (int32_t(data_size_) < 0)) {
                throw BuildErrorInvalidDataSize(inst["mnemonic"], data_size_);
            }
        }
        if (inst.find("w-oper") != inst.end()) {
            if (inst["w-oper"] == "all") {
                setAllOperandsType_(OperandTypes::WORD);
            } else {
                FieldNameListType flist;
                flist = inst["w-oper"].get<FieldNameListType>();
                setOperandsType_(flist, OperandTypes::WORD);
            }
        }
        if (inst.find("l-oper") != inst.end()) {
            if (inst["l-oper"] == "all") {
                setAllOperandsType_(OperandTypes::LONG);
            } else {
                FieldNameListType flist;
                flist = inst["l-oper"].get<FieldNameListType>();
                setOperandsType_(flist, OperandTypes::LONG);
            }
        }
        if (inst.find("s-oper") != inst.end()) {
            if (inst["s-oper"] == "all") {
                setAllOperandsType_(OperandTypes::SINGLE);
            } else {
                FieldNameListType flist;
                flist = inst["s-oper"].get<FieldNameListType>();
                setOperandsType_(flist, OperandTypes::SINGLE);
            }
        }
        if (inst.find("d-oper") != inst.end()) {
            if (inst["d-oper"] == "all") {
                setAllOperandsType_(OperandTypes::DOUBLE);
            } else {
                FieldNameListType flist;
                flist = inst["d-oper"].get<FieldNameListType>();
                setOperandsType_(flist, OperandTypes::DOUBLE);
            }
        }
        if (inst.find("q-oper") != inst.end()) {
            if (inst["q-oper"] == "all") {
                setAllOperandsType_(OperandTypes::QUAD);
            } else {
                FieldNameListType flist;
                flist = inst["q-oper"].get<FieldNameListType>();
                setOperandsType_(flist, OperandTypes::QUAD);
            }
        }
        if (inst.find("v-oper") != inst.end()) {
            if (inst["v-oper"] == "all") {
                setAllOperandsType_(OperandTypes::VECTOR);
            } else {
                FieldNameListType flist;
                flist = inst["v-oper"].get<FieldNameListType>();
                setOperandsType_(flist, OperandTypes::VECTOR);
            }
        }
    }

    /**
     * \brief Copy constructor
     * \param other
     */
    InstMetaData(const InstMetaData& other) = default;

    InstMetaData::PtrType clone() const
    {
        return std::make_shared<InstMetaData>(*this);
    }

    /**
     * Construct according to ISA (for custom instruction factories)
     * @param iset
     */
    // TODO: finish this
    InstMetaData(ISA iset)
    {
        oper_type_.fill(InstMetaData::OperandTypes::NONE);

        switch (iset) {
            case ISA::RV32I:
                //field_set_[static_cast<uint32_t>(OperandTypes::WORD)] = -1u;
                setAllOperandsType_(OperandTypes::WORD);
                inst_types_ = static_cast<std::underlying_type_t<InstructionTypes>>(InstructionTypes::INT);
                break;
            case ISA::RV64I:
                //field_set_[static_cast<uint32_t>(OperandTypes::LONG)] = -1u;
                setAllOperandsType_(OperandTypes::LONG);
                inst_types_ = static_cast<std::underlying_type_t<InstructionTypes>>(InstructionTypes::INT);
                break;
        }
    }

    /**
     * \brief Merge information from another InstMetaData object
     * For now, we only merge in the instruction type and compression flag
     *
     * \param other
     */
    void merge(const InstMetaData::PtrType& other)
    {
        assert(other != nullptr);
        compressed_ = other->compressed_;
        inst_types_ |= other->inst_types_;
    }

    void parseOverrides(const json& inst)
    {
        if (inst.find("type") != inst.end()) {
            const FieldNameListType tlist = inst["type"].get<FieldNameListType>();
            for (const auto& t : tlist) {
                const auto itr = tmap_.find(t);
                if (itr == tmap_.end()) {
                    throw BuildErrorUnknownType(inst["mnemonic"], t);
                }
                inst_types_ |= static_cast<std::underlying_type_t<InstructionTypes>>(itr->second);
            }
        }
    }

    template<typename ...ArgTypes>
    void setInstType(const ArgTypes&& ... args)
    {
        const std::vector<InstructionTypes> itypes {args...};
        for (const auto& i : itypes) {
            inst_types_ |= static_cast<std::underlying_type_t<InstructionTypes>>(i);
        }
    }

    std::underlying_type_t<InstructionTypes> getInstType() const
    {
        return inst_types_;
    }

    bool isInstType(const InstructionTypes itype) const
    {
        return (static_cast<std::underlying_type_t<InstructionTypes>>(itype) & inst_types_) ==
               static_cast<std::underlying_type_t<InstructionTypes>>(itype);
    }

    template<typename ...ArgTypes>
    bool isInstTypeAnyOf(ArgTypes&& ... args) const
    {
        const std::vector<InstructionTypes> itypes {args...};
        for (const auto& i : itypes) {
            if (isInstType(i)) {
                return true;
            }
        }
        return false;
    }

    template<typename ...ArgTypes>
    bool isInstTypeAllOf(ArgTypes&& ... args) const
    {
        const std::vector<InstructionTypes> itypes {args...};
        for (const auto& i : itypes) {
            if (!isInstType(i)) {
                return false;
            }
        }
        return true;
    }

    template<typename ...ArgTypes>
    bool isInstTypeSameAs(std::underlying_type_t<InstructionTypes> other_types, ArgTypes&& ... args) const
    {
        const std::vector<InstructionTypes> itypes {args...};
        if (itypes.empty()) {
            return (inst_types_ == other_types);
        }

        for (const auto& i : itypes) {
            if ((static_cast<std::underlying_type_t<InstructionTypes>>(i) & inst_types_) !=
                (static_cast<std::underlying_type_t<InstructionTypes>>(i) & other_types)) {
                return false;
            }
        }
        return true;
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
        return ((1u << static_cast<std::underlying_type_t<OperandFieldID>>(fid)) &
                field_set_[static_cast<std::underlying_type_t<OperandTypes>>(kind)]) != 0;
    }

    bool isAnySourceOperandType(const OperandTypes kind) const
    {
        return (source_fields_ & field_set_[static_cast<std::underlying_type_t<OperandTypes>>(kind)]) != 0;
    }

    bool isAnyDestOperandType(const OperandTypes kind) const
    {
        return (dest_fields_ & field_set_[static_cast<std::underlying_type_t<OperandTypes>>(kind)]) != 0;
    }

    OperandTypes getOperandType(const OperandFieldID fid) const {
        return oper_type_[static_cast<std::underlying_type_t<OperandFieldID>>(fid)];
    }

    // DirectInfo extractions don't provide an operand field ID, so we pick a default type
    OperandTypes getDefaultSourceType() const
    {
        for (auto i = static_cast<std::underlying_type_t<OperandTypes>>(OperandTypes::__FIRST);
             i < static_cast<std::underlying_type_t<OperandTypes>>(OperandTypes::__N);
             ++i) {
            if (isAnySourceOperandType(static_cast<OperandTypes>(i))) {
                return static_cast<OperandTypes>(i);
            }
        }
        return OperandTypes::NONE;
    }

    // DirectInfo extractions don't provide an operand field ID, so we pick a default type
    OperandTypes getDefaultDestType() const
    {
        for (auto i = static_cast<std::underlying_type_t<OperandTypes>>(OperandTypes::__FIRST);
             i < static_cast<std::underlying_type_t<OperandTypes>>(OperandTypes::__N);
             ++i) {
            if (isAnyDestOperandType(static_cast<OperandTypes>(i))) {
                return static_cast<OperandTypes>(i);
            }
        }
        return OperandTypes::NONE;
    }

    uint32_t getDataSize() const
    {
        return data_size_;
    }

    static inline const std::string& getFieldIDName(OperandFieldID fid)
    {
        assert(fid != OperandFieldID::NONE);
        return ofinames_[static_cast<std::underlying_type_t<OperandFieldID>>(fid)];
    }

    static inline OperandFieldID getFieldID(const std::string& fname)
    {
        const auto itr = ofimap_.find(fname);
        if (itr == ofimap_.end()) {
            return OperandFieldID::NONE;
        } else {
            return itr->second;
        }
    }

private:
    bool compressed_ = false;
    std::underlying_type_t<InstructionTypes> inst_types_ = 0;
    std::array<uint32_t, static_cast<size_t>(OperandTypes::__N)> field_set_ {0};    /// Maps operand type to bit set of fields
    std::array<OperandTypes, static_cast<size_t>(OperandFieldID::__N)> oper_type_;  /// Maps field to operand type
    uint32_t data_size_ = 0;

    static inline std::underlying_type_t<OperandFieldID> getFieldIndex_(const std::string& fname)
    {
        const auto itr = ofimap_.find(fname);
        if (itr == ofimap_.end()) {
            throw BuildErrorUnknownFormField("InstMetaData", fname);
        }
        return static_cast<std::underlying_type_t<OperandFieldID>>(itr->second);
    }

    static inline uint64_t getFieldIDSet_(const std::vector<std::string>& flist)
    {
        uint64_t fset = 0;
        for (const auto& f : flist) {
            fset |= (1u << getFieldIndex_(f));
        }
        return fset;
    }

    inline void setOperandsType_(const FieldNameListType& flist, OperandTypes ot) {
        field_set_[static_cast<std::underlying_type_t<OperandTypes>>(ot)] = getFieldIDSet_(flist);
        for (const auto& f : flist) {
            oper_type_[getFieldIndex_(f)] = ot;
        }
    }

    inline void setAllOperandsType_(OperandTypes ot) {
        field_set_[static_cast<std::underlying_type_t<OperandTypes>>(ot)] = -1u;
        for (uint32_t fid = 0; fid < static_cast<std::underlying_type_t<OperandFieldID>>(OperandFieldID::__N); ++fid) {
            oper_type_[fid] = ot;
        }
    }
};

} // namespace mavis
