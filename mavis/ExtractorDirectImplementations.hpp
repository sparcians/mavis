
#pragma once

#include "ExtractorDirectInfo.h"

namespace mavis
{

/**
 * Direct Extractor: provides programmatic interface. This derivative takes operand register lists
 */
class ExtractorPseudoInfo : public ExtractorDirectBase
{
private:
    const std::string name_ {"ExtractorPseudoInfo"};

public:
    ExtractorPseudoInfo(const std::string &mnemonic,
                        const OperandInfo::ElementList & sources,
                        const OperandInfo::ElementList & dests,
                        uint64_t imm = 0) :
        ExtractorDirectBase(mnemonic, imm),
        sources_(sources),
        dests_(dests)
    {}

    ExtractorPseudoInfo(const InstructionUniqueID uid,
                        const OperandInfo::ElementList & sources,
                        const OperandInfo::ElementList & dests,
                        uint64_t imm = 0) :
        ExtractorDirectBase(uid, imm),
        sources_(sources),
        dests_(dests)
    {}

    ExtractorPseudoInfo(const ExtractorPseudoInfo &other) = default;

    ExtractorIF::PtrType clone() const override
    {
        return std::make_shared<ExtractorPseudoInfo>(*this);
    }

    const std::string &getName() const override
    {
        return name_;
    }

    uint64_t getSourceOperTypeRegs(const uint64_t,
                                   const InstMetaData::PtrType &meta,
                                   InstMetaData::OperandTypes kind) const override
    {
        (void) meta; // don't know what this is for...
        uint64_t bits = 0;
        for (const auto reg : sources_.getElements()) {
            if(reg.operand_type == kind) {
                bits |= (1ull << reg.field_value);
            }
        }
        return bits;
    }

    uint64_t getSourceRegs(const uint64_t) const override
    {
        uint64_t src_bits = 0;
        for (const auto reg : sources_.getElements()) {
            if (reg.field_value > MAX_REG_NUM) {
                throw InvalidRegisterNumber(mnemonic_, reg.field_value);
            } else {
                src_bits |= (1ull << reg.field_value);
            }
        }
        return src_bits;
    }

    uint64_t getDestOperTypeRegs(const uint64_t,
                                 const InstMetaData::PtrType &meta,
                                 InstMetaData::OperandTypes kind) const override
    {
        (void) meta; // don't know what this is for...
        uint64_t bits = 0;
        for (const auto reg : dests_.getElements()) {
            if(reg.operand_type == kind) {
                bits |= (1ull << reg.field_value);
            }
        }
        return bits;
    }

    uint64_t getDestRegs(const uint64_t) const override
    {
        uint64_t dst_bits = 0;
        for (const auto reg : dests_.getElements()) {
            if (reg.field_value > MAX_REG_NUM) {
                throw InvalidRegisterNumber(mnemonic_, reg.field_value);
            } else {
                dst_bits |= (1ull << reg.field_value);
            }
        }
        return dst_bits;
    }

    OperandInfo getSourceOperandInfo(Opcode,
                                     const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        return sources_;
    }

    OperandInfo getDestOperandInfo(Opcode,
                                   const InstMetaData::PtrType& meta,
                                   bool suppress_x0 = false) const override
    {
        return dests_;
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const uint64_t) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t";
        for (const auto reg : dests_.getElements()) {
            ss << static_cast<uint32_t>(reg.field_value) << ",";
        }
        for (const auto reg : sources_.getElements()) {
            ss << static_cast<uint32_t>(reg.field_value) << ",";
        }
        ss << " 0x" << std::hex << immediate_;
        return ss.str();
    }

private:
    const OperandInfo sources_;
    const OperandInfo dests_;
};

/**
 * Direct Extractor: provides programmatic interface. This derivative takes operand register bitmasks
 */
class ExtractorDirectInfoBitMask : public ExtractorDirectBase
{
private:
    const std::string name_ {"ExtractorDirectInfoBitMask"};

public:
    ExtractorDirectInfoBitMask(const std::string &mnemonic, const uint64_t sources, const uint64_t dests,
                               uint64_t imm = 0) :
        ExtractorDirectBase(mnemonic, imm), sources_(sources), dests_(dests)
    {}

    ExtractorDirectInfoBitMask(const InstructionUniqueID uid, const uint64_t sources, const uint64_t dests,
                               uint64_t imm = 0) :
        ExtractorDirectBase(uid, imm), sources_(sources), dests_(dests)
    {}

    ExtractorDirectInfoBitMask(const ExtractorDirectInfoBitMask &other) = default;

    ExtractorIF::PtrType clone() const override
    {
        return std::make_shared<ExtractorDirectInfoBitMask>(*this);
    }

    const std::string &getName() const override
    {
        return name_;
    }

    uint64_t getSourceRegs(const uint64_t) const override
    {
        return sources_;
    }

    //RegListType getSourceList(const Opcode, bool suppress_x0 = false) const override
    //{
    //    uint64_t sources = (suppress_x0 ? (sources_ & ~0x1ull) : sources_);
    //    return bitmaskToRegList_(sources);
    //}

    uint64_t getDestRegs(const uint64_t) const override
    {
        return dests_;
    }

    //RegListType getDestList(const Opcode, bool suppress_x0 = false) const override
    //{
    //    uint64_t dests = (suppress_x0 ? (dests_ & ~0x1ull) : dests_);
    //    return bitmaskToRegList_(dests);
    //}

    OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        RegListType src_list = bitmaskToRegList_(sources_);
        for (const auto& reg : src_list) {
            olist.addElement(InstMetaData::OperandFieldID::NONE, meta->getDefaultSourceType(),
                               reg, false);
        }
        return olist;
    }

    OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType& meta,
                                   bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        RegListType dst_list = bitmaskToRegList_(dests_);
        for (const auto& reg : dst_list) {
            olist.addElement(InstMetaData::OperandFieldID::NONE, meta->getDefaultDestType(),
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
        ss << bitmaskToStringVals_(dests_) << ",";
        ss << bitmaskToStringVals_(sources_);
        ss << " 0x" << std::hex << immediate_;
        return ss.str();
    }

private:
    const uint64_t sources_;
    const uint64_t dests_;
};

/**
 * Direct Extractor for STORES: provides programmatic interface
 */
class ExtractorDirectInfo_Stores : public ExtractorDirectBase
{
private:
    const std::string name_ {"ExtractorDirectInfo_Stores"};

public:
    ExtractorDirectInfo_Stores(const std::string &mnemonic, const RegListType &addr_sources,
                               const RegListType &data_sources, uint64_t imm = 0) :
        ExtractorDirectBase(mnemonic, imm), addr_sources_(addr_sources), data_sources_(data_sources)
    {}

    ExtractorDirectInfo_Stores(const std::string &mnemonic, const RegListType &addr_sources,
                               const RegListType &data_sources, const ValueListType &specials, uint64_t imm = 0) :
        ExtractorDirectBase(mnemonic, imm), addr_sources_(addr_sources), data_sources_(data_sources), specials_(specials)
    {}

    ExtractorDirectInfo_Stores(const InstructionUniqueID uid, const RegListType &addr_sources,
                               const RegListType &data_sources, uint64_t imm = 0) :
        ExtractorDirectBase(uid, imm), addr_sources_(addr_sources), data_sources_(data_sources)
    {}

    ExtractorDirectInfo_Stores(const InstructionUniqueID uid, const RegListType &addr_sources,
                               const RegListType &data_sources, const ValueListType &specials, uint64_t imm = 0) :
        ExtractorDirectBase(uid, imm), addr_sources_(addr_sources), data_sources_(data_sources), specials_(specials)
    {}

    ExtractorDirectInfo_Stores(const ExtractorDirectInfo_Stores &other) = default;

    ExtractorIF::PtrType clone() const override
    {
        return std::make_shared<ExtractorDirectInfo_Stores>(*this);
    }

    const std::string &getName() const override
    {
        return name_;
    }

    uint64_t getSourceAddressRegs(const uint64_t) const override
    {
        uint64_t src_bits = 0;
        for (const auto reg : addr_sources_) {
            if (reg > MAX_REG_NUM) {
                throw InvalidRegisterNumber(mnemonic_, reg);
            } else {
                src_bits |= (1ull << reg);
            }
        }
        return src_bits;
    }

    uint64_t getSourceDataRegs(const uint64_t icode) const override
    {
        uint64_t src_bits = 0;
        for (const auto reg : data_sources_) {
            if (reg > MAX_REG_NUM) {
                throw InvalidRegisterNumber(mnemonic_, reg);
            } else {
                src_bits |= (1ull << reg);
            }
        }
        return src_bits;
    }

    uint64_t getSourceRegs(const uint64_t) const override
    {
        return getSourceAddressRegs(0) | getSourceDataRegs(0);
    }

    OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        // Order is important here, first address sources, then data sources. This is to match
        // up with the FormGeneric ordering in parsing the JSON information...
        for (const auto& reg : addr_sources_) {
            olist.addElement(InstMetaData::OperandFieldID::NONE, meta->getDefaultSourceType(),
                               reg, false);
        }
        for (const auto& reg : data_sources_) {
            olist.addElement(InstMetaData::OperandFieldID::NONE, meta->getDefaultSourceType(),
                               reg, true);
        }
        return olist;
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const uint64_t) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t";
        for (const auto reg : data_sources_) {
            ss << static_cast<uint32_t>(reg) << "(D),";
        }
        for (const auto reg : addr_sources_) {
            ss << static_cast<uint32_t>(reg) << "(A),";
        }
        ss << " 0x" << std::hex << immediate_;
        return ss.str();
    }

private:
    const RegListType addr_sources_;
    const RegListType data_sources_;
    const ValueListType specials_;

private:
    uint64_t getSpecialFieldByIndex_(uint32_t index) const override
    {
        // We want bounds checking...
        return specials_.at(index);
    }
};

/**
 * Direct Extractor: provides programmatic interface. This derivative takes operand register bitmasks for STORES
 */
class ExtractorDirectInfoBitMask_Stores : public ExtractorDirectBase
{
private:
    const std::string name_ {"ExtractorDirectInfoBitMask_Stores"};

public:
    ExtractorDirectInfoBitMask_Stores(const std::string &mnemonic, const uint64_t addr_sources,
                                      const uint64_t data_sources, uint64_t imm = 0) :
        ExtractorDirectBase(mnemonic, imm), addr_sources_(addr_sources), data_sources_(data_sources)
    {}

    ExtractorDirectInfoBitMask_Stores(const InstructionUniqueID uid, const uint64_t addr_sources,
                                      const uint64_t data_sources, uint64_t imm = 0) :
        ExtractorDirectBase(uid, imm), addr_sources_(addr_sources), data_sources_(data_sources)
    {}

    ExtractorDirectInfoBitMask_Stores(const ExtractorDirectInfoBitMask_Stores &other) = default;

    ExtractorIF::PtrType clone() const override
    {
        return std::make_shared<ExtractorDirectInfoBitMask_Stores>(*this);
    }

    const std::string &getName() const override
    {
        return name_;
    }

    uint64_t getSourceAddressRegs(const uint64_t) const override
    {
        return addr_sources_;
    }

    //RegListType getSourceAddressList(const Opcode, bool suppress_x0 = false) const override
    //{
    //    uint64_t addr_sources = (suppress_x0 ? (addr_sources_ & ~0x1ull) : addr_sources_);
    //    return bitmaskToRegList_(addr_sources);
    //}

    uint64_t getSourceDataRegs(const uint64_t icode) const override
    {
        return data_sources_;
    }

    //RegListType getSourceDataList(const Opcode, bool suppress_x0 = false) const override
    //{
    //    uint64_t data_sources = (suppress_x0 ? (data_sources_ & ~0x1ull) : data_sources_);
    //    return bitmaskToRegList_(data_sources);
    //}

    uint64_t getSourceRegs(const uint64_t) const override
    {
        return addr_sources_ | data_sources_;
    }

    //RegListType getSourceList(const Opcode, bool suppress_x0 = false) const override
    //{
    //    uint64_t sources = getSourceRegs(0) & ~(suppress_x0 ? 0x1ull : 0x0ull);
    //    return bitmaskToRegList_(sources);
    //}

    OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        RegListType addr_list = bitmaskToRegList_(addr_sources_);
        for (const auto& reg : addr_list) {
            olist.addElement(InstMetaData::OperandFieldID::NONE, meta->getDefaultSourceType(),
                               reg, false);
        }
        RegListType data_list = bitmaskToRegList_(data_sources_);
        for (const auto& reg : data_list) {
            olist.addElement(InstMetaData::OperandFieldID::NONE, meta->getDefaultSourceType(),
                               reg, true);
        }
        return olist;
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const uint64_t) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t D:";
        ss << bitmaskToStringVals_(data_sources_) << ", A:";
        ss << bitmaskToStringVals_(addr_sources_);
        ss << " 0x" << std::hex << immediate_;
        return ss.str();
    }

private:
    const uint64_t addr_sources_;
    const uint64_t data_sources_;
};

/**
 * Direct Extractor: provides programmatic interface. This derivative takes operand register bitmasks for STORES
 * which include a destination register. These are not currently part of any ISA, but an artifact of fusing
 * ALU ops with stores
 */
class ExtractorDirectInfoBitMask_DestStores : public ExtractorDirectBase
{
private:
    const std::string name_ {"ExtractorDirectInfoBitMask_Stores"};

public:
    ExtractorDirectInfoBitMask_DestStores(const std::string &mnemonic, const uint64_t addr_sources,
                                          const uint64_t data_sources, const uint64_t dests, uint64_t imm = 0) :
        ExtractorDirectBase(mnemonic, imm), addr_sources_(addr_sources), data_sources_(data_sources), dests_(dests)
    {}

    ExtractorDirectInfoBitMask_DestStores(const InstructionUniqueID uid, const uint64_t addr_sources,
                                          const uint64_t data_sources, const uint64_t dests, uint64_t imm = 0) :
        ExtractorDirectBase(uid, imm), addr_sources_(addr_sources), data_sources_(data_sources), dests_(dests)
    {}

    ExtractorDirectInfoBitMask_DestStores(const ExtractorDirectInfoBitMask_DestStores &other) = default;

    ExtractorIF::PtrType clone() const override
    {
        return std::make_shared<ExtractorDirectInfoBitMask_DestStores>(*this);
    }

    const std::string &getName() const override
    {
        return name_;
    }

    uint64_t getSourceAddressRegs(const uint64_t) const override
    {
        return addr_sources_;
    }

    uint64_t getSourceDataRegs(const uint64_t) const override
    {
        return data_sources_;
    }

    uint64_t getSourceRegs(const uint64_t) const override
    {
        return addr_sources_ | data_sources_;
    }

    uint64_t getDestRegs(const uint64_t) const override
    {
        return dests_;
    }

    OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        RegListType addr_list = bitmaskToRegList_(addr_sources_);
        for (const auto& reg : addr_list) {
            olist.addElement(InstMetaData::OperandFieldID::NONE, meta->getDefaultSourceType(),
                             reg, false);
        }
        RegListType data_list = bitmaskToRegList_(data_sources_);
        for (const auto& reg : data_list) {
            olist.addElement(InstMetaData::OperandFieldID::NONE, meta->getDefaultSourceType(),
                             reg, true);
        }
        return olist;
    }

    OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType& meta,
                                   bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        RegListType dst_list = bitmaskToRegList_(dests_);
        for (const auto& reg : dst_list) {
            olist.addElement(InstMetaData::OperandFieldID::NONE, meta->getDefaultDestType(),
                             reg, false);
        }
        return olist;
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
    // overloads are considered
    std::string dasmString(const std::string &mnemonic, const uint64_t) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t " << bitmaskToStringVals_(dests_)
           << ", D:" << bitmaskToStringVals_(data_sources_)
           << ", A:" << bitmaskToStringVals_(addr_sources_)
           << " 0x" << std::hex << immediate_;
        return ss.str();
    }

private:
    const uint64_t addr_sources_;
    const uint64_t data_sources_;
    const uint64_t dests_;
};

} // namespace mavis
