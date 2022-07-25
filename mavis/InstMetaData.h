#pragma once

#include "json.hpp"
#include "DecoderExceptions.h"
#include "FormIF.h"
#include <memory>
#include <array>
#include <cinttypes>
#include <regex>

namespace mavis {

class InstMetaData
{
private:
    using json = nlohmann::json;
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
        STRIDE  = 1ull << 20u,
        INDEXED = 1ull << 21u,
        SEGMENT = 1ull << 22u,
        FAULTFIRST = 1ull << 23u,
        WHOLE = 1ull << 24u,
        MASK = 1ull << 25u,
        HYPERVISOR = 1ull << 26u,
        CACHE = 1ull << 59u,
        ATOMIC = 1ull << 60u,
        FENCE = 1ull << 61u,
        SYSTEM = 1ull << 62u,
        CSR = 1ull << 63u
    };

    enum class ISAExtensionIndex : uint64_t
    {
        A = 0,      // Atomic
        B,          // Bitmanip
        C,          // Compressed
        D,          // Double precision
        F,          // Float (single)
        G,          // "General"
        H,          // Hypervisor
        I,          // Integer
        M,          // Multiply
        Q,          // Quad
        V,          // Vector
        __N         // LAST: number of enum values
    };

    enum class ISAExtension : uint64_t
    {
        A = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(ISAExtensionIndex::A),
        B = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(ISAExtensionIndex::B),
        C = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(ISAExtensionIndex::C),
        D = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(ISAExtensionIndex::D),
        F = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(ISAExtensionIndex::F),
        G = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(ISAExtensionIndex::G),
        H = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(ISAExtensionIndex::H),
        I = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(ISAExtensionIndex::I),
        M = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(ISAExtensionIndex::M),
        Q = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(ISAExtensionIndex::Q),
        V = 1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(ISAExtensionIndex::V)
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
        RS4,
        RS_MAX,
        FUSED_SD_0,       // for fusion: store data operand 0
        FUSED_SD_1,       // for fusion: store data operand 1
        RD,
        RD1 = RD,
        RD2,
        RD_MAX,
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
        {"hypervisor", InstructionTypes::HYPERVISOR},
        {"cache",      InstructionTypes::CACHE},
        {"atomic",     InstructionTypes::ATOMIC},
        {"fence",      InstructionTypes::FENCE},
        {"system",     InstructionTypes::SYSTEM},
        {"csr",        InstructionTypes::CSR}
    };

    static inline std::map<std::string, ISAExtensionIndex> isamap_ {
        { "A",  ISAExtensionIndex::A},
        { "B",  ISAExtensionIndex::B},
        { "C",  ISAExtensionIndex::C},
        { "D",  ISAExtensionIndex::D},
        { "F",  ISAExtensionIndex::F},
        { "G",  ISAExtensionIndex::G},
        { "H",  ISAExtensionIndex::H},
        { "I",  ISAExtensionIndex::I},
        { "M",  ISAExtensionIndex::M},
        { "Q",  ISAExtensionIndex::Q},
        { "V",  ISAExtensionIndex::V}
    };

    static inline const std::regex isa_ext_pattern_ = std::regex("([1-9][0-9]*?)?([A-Z])", std::regex::optimize);

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

        // Data size
        if (inst.find("data") != inst.end()) {
            data_size_ = inst["data"];
            // Check positive, power-of-2 or zero
            if ((data_size_ & (data_size_ - 1)) || (int32_t(data_size_) < 0)) {
                throw BuildErrorInvalidDataSize(inst["mnemonic"], data_size_);
            }
        }

        // Word operand types
        if (inst.find("w-oper") != inst.end()) {
            if (inst["w-oper"] == "all") {
                setAllOperandsType_(OperandTypes::WORD);
            } else {
                FieldNameListType flist;
                flist = inst["w-oper"].get<FieldNameListType>();
                setOperandsType_(flist, OperandTypes::WORD);
            }
        }

        // Long operand types
        if (inst.find("l-oper") != inst.end()) {
            if (inst["l-oper"] == "all") {
                setAllOperandsType_(OperandTypes::LONG);
            } else {
                FieldNameListType flist;
                flist = inst["l-oper"].get<FieldNameListType>();
                setOperandsType_(flist, OperandTypes::LONG);
            }
        }

        // Single operand types
        if (inst.find("s-oper") != inst.end()) {
            if (inst["s-oper"] == "all") {
                setAllOperandsType_(OperandTypes::SINGLE);
            } else {
                FieldNameListType flist;
                flist = inst["s-oper"].get<FieldNameListType>();
                setOperandsType_(flist, OperandTypes::SINGLE);
            }
        }

        // Double operand types
        if (inst.find("d-oper") != inst.end()) {
            if (inst["d-oper"] == "all") {
                setAllOperandsType_(OperandTypes::DOUBLE);
            } else {
                FieldNameListType flist;
                flist = inst["d-oper"].get<FieldNameListType>();
                setOperandsType_(flist, OperandTypes::DOUBLE);
            }
        }

        // Quad operand types
        if (inst.find("q-oper") != inst.end()) {
            if (inst["q-oper"] == "all") {
                setAllOperandsType_(OperandTypes::QUAD);
            } else {
                FieldNameListType flist;
                flist = inst["q-oper"].get<FieldNameListType>();
                setOperandsType_(flist, OperandTypes::QUAD);
            }
        }

        // Vector operand types
        if (inst.find("v-oper") != inst.end()) {
            if (inst["v-oper"] == "all") {
                setAllOperandsType_(OperandTypes::VECTOR);
            } else {
                FieldNameListType flist;
                flist = inst["v-oper"].get<FieldNameListType>();
                setOperandsType_(flist, OperandTypes::VECTOR);
            }
        }

        // Try to find ISA extensions of the form 'wX' where 'w' is a an optional
        // "width" (e.g. 32, 64, etc.) and 'X' is an extension letter
        if (inst.find("isa") != inst.end()) {
            ISAExtListType ilist = inst["isa"].get<ISAExtListType>();
            if (! ilist.empty()) {
                std::smatch matches;
                for (const auto& s : ilist) {
                    // matches[0] is the entire string matched,
                    // matches[1] is the capture group for the optional width (emtpy if not provided)
                    // matches[2] is the capture group for the ISA extension letter
                    if (std::regex_search(s, matches, isa_ext_pattern_)) {
                        const auto itr = isamap_.find(matches[2].str());
                        if (itr != isamap_.end()) {
                            isa_ext_ |= (1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(itr->second));
                        } else {
                            // Invalid ISA extension letter
                            throw BuildErrorInvalidISAExtension(inst["mnemonic"], s, matches[2].str());
                        }
                        if (matches[1].length() != 0) {
                            const uint32_t n = std::strtoull(matches[1].str().c_str(), nullptr, 0);
                            if ((n == 0) || ((n & (n - 1)) != 0)) {
                                // Not a non-zero power of 2
                                throw BuildErrorInvalidISAWidth(inst["mnemonic"], s, n);
                            }
                            setISAWidth(itr->second, n);
                        }
                    } else {
                        // Malformed ISA extension string
                        throw BuildErrorMalformedISAExtension(inst["mnemonic"], s);
                    }
                }
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
    // TODO: Deprecate this
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

    std::underlying_type_t<ISAExtension> getISA() const
    {
        return isa_ext_;
    }

    bool isISA(const ISAExtension isa) const
    {
        return (isa_ext_ & static_cast<std::underlying_type_t<ISAExtension>>(isa)) ==
                static_cast<std::underlying_type_t<ISAExtension>>(isa);
    }

    template<typename ...ArgTypes>
    bool isISAAnyOf(ArgTypes&& ... args) const
    {
        return (ISAtoBits_(std::forward<ArgTypes>(args)...) & isa_ext_) != 0;
    }

    template<typename ...ArgTypes>
    bool isISAAllOf(ArgTypes&& ... args) const
    {
        const std::underlying_type_t<ISAExtension>  isa_bits = ISAtoBits_(std::forward<ArgTypes>(args)...);
        return (isa_bits & isa_ext_) == isa_bits;
    }

    template<typename ...ArgTypes>
    bool isISASameAs(std::underlying_type_t<ISAExtension> other_isa, ArgTypes&& ... args) const
    {
        const std::underlying_type_t<ISAExtension> check_bits = ISAtoBits_(std::forward<ArgTypes>(args)...);
        if (check_bits == 0) {
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
        assert((width != 0) && ((width & (width - 1)) == 0));
        isa_width_[static_cast<std::underlying_type_t<ISAExtensionIndex>>(isa)] |= width;
    }

    bool isISAWidth(const ISAExtensionIndex isa, uint32_t width) const
    {
        const ISAExtension isa_bit_enum = static_cast<ISAExtension>(1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(isa));
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

    void addFixedFields(const std::vector<std::string>& flist)
    {
        for(const auto& field: flist) {
            OperandFieldID fid = getFieldID(field);
            if (fid != OperandFieldID::NONE) {
                fixed_fields_[static_cast<std::underlying_type_t<OperandFieldID>>(getFieldID(field))] = true;
            }
        }
    }

    bool isOperandFixed(const OperandFieldID fid) const {
        if (fid != OperandFieldID::NONE) {
            return fixed_fields_[static_cast<std::underlying_type_t<OperandFieldID>>(fid)];
        } else {
            // By convention, the NONE (non-existent) field is considered a fixed field
            return true;
        }
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
    std::underlying_type_t<ISAExtension> isa_ext_ = 0;
    std::array<uint32_t, static_cast<size_t>(ISAExtensionIndex::__N)> isa_width_ = {0} ; /// Width set (bits) for each of our ISA's
    std::array<uint32_t, static_cast<size_t>(OperandTypes::__N)> field_set_ {0};    /// Maps operand type to bit set of fields
    std::array<OperandTypes, static_cast<size_t>(OperandFieldID::__N)> oper_type_;  /// Maps field to operand type
    std::array<bool, static_cast<size_t>(OperandFieldID::__N)> fixed_fields_ {false};       /// Is operand part of the encoding (fixed field)
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

    // TODO: Refactor the InstType queries above to use this (similar to the ISA queries)
    template<typename ...ArgTypes>
    static inline std::underlying_type_t<InstructionTypes> InstTypesToBits_(ArgTypes&& ... args)
    {
        const std::vector<InstructionTypes> itype_list {args...};
        std::underlying_type_t<InstructionTypes> itype_bits = 0;
        std::for_each(itype_list.cbegin(), itype_list.cend(),
                      [&](const auto& i) { itype_bits |= static_cast<std::underlying_type_t<InstructionTypes>>(i); });
        return itype_bits;
    }

    template<typename ...ArgTypes>
    static inline std::underlying_type_t<ISAExtension> ISAtoBits_(ArgTypes&& ... args)
    {
        const std::vector<ISAExtension> isa_list {args...};
        std::underlying_type_t<ISAExtension> isa_bits = 0;
        std::for_each(isa_list.cbegin(), isa_list.cend(),
                      [&](const auto& i) { isa_bits |= static_cast<std::underlying_type_t<ISAExtension>>(i); });
        return isa_bits;
    }

};

} // namespace mavis
