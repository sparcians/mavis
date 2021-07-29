#pragma once

#include "DecoderConsts.h"
#include "DecoderExceptions.h"
#include "ExtractorIF.h"
#include <string>

namespace mavis {

/**
 * \brief Interface for direct extractors
 */
class ExtractorDirectInfoIF : public ExtractorIF
{
public:
    virtual ExtractorIF::PtrType clone() const = 0;

    virtual std::string getMnemonic() const = 0;

    virtual InstructionUniqueID getUID() const = 0;

    bool isIllop(Opcode) const override
    {
        return false;
    }

    bool isHint(Opcode) const override
    {
        return false;
    }

    bool hasImmediate() const override
    {
        return false;
    }
};

/**
 * \brief Base class of common and convenience methods
 */
class ExtractorDirectBase : public ExtractorDirectInfoIF
{
public:
    explicit ExtractorDirectBase(const std::string &mnemonic, uint64_t imm = 0) :
        mnemonic_(mnemonic), uid_(INVALID_UID), immediate_(imm)
    {}

    explicit ExtractorDirectBase(const InstructionUniqueID uid, uint64_t imm = 0) :
        mnemonic_("UNSET-in-ExtractorDirectBase"), uid_(uid), immediate_(imm)
    {}

    ExtractorDirectBase(const ExtractorDirectBase &other) = default;

    std::string getMnemonic() const override
    {
        return mnemonic_;
    }

    InstructionUniqueID getUID() const override
    {
        return uid_;
    }

    // Default implementation returns 0 bitset
    uint64_t getSourceRegs(const Opcode icode) const override
    {
        return 0;
    }

    // Default implementation returns 0 bitset
    uint64_t getSourceAddressRegs(const Opcode) const override
    {
        return 0;
    }

    // Default implementation returns 0 bitset
    uint64_t getSourceDataRegs(const Opcode) const override
    {
        return 0;
    }

    // Default implementation returns 0 bitset
    uint64_t getDestRegs(const Opcode) const override
    {
        return 0;
    }

    // Default implementation returns empty list
    OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType&, bool suppress_x0 = false) const override
    {
        return {};
    }

    // Default implementation returns empty list
    OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType&, bool suppress_x0 = false) const override
    {
        return {};
    }

    uint64_t getImmediate(const uint64_t) const override
    {
        return immediate_;
    }

    int64_t getSignedOffset(const uint64_t icode) const override
    {
        return getImmediate(icode);
    }

    uint64_t getSourceOperTypeRegs(const uint64_t,
                                   const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        if (meta->isAllOperandType(kind) || meta->isAnySourceOperandType(kind)) {
            return getSourceRegs(0);
        } else {
            return 0;
        }
    }

    uint64_t getDestOperTypeRegs(const uint64_t,
                                 const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        if (meta->isAllOperandType(kind) || meta->isAnyDestOperandType(kind)) {
            return getDestRegs(0);
        } else {
            return 0;
        }
    }

    uint64_t getSpecialField(SpecialField, Opcode) const override
    {
        throw InvalidExtractorSpecialFieldID(mnemonic_);
    }

    void print(std::ostream &os) const override
    {
        // TODO: Need a print function
    }

private:
    ExtractorIF::PtrType specialCaseClone(const uint64_t, const uint64_t) const override
    {
        assert(false);
        return nullptr;
    }

protected:
    const std::string mnemonic_;
    const InstructionUniqueID uid_ = INVALID_UID;
    const uint64_t immediate_;

    /**
     * \brief Returns a vector of bit indices from combined (AND-ed) BitMasks
     * \tparam ArgTypes
     * \param args
     * \return
     */
    static constexpr uint32_t count1Bits_(const uint64_t n)
    {
        uint64_t x = n - ((n >> 1u) & 0x5555555555555555ull);
        x = (x & 0x3333333333333333ull) + ((x >> 2u) & 0x3333333333333333ull);
        x = (x + (x >> 4u)) & 0x0F0F0F0F0F0F0F0Full;
        x = x + (x >> 8u);
        x = x + (x >> 16u);
        x = x + (x >> 32u);
        return x & 0x7Full;
    }

    static inline std::string bitmaskToStringVals_(uint64_t bits)
    {
        // Iterate through the combined mask, adding 1-bit positions to the
        // returned vector
        std::stringstream ss;
        const uint32_t bitcnt = count1Bits_(bits);
        if (bitcnt > 0) {
            uint32_t found_bits = 0;
            for (uint32_t i = 0; (found_bits != bitcnt); ++i) {
                if (bits & (0x1ull << i)) {
                    if (found_bits != 0) {
                        ss << ",";
                    }
                    ++found_bits;
                    ss << i;
                }
            }
        }
        return ss.str();
    }

    static inline RegListType bitmaskToRegList_(uint64_t bits)
    {
        // Iterate through the combined mask, adding 1-bit positions to the
        // returned vector
        RegListType rlist;
        const uint32_t bitcnt = count1Bits_(bits);
        if (bitcnt > 0) {
            uint32_t found_bits = 0;
            for (uint32_t i = 0; (found_bits != bitcnt); ++i) {
                if (bits & (0x1ull << i)) {
                    rlist.push_back(i);
                    ++found_bits;
                }
            }
        }
        return rlist;
    }
};

class ExtractorDirectInfo : public ExtractorDirectBase
{
private:
    static inline const std::string name_ {"ExtractorDirectInfo"};

public:
    ExtractorDirectInfo(const std::string &mnemonic, const RegListType &sources, const RegListType &dests, uint64_t imm = 0) :
        ExtractorDirectBase(mnemonic, imm), sources_(sources), dests_(dests)
    {}

    ExtractorDirectInfo(const std::string &mnemonic, const RegListType &sources, const RegListType &dests,
                        const ValueListType& specials, uint64_t imm = 0) :
        ExtractorDirectBase(mnemonic, imm), sources_(sources), dests_(dests), specials_(specials)
    {}

    ExtractorDirectInfo(const InstructionUniqueID uid, const RegListType &sources, const RegListType &dests, uint64_t imm = 0) :
        ExtractorDirectBase(uid, imm), sources_(sources), dests_(dests)
    {}

    ExtractorDirectInfo(const InstructionUniqueID uid, const RegListType &sources, const RegListType &dests,
                        const ValueListType& specials, uint64_t imm = 0) :
        ExtractorDirectBase(uid, imm), sources_(sources), dests_(dests), specials_(specials)
    {}

    ExtractorDirectInfo(const ExtractorDirectInfo &other) = default;

    ExtractorIF::PtrType clone() const override
    {
        return std::make_shared<ExtractorDirectInfo>(*this);
    }

    const std::string &getName() const override
    {
        return name_;
    }

    uint64_t getSourceRegs(const uint64_t) const override
    {
        uint64_t src_bits = 0;
        for (const auto reg : sources_) {
            if (reg > MAX_REG_NUM) {
                throw InvalidRegisterNumber(mnemonic_, reg);
            } else {
                src_bits |= (1ull << reg);
            }
        }
        return src_bits;
    }

    uint64_t getDestRegs(const uint64_t) const override
    {
        uint64_t dst_bits = 0;
        for (const auto reg : dests_) {
            if (reg > MAX_REG_NUM) {
                throw InvalidRegisterNumber(mnemonic_, reg);
            } else {
                dst_bits |= (1ull << reg);
            }
        }
        return dst_bits;
    }

    OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        uint32_t field_id = static_cast<uint32_t>(InstMetaData::OperandFieldID::RS1);
        for (const auto& reg : sources_) {
            const InstMetaData::OperandFieldID op_field_id = static_cast<InstMetaData::OperandFieldID>(field_id);
            olist.addElement(op_field_id, meta->getOperandType(op_field_id), reg, false);
            ++field_id;
        }
        return olist;
    }

    OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType& meta,
                                   bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        for (const auto& reg : dests_) {
            olist.addElement(InstMetaData::OperandFieldID::RD, meta->getDefaultDestType(),
                             reg, false);
        }
        return olist;
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const uint64_t) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t";
        for (const auto reg : dests_) {
            ss << static_cast<uint32_t>(reg) << ",";
        }
        for (const auto reg : sources_) {
            ss << static_cast<uint32_t>(reg) << ",";
        }
        ss << " 0x" << std::hex << immediate_;
        return ss.str();
    }

private:
    const RegListType sources_;
    const RegListType dests_;
    const ValueListType specials_;

private:
    uint64_t getSpecialFieldByIndex_(uint32_t index) const override
    {
        // We want bounds checking...
        return specials_.at(index);
    }
};

} // namespace mavis
