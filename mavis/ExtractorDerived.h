#pragma once

#include "Extractor.h"
#include "DecoderConsts.h"
#include "ExtractorForms.h"
#include "FormStub.h"

namespace mavis {

/**
 * Derivative of Form_I extractor for MV (addi overlay)
 */
template<>
class Extractor<Form_I_mv> : public Extractor<Form_I>
{
public:
    Extractor<Form_I_mv>() :
        Extractor<Form_I>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_I_mv>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_I_mv::getName();
    }

    bool hasImmediate() const override
    {
        return false;
    }

private:
    Extractor<Form_I_mv>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_I>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_I extractor for LOADS
 * NOTE: rs1 is the address base, rs2 is the source data
 */
template<>
class Extractor<Form_I_load> : public Extractor<Form_I>
{
public:
    Extractor<Form_I_load>() :
        Extractor<Form_I>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_I_load>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_I_load::getName();
    }

    uint64_t getSourceAddressRegs(const Opcode icode) const override
    {
        return getSourceRegs(icode);
    }

    //RegListType getSourceAddressList(const Opcode icode, bool suppress_x0 = false) const override
    //{
    //    return getSourceList(icode, suppress_x0);
    //}

private:
    Extractor<Form_I_load>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_I>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C0 extractor for LOADS
 */
template<>
class Extractor<Form_C0_load> : public Extractor<Form_C0>
{
public:
    Extractor<Form_C0_load>() :
        Extractor<Form_C0>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C0_load>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C0_load::getName();
    }

    uint64_t getSourceAddressRegs(const Opcode icode) const override
    {
        return getSourceRegs(icode);
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const Opcode icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\t" << extractCompressedRegister_(Form_C0::idType::RD, icode & ~fixed_field_mask_)
           << "," << extractCompressedRegister_(Form_C0::idType::RS1, icode & ~fixed_field_mask_)
           << ", +0x" << std::hex << getImmediate(icode);
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatCompressedRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_C0::idType::RD, InstMetaData::OperandFieldID::RD },
                                   { Form_C0::idType::RS1, InstMetaData::OperandFieldID::RS1} })
            << ", +0x" << std::hex << getImmediate(icode);
        return ss.str();
    }

protected:
    Extractor<Form_C0_load>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C0>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C0 extractor for LOAD WORDS
 */
template<>
class Extractor<Form_C0_load_word> : public Extractor<Form_C0_load>
{
public:
    Extractor<Form_C0_load_word>() :
        Extractor<Form_C0_load>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C0_load_word>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C0_load_word::getName();
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        const uint64_t imm = (extract_(Form_C0::idType::IMM3, icode) << 2ull) |
                             extract_(Form_C0::idType::IMM2, icode);
        using R = Swizzler::Range;
        // Bit ranges to extract from imm, starting with LSB
        return Swizzler::extract(imm, R{6}, R{2}, R{3, 5});
    }

private:
    Extractor<Form_C0_load_word>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C0_load>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C0 extractor for LOAD DOUBLES
 */
template<>
class Extractor<Form_C0_load_double> : public Extractor<Form_C0_load>
{
public:
    Extractor<Form_C0_load_double>() :
        Extractor<Form_C0_load>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C0_load_double>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C0_load_double::getName();
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        const uint64_t imm = (extract_(Form_C0::idType::IMM3, icode) << 2ull) |
                              extract_(Form_C0::idType::IMM2, icode);
        using R = Swizzler::Range;
        // Bit ranges to extract from imm, starting with LSB
        return Swizzler::extract(imm, R{6, 7}, R{3, 5});
    }

private:
    Extractor<Form_C0_load_double>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C0_load>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C0 extractor for STORES
 * NOTE: rs1 is the address base, rs2 is the source data
 */
template<>
class Extractor<Form_C0_store> : public Extractor<Form_C0>
{
public:
    Extractor<Form_C0_store>() :
        Extractor<Form_C0>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C0_store>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C0_store::getName();
    }

    uint64_t getDestRegs(const uint64_t icode) const override
    {
        return 0;
    }

    uint64_t getSourceRegs(const Opcode icode) const override
    {
        return extractUnmaskedCompressedIndexBit_(Form_C0::idType::RS1, icode, fixed_field_mask_) |
               extractUnmaskedCompressedIndexBit_(Form_C0::idType::RD, icode, fixed_field_mask_);
    }

    uint64_t getSourceOperTypeRegs(const Opcode icode,
                                   const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        if (meta->isNoneOperandType(kind)) {
            return 0;
        } else if (meta->isAllOperandType(kind)) {
            return getSourceRegs(icode);
        } else {
            uint64_t result = 0;
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind)) {
                result |= extractUnmaskedCompressedIndexBit_(Form_C0::idType::RS1, icode, fixed_field_mask_);
            }
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind)) {
                result |= extractUnmaskedCompressedIndexBit_(Form_C0::idType::RD, icode, fixed_field_mask_);
            }
            return result;
        }
    }

    uint64_t getSourceAddressRegs(const uint64_t icode) const override
    {
        return extractUnmaskedCompressedIndexBit_(Form_C0::idType::RS1, icode, fixed_field_mask_);
    }

    // RS2 is in Form_C0's RD slot
    uint64_t getSourceDataRegs(const uint64_t icode) const override
    {
        return extractUnmaskedCompressedIndexBit_(Form_C0::idType::RD, icode, fixed_field_mask_);
    }

    OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                         bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        appendUnmaskedCompressedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                              fixed_field_mask_, Form_C0::idType::RS1, false);
        appendUnmaskedCompressedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                              fixed_field_mask_, Form_C0::idType::RD, true);
        return olist;
    }

    OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType&, bool suppress_x0 = false) const override
    {
        return {};
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const uint64_t icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\t" << extractCompressedRegister_(Form_C0::idType::RD, icode & ~fixed_field_mask_)
           << "," << extractCompressedRegister_(Form_C0::idType::RS1, icode & ~fixed_field_mask_)
           << ",0x" << std::hex << getImmediate(icode);
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatCompressedRegList_(meta, icode, fixed_field_mask_,
                                           { { Form_C0::idType::RD, InstMetaData::OperandFieldID::RS2 },
                                             { Form_C0::idType::RS1, InstMetaData::OperandFieldID::RS1} })
           << ", +0x" << std::hex << getImmediate(icode);
        return ss.str();
    }

protected:
    Extractor<Form_C0_store>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C0>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C0_store extractor for STORE WORDS
 * NOTE: rs1 is the address base, rs2 is the source data
 */
template<>
class Extractor<Form_C0_store_word> : public Extractor<Form_C0_store>
{
public:
    Extractor<Form_C0_store_word>() :
        Extractor<Form_C0_store>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C0_store_word>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C0_store_word::getName();
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        const uint64_t imm = (extract_(Form_C0::idType::IMM3, icode) << 2ull) |
            extract_(Form_C0::idType::IMM2, icode);
        using R = Swizzler::Range;
        // Bit ranges to extract from imm, starting with LSB
        return Swizzler::extract(imm, R{6}, R{2}, R{3, 5});
    }

private:
    Extractor<Form_C0_store_word>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C0_store>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C0_store extractor for STORE DOUBLES
 * NOTE: rs1 is the address base, rs2 is the source data
 */
template<>
class Extractor<Form_C0_store_double> : public Extractor<Form_C0_store>
{
public:
    Extractor<Form_C0_store_double>() :
        Extractor<Form_C0_store>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C0_store_double>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C0_store_double::getName();
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        const uint64_t imm = (extract_(Form_C0::idType::IMM3, icode) << 2ull) |
            extract_(Form_C0::idType::IMM2, icode);
        using R = Swizzler::Range;
        // Bit ranges to extract from imm, starting with LSB
        return Swizzler::extract(imm, R{6, 7}, R{3, 5});
    }

private:
    Extractor<Form_C0_store_double>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C0_store>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_CI extractor for c.addi (hint special cases)
 */
template<>
class Extractor<Form_CI_addi> : public Extractor<Form_CI>
{
public:
    Extractor<Form_CI_addi>() :
        Extractor<Form_CI>()
    {}

    bool isHint(Opcode icode) const override
    {
        return getImmediate(icode) == 0;
    }

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_CI_addi>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_CI_addi::getName();
    }

private:
    Extractor<Form_CI_addi>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_CI>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_CI extractor for c.addiw (illop special cases)
 */
template<>
class Extractor<Form_CI_addiw> : public Extractor<Form_CI>
{
public:
    Extractor<Form_CI_addiw>() :
        Extractor<Form_CI>()
    {}

    bool isIllop(Opcode icode) const override
    {
        return (getDestRegs(icode) & (1ull << REGISTER_X0)) != 0;
    }

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_CI_addiw>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_CI_addiw::getName();
    }

private:
    Extractor<Form_CI_addiw>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_CI>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_CI extractor with SP source and destination
 * SP is explicitly encoded (c.addi16sp)
 */
template<>
class Extractor<Form_CI_sp> : public Extractor<Form_CI>
{
public:
    Extractor<Form_CI_sp>() :
        Extractor<Form_CI>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_CI_sp>(ffmask, fset));
    }

    bool isIllop(Opcode icode) const override
    {
        // Don't call getImmediate() here, since we're just checking for 0
        // in both fields and don't need to shift or swizzle
        return ((extract_(Form_CI::idType::IMM1, icode) == 0) &&
                (extract_(Form_CI::idType::IMM5, icode) == 0));
    }

    const std::string &getName() const override
    {
        return Form_CI_sp::getName();
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        const uint64_t imm = (extract_(Form_CI::idType::IMM1, icode) << 5ull) |
            extract_(Form_CI::idType::IMM5, icode);
        using R = Swizzler::Range;
        // Bit ranges to extract from imm, starting with LSB
        return Swizzler::extract(imm, R{5}, R{7,8}, R{6}, R{4}, R{9});
    }

    int64_t getSignedOffset(const Opcode icode) const override
    {
        return signExtend_(getImmediate(icode), 9);
    }

private:
    Extractor<Form_CI_sp>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_CI>(ffmask, fset)
    {
        // Re-enable the fixed fields for RS1 and RD in the parent's mask
        // This will keep the parent from ignoring SP (encoded in RS1/RD)
        // for getSourceRegs(), getDestRegs(), getOperTypeRegs(), and dasmString()
        fixed_field_mask_ &= ~(Form_CI::getField(Form_CI::idType::RS1).getShiftedMask() |
                               Form_CI::getField(Form_CI::idType::RD).getShiftedMask());
        fixed_field_set_ &= ~((1ull << static_cast<uint32_t>(Form_CI::idType::RS1)) |
                              (1ull << static_cast<uint32_t>(Form_CI::idType::RD)));
    }
};

/**
 * Derivative of Form_CIW extractor with SP source
 * SP is IMPLICITLY encoded
 *
 * Used by c.addi4spn
 */
template<>
class Extractor<Form_CIW_sp> : public Extractor<Form_CIW>
{
public:
    Extractor<Form_CIW_sp>() :
        Extractor<Form_CIW>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_CIW_sp>(ffmask, fset));
    }

    bool isIllop(Opcode icode) const override
    {
        // Don't call getImmediate() here, since we're just checking for 0 and don't
        // need to swizzle
        return extract_(Form_CIW::idType::IMM8, icode & ~fixed_field_mask_) == 0;
    }

    const std::string &getName() const override
    {
        return Form_CIW_sp::getName();
    }

    // TODO Form_CIW (base class) has no sources, so this is overkill...
    uint64_t getSourceRegs(const uint64_t icode) const override
    {
        return (Extractor<Form_CIW>::getSourceRegs(icode) |
                (1ull << REGISTER_SP)) & ~0x1ull; // add SP, remove X0
    }

    //RegListType getSourceList(const Opcode icode, bool suppress_x0 = false) const override
    //{
    //    return {REGISTER_SP};
    //}

    uint64_t getSourceOperTypeRegs(const uint64_t icode,
                                   const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        if (meta->isNoneOperandType(kind)) {
            return 0;
        } else if (meta->isAllOperandType(kind)) {
            return (Extractor<Form_CIW>::getSourceOperTypeRegs(icode, meta, kind) |
                    (1ull << REGISTER_SP)) & ~0x1ull;
        } else {
            uint64_t result = Extractor<Form_CIW>::getSourceOperTypeRegs(icode, meta, kind);
            // RD is the only register we have in this extraction form (c.addi4spn)
            // Use RD's type as the type of the SP source register
            if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind)) {
                result |= (1ull << REGISTER_SP);
                result &= ~0x1ull; // remove x0
            }
            return result;
        }
    }

    //RegListType getSourceOperTypeList(const Opcode icode,
    //                                  const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind,
    //                                  bool suppress_x0 = false) const override
    //{
    //    if (meta->isNoneOperandType(kind)) {
    //        return {};
    //    } else if (meta->isAllOperandType(kind)) {
    //        return getSourceList(icode, suppress_x0);
    //    } else {
    //        if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind)) {
    // RD is the only register we have in this extraction form (c.addi4spn)
    // Use RD's type as the type of the SP source register
    //return {REGISTER_SP};
    //}
    //return {};
    //}
    //}

    OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType& meta, bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        olist.addElement(InstMetaData::OperandFieldID::RS1, meta->getOperandType(InstMetaData::OperandFieldID::RD),
                         REGISTER_SP, false);
        return olist;
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        const uint64_t imm = extract_(Form_CIW::idType::IMM8, icode & ~fixed_field_mask_);
        using R = Swizzler::Range;
        return Swizzler::extract(imm, R{3}, R{2}, R{6,9}, R{4,5});
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const uint64_t icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\t" << extractCompressedRegister_(Form_CIW::idType::RD, icode)
           << ", SP, IMM=0x" << std::hex << getImmediate(icode);
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatCompressedRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_CIW::idType::RD, InstMetaData::OperandFieldID::RD } })
           << ", SP, IMM=0x" << std::hex << getImmediate(icode);
        return ss.str();
    }

private:
    Extractor<Form_CIW_sp>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_CIW>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_CIX extractor for c.andi (hint special cases)
 */
template<>
class Extractor<Form_CIX_andi> : public Extractor<Form_CIX>
{
public:
    Extractor<Form_CIX_andi>() :
        Extractor<Form_CIX>()
    {}

    bool isHint(Opcode icode) const override
    {
        return false;
    }

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_CIX_andi>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_CIX_andi::getName();
    }

    int64_t getSignedOffset(const Opcode icode) const override
    {
        return signExtend_(getImmediate(icode), 5);
    }

private:
    Extractor<Form_CIX_andi>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_CIX>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_CJR extractor with x1 (link) destination
 * IMPLICITLY encoded
 */
template<>
class Extractor<Form_CJALR> : public Extractor<Form_CJR>
{
public:
    Extractor<Form_CJALR>() :
        Extractor<Form_CJR>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_CJALR>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_CJALR::getName();
    }

    uint64_t getDestRegs(const uint64_t) const override
    {
        return (1ull << REGISTER_LINK);
    }

    //RegListType getDestList(const Opcode icode, bool suppress_x0 = false) const override
    //{
    //    return {REGISTER_LINK};
    //}

    // TODO: Form_CJR (base class) has no destinations, this is overkill...
    uint64_t getDestOperTypeRegs(const uint64_t icode,
                                 const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        if (meta->isNoneOperandType(kind)) {
            return 0;
        } else if (meta->isAllOperandType(kind)) {
            return (Extractor<Form_CJR>::getDestOperTypeRegs(icode, meta, kind) |
                    (1ull << REGISTER_LINK)) & ~0x1ull;

        } else {
            uint64_t result = Extractor<Form_CJR>::getDestOperTypeRegs(icode, meta, kind);
            // This form has an implied LINK field (c.jalr)
            // Use the type of RS1 for the type of LINK
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind)) {
                result |= (1ull << REGISTER_LINK);
                result &= ~0x1ull; // remove x0
            }
            return result;
        }
    }

    //RegListType getDestOperTypeList(const Opcode icode,
    //                                const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind,
    //                                bool suppress_x0 = false) const override
    //{
    //    if (meta->isNoneOperandType(kind)) {
    //        return {};
    //    } else if (meta->isAllOperandType(kind)) {
    //        return getDestList(icode, suppress_x0);
    //    } else {
    // This form has an implied LINK field (c.jalr)
    // Use the type of RS1 for the type of LINK
    //if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind)) {
    //    return {REGISTER_LINK};
    //}
    //return {};
    //}
    //}

    OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType& meta, bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        olist.addElement(InstMetaData::OperandFieldID::RD, meta->getOperandType(InstMetaData::OperandFieldID::RS1),
                         REGISTER_LINK, false);
        return olist;
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const uint64_t icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\t " << REGISTER_LINK
           << ", " << extract_(Form_CJR::idType::RS1, icode)
           << ", IMM=0x" << std::hex << getImmediate(icode);
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t x" << REGISTER_LINK
           << ", " << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_CJR::idType::RS1, InstMetaData::OperandFieldID::RS1 } })
           << ", IMM=0x" << std::hex << getImmediate(icode);
        return ss.str();
    }

private:
    Extractor<Form_CJALR>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_CJR>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C2 extractor for c.add (hint special case)
 */
template<>
class Extractor<Form_C2_add> : public Extractor<Form_C2>
{
public:
    Extractor<Form_C2_add>() = default;

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C2_add>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C2_add::getName();
    }

    bool isHint(Opcode icode) const override
    {
        return (getDestRegs(icode) & (1ull << REGISTER_X0)) != 0;
    }

    uint64_t getSourceRegs(const Opcode icode) const override
    {
        return extractUnmaskedIndexBit_(Form_C2::idType::RS1, icode, fixed_field_mask_) |    // Alias for rD
            extractUnmaskedIndexBit_(Form_C2::idType::RS2, icode, fixed_field_mask_);     // Alias for rS
    }

    uint64_t getSourceOperTypeRegs(const Opcode icode,
                                   const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        if (meta->isNoneOperandType(kind)) {
            return 0;
        } else if (meta->isAllOperandType(kind)) {
            return getSourceRegs(icode);
        } else {
            uint64_t result = 0;
            if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind)) {
                result |= extractUnmaskedIndexBit_(Form_C2::idType::RS1, icode, fixed_field_mask_);
            }
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind)) {
                result |= extractUnmaskedIndexBit_(Form_C2::idType::RS2, icode, fixed_field_mask_);
            }
            return result;
        }
    }

    OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                   fixed_field_mask_, Form_C2::idType::RS1, false); // Could also use RD here
        appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                   fixed_field_mask_, Form_C2::idType::RS2, false);
        return olist;
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const Opcode icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\t" << extract_(Form_C2::idType::RD, icode & ~fixed_field_mask_)
           << "," << extract_(Form_C2::idType::RS1, icode & ~fixed_field_mask_)
           << "," << extract_(Form_C2::idType::RS2, icode & ~fixed_field_mask_);
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_C2::idType::RD, InstMetaData::OperandFieldID::RD },
                                   { Form_C2::idType::RS1, InstMetaData::OperandFieldID::RS1},
                                   { Form_C2::idType::RS2, InstMetaData::OperandFieldID::RS2 } });
        return ss.str();
    }

private:
    Extractor<Form_C2_add>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C2>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C2 extractor with implicit x0 source
 * For c.mv extraction
 *
 * NOTE: the x0 implicit source insertion is not being used right now, since dabble
 * rename cannot yet handle a 2-source move. C.MV may be transformed to CMOV in
 * dabble, so the x0 source needs to be "replaced" in CMOV with the destination
 * register (to preserve the old value if the condition fails)
 */
template<>
class Extractor<Form_C2_mv> : public Extractor<Form_C2>
{
public:
    Extractor<Form_C2_mv>() = default;

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C2_mv>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C2_mv::getName();
    }

    bool isHint(Opcode icode) const override
    {
        return (getDestRegs(icode) & (1ull << REGISTER_X0)) != 0;
    }

    uint64_t getSourceRegs(const uint64_t icode) const override
    {
        return (Extractor<Form_C2>::getSourceRegs(icode) |
                (0x1ull << REGISTER_X0));
    }

    uint64_t getSourceOperTypeRegs(const uint64_t icode,
                                   const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        if (meta->isNoneOperandType(kind)) {
            return 0;
        } else if (meta->isAllOperandType(kind)) {
            return (Extractor<Form_C2>::getSourceOperTypeRegs(icode, meta, kind) |
                    (1ull << REGISTER_X0));
        } else {
            uint64_t result = Extractor<Form_C2>::getSourceOperTypeRegs(icode, meta, kind);
            // This form has an implied x0 source register
            // Use the type of RS1 for the type of x0
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind)) {
                result |= (1ull << REGISTER_X0);
            }
            return result;
        }
    }

    OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        olist.addElement(InstMetaData::OperandFieldID::RS1, meta->getOperandType(InstMetaData::OperandFieldID::RS1),
                         REGISTER_X0, false);
        appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                   fixed_field_mask_, Form_C2::idType::RS,
                                   false, suppress_x0);
        return olist;
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const uint64_t icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\t" << extract_(Form_C2::idType::RD, icode)
           << ", 0, " << extract_(Form_C2::idType::RS2, icode); // RS2 == RS
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_C2::idType::RD, InstMetaData::OperandFieldID::RD } })
           << ", x0, "
           << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_C2::idType::RS2, InstMetaData::OperandFieldID::RS2 } });
        return ss.str();
    }

private:
    Extractor<Form_C2_mv>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C2>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C2 extractor for c.slli (hint special case)
 *
 * NOTE: RS1 is an alias for RD
 */
template<>
class Extractor<Form_C2_slli> : public Extractor<Form_C2>
{
public:
    Extractor<Form_C2_slli>() = default;

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C2_slli>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C2_slli::getName();
    }

    bool isHint(Opcode icode) const override
    {
        return ((getDestRegs(icode) & (1ull << REGISTER_X0)) != 0) ||
            (getImmediate(icode) == 0);
    }

    uint64_t getSourceRegs(const Opcode icode) const override
    {
        return extractUnmaskedIndexBit_(Form_C2::idType::RS1, icode, fixed_field_mask_);
    }

    uint64_t getSourceOperTypeRegs(const Opcode icode,
                                   const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        if (meta->isNoneOperandType(kind)) {
            return 0;
        } else if (meta->isAllOperandType(kind)) {
            return getSourceRegs(icode);
        } else {
            uint64_t result = 0;
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind)) {
                result |= extractUnmaskedIndexBit_(Form_C2::idType::RS1, icode, fixed_field_mask_);
            }
            return result;
        }
    }

    OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                   fixed_field_mask_, Form_C2::idType::RS1,
                                   false, suppress_x0);
        return olist;
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        return (extract_(Form_C2::idType::SHAMT1, icode) << 5ull) |
            extract_(Form_C2::idType::SHAMT5, icode);
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const Opcode icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\t" << extract_(Form_C2::idType::RD, icode & ~fixed_field_mask_)
           << "," << extract_(Form_C2::idType::RS1, icode & ~fixed_field_mask_)
           << ", IMM=0x" << std::hex << getImmediate(icode);
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_C2::idType::RD, InstMetaData::OperandFieldID::RD },
                                   { Form_C2::idType::RS1, InstMetaData::OperandFieldID::RS1} })
           << ", IMM=0x" << std::hex << getImmediate(icode);
        return ss.str();
    }

private:
    Extractor<Form_C2_slli>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C2>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C2 extractor with SP source
 * SP is IMPLICITLY encoded
 *
 * This is a base class for C2_sp_load and C2_sp_store
 */
template<>
class Extractor<Form_C2_sp> : public Extractor<Form_C2>
{
public:
    Extractor<Form_C2_sp>() = default;

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C2_sp>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C2_sp::getName();
    }

    uint64_t getSourceRegs(const uint64_t icode) const override
    {
        return (Extractor<Form_C2>::getSourceRegs(icode) |
                (1ull << REGISTER_SP)) & ~0x1ull; // add SP, remove X0
    }

    uint64_t getSourceOperTypeRegs(const uint64_t icode,
                                   const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        if (meta->isNoneOperandType(kind)) {
            return 0;
        } else if (meta->isAllOperandType(kind)) {
            return (Extractor<Form_C2>::getSourceOperTypeRegs(icode, meta, kind) |
                    (1ull << REGISTER_SP)) & ~0x1ull;
        } else {
            uint64_t result = Extractor<Form_C2>::getSourceOperTypeRegs(icode, meta, kind);
            // This form has an implied SP field
            // Use the type of RS1 for the type of SP
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind)) {
                result |= (1ull << REGISTER_SP);
                result &= ~0x1ull; // remove x0
            }
            return result;
        }
    }

    OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        olist.addElement(InstMetaData::OperandFieldID::RS1, meta->getOperandType(InstMetaData::OperandFieldID::RS1),
                         REGISTER_SP, false);
        appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                   fixed_field_mask_, Form_C2::idType::RS,
                                   false, suppress_x0);
        return olist;
    }

    bool hasImmediate() const override
    {
        return true;
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const uint64_t icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\t" << extract_(Form_C2::idType::RD, icode)
           << ", SP, IMM=0x" << std::hex << getImmediate(icode);
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_C2::idType::RD, InstMetaData::OperandFieldID::RD } })
           << ", SP, IMM=0x" << std::hex << getImmediate(icode);
        return ss.str();
    }

protected:
    Extractor<Form_C2_sp>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C2>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C2_sp extractor for LOADS
 */
template<>
class Extractor<Form_C2_sp_load> : public Extractor<Form_C2_sp>
{
public:
    Extractor<Form_C2_sp_load>() :
        Extractor<Form_C2_sp>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C2_sp_load>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C2_sp_load::getName();
    }

    bool isIllop(Opcode icode) const override
    {
        return (getDestRegs(icode) & (1ull << REGISTER_X0)) != 0;
    }

    uint64_t getSourceRegs(const uint64_t icode) const override
    {
        return (1ull << REGISTER_SP);
    }

    uint64_t getSourceAddressRegs(const uint64_t icode) const override
    {
        return getSourceRegs(icode);
    }

    OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        olist.addElement(InstMetaData::OperandFieldID::RS1, meta->getOperandType(InstMetaData::OperandFieldID::RS1),
                         REGISTER_SP, false);
        return olist;
    }

    bool hasImmediate() const override
    {
        return true;
    }

protected:
    Extractor<Form_C2_sp_load>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C2_sp>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C2_sp_load extractor for LOAD WORD
 */
template<>
class Extractor<Form_C2_sp_load_word> : public Extractor<Form_C2_sp_load>
{
public:
    Extractor<Form_C2_sp_load_word>() :
        Extractor<Form_C2_sp_load>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_word>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C2_sp_load_word::getName();
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        const uint64_t imm = (extract_(Form_C2::idType::SHAMT1, icode) << 5ull) |
            extract_(Form_C2::idType::SHAMT5, icode);
        using R = Swizzler::Range;
        // Bit ranges to extract from imm, starting with LSB
        return Swizzler::extract(imm, R{6, 7}, R{2,4}, R{5});
    }

protected:
    Extractor<Form_C2_sp_load_word>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C2_sp_load>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C2_sp_load extractor for LOAD DOUBLE
 */
template<>
class Extractor<Form_C2_sp_load_double> : public Extractor<Form_C2_sp_load>
{
public:
    Extractor<Form_C2_sp_load_double>() :
        Extractor<Form_C2_sp_load>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_double>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C2_sp_load_double::getName();
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        const uint64_t imm = (extract_(Form_C2::idType::SHAMT1, icode) << 5ull) |
            extract_(Form_C2::idType::SHAMT5, icode);
        using R = Swizzler::Range;
        // Bit ranges to extract from imm, starting with LSB
        return Swizzler::extract(imm, R{6, 8}, R{3,4}, R{5});
    }

protected:
    Extractor<Form_C2_sp_load_double>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C2_sp_load>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C2_sp_load extractor for FP loads (SINGLE PRECISION)
 */
template<>
class Extractor<Form_C2_sp_load_float_single> : public Extractor<Form_C2_sp_load_word>
{
public:
    Extractor<Form_C2_sp_load_float_single>() :
        Extractor<Form_C2_sp_load_word>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_float_single>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C2_sp_load_float_single::getName();
    }

    bool isIllop(Opcode) const override
    {
        return false;
    }

private:
    Extractor<Form_C2_sp_load_float_single>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C2_sp_load_word>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C2_sp_load extractor for FP loads (DOUBLE PRECISION)
 */
template<>
class Extractor<Form_C2_sp_load_float_double> : public Extractor<Form_C2_sp_load_double>
{
public:
    Extractor<Form_C2_sp_load_float_double>() :
        Extractor<Form_C2_sp_load_double>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_float_double>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C2_sp_load_float_double::getName();
    }

    bool isIllop(Opcode) const override
    {
        return false;
    }

private:
    Extractor<Form_C2_sp_load_float_double>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C2_sp_load_double>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C2_sp_store extractor for STORE WORD
 */
template<>
class Extractor<Form_C2_sp_store_word> : public Extractor<Form_C2_sp_store>
{
public:
    Extractor<Form_C2_sp_store_word>() :
        Extractor<Form_C2_sp_store>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C2_sp_store_word>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C2_sp_store_word::getName();
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        const uint64_t imm = extract_(Form_C2_sp_store::idType::IMM, icode);
        using R = Swizzler::Range;
        // Bit ranges to extract from imm, starting with LSB
        return Swizzler::extract(imm, R{6, 7}, R{2,5});
    }

private:
    Extractor<Form_C2_sp_store_word>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C2_sp_store>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_C2_sp_store extractor for STORE DOUBLE
 */
template<>
class Extractor<Form_C2_sp_store_double> : public Extractor<Form_C2_sp_store>
{
public:
    Extractor<Form_C2_sp_store_double>() :
        Extractor<Form_C2_sp_store>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_C2_sp_store_double>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_C2_sp_store_double::getName();
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        const uint64_t imm = extract_(Form_C2_sp_store::idType::IMM, icode);
        using R = Swizzler::Range;
        // Bit ranges to extract from imm, starting with LSB
        return Swizzler::extract(imm, R{6, 8}, R{3,5});
    }

private:
    Extractor<Form_C2_sp_store_double>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_C2_sp_store>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_CI_rD_only extractor with SP source and destination
 * SP is explicitly encoded
 */
template<>
class Extractor<Form_CI_rD_shifted> : public Extractor<Form_CI_rD_only>
{
public:
    Extractor<Form_CI_rD_shifted>() :
        Extractor<Form_CI_rD_only>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_CI_rD_shifted>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_CI_rD_shifted::getName();
    }

    bool isIllop(Opcode icode) const override
    {
        return getImmediate(icode) == 0;
    }

    uint64_t getSourceRegs(const Opcode) const override
    {
        return 0;
    }

    //RegListType getSourceList(const Opcode, bool suppress_x0 = false) const override
    //{
    //    return {};
    //}

    uint64_t getSourceOperTypeRegs(const Opcode,
                                   const InstMetaData::PtrType&, InstMetaData::OperandTypes) const override
    {
        return 0;
    }

    //RegListType getSourceOperTypeList(const Opcode,
    //                                  const InstMetaData::PtrType&, InstMetaData::OperandTypes,
    //                                  bool suppress_x0 = false) const override
    //{
    //    return {};
    //}

    OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType&, bool suppress_x0 = false) const override
    {
        return {};
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        return Extractor<Form_CI_rD_only>::getImmediate(icode) << 12ull;
    }

    int64_t getSignedOffset(const Opcode icode) const override
    {
        return signExtend_(getImmediate(icode), 17);
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
                                   // overloads are considered
    std::string dasmString(const std::string &mnemonic, const Opcode icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\t" << extract_(Form_CI_rD_only::idType::RD, icode & ~fixed_field_mask_)
           << ", +0x" << std::hex << getSignedOffset(icode);
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_CI_rD_only::idType::RD, InstMetaData::OperandFieldID::RD } })
           << ", +0x" << std::hex << getSignedOffset(icode);
        return ss.str();
    }

private:
    Extractor<Form_CI_rD_shifted>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_CI_rD_only>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_VF_mem extractor for vector loads
 */
template<>
class Extractor<Form_V_load> : public Extractor<Form_VF_mem>
{
public:
    Extractor<Form_V_load>() :
        Extractor<Form_VF_mem>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_V_load>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_V_load::getName();
    }

private:
    Extractor<Form_V_load>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_VF_mem>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_VF_mem extractor for vector stores
 */
template<>
class Extractor<Form_V_store> : public Extractor<Form_VF_mem>
{
public:
    Extractor<Form_V_store>() :
        Extractor<Form_VF_mem>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_V_store>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_V_store::getName();
    }

    uint64_t getSourceRegs(const Opcode icode) const override
    {
        return extractUnmaskedIndexBit_(Form_VF_mem::idType::RS1, icode, fixed_field_mask_) |
            extractUnmaskedIndexBit_(Form_VF_mem::idType::RS2, icode, fixed_field_mask_) |
            extractUnmaskedIndexBit_(Form_VF_mem::idType::RS3, icode, fixed_field_mask_);
    }

    uint64_t getSourceAddressRegs(const Opcode icode) const override
    {
        return extractUnmaskedIndexBit_(Form_VF_mem::idType::RS1, icode, fixed_field_mask_) |
            extractUnmaskedIndexBit_(Form_VF_mem::idType::RS2, icode, fixed_field_mask_);
    }

    uint64_t getSourceDataRegs(const Opcode icode) const override
    {
        return extractUnmaskedIndexBit_(Form_VF_mem::idType::RS3, icode, fixed_field_mask_);
    }

    uint64_t getDestRegs(const Opcode icode) const override
    {
        return 0;
    }

    uint64_t getSourceOperTypeRegs(const Opcode icode,
                                   const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        if (meta->isNoneOperandType(kind)) {
            return 0;
        } else if (meta->isAllOperandType(kind)) {
            return getSourceRegs(icode);
        } else {
            uint64_t result = 0;
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind)) {
                result |= extractUnmaskedIndexBit_(Form_VF_mem::idType::RS1, icode, fixed_field_mask_);
            }
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind)) {
                result |= extractUnmaskedIndexBit_(Form_VF_mem::idType::RS2, icode, fixed_field_mask_);
            }
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS3, kind)) {
                result |= extractUnmaskedIndexBit_(Form_VF_mem::idType::RS3, icode, fixed_field_mask_);
            }
            return result;
        }
    }

    uint64_t getDestOperTypeRegs(const Opcode icode,
                                 const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        return 0;
    }

    OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                   fixed_field_mask_, Form_VF_mem::idType::RS1,
                                   false, suppress_x0);
        appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                   fixed_field_mask_, Form_VF_mem::idType::RS2,
                                   false, suppress_x0);
        appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS3,
                                   fixed_field_mask_, Form_VF_mem::idType::RS3,
                                   true, suppress_x0);
        return olist;
    }

    OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                   bool suppress_x0 = false) const override
    {
        return {};
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << "v" << extract_(Form_VF_mem::idType::RS3, icode & ~fixed_field_mask_)
           << ",x" << extract_(Form_VF_mem::idType::RS1, icode & ~fixed_field_mask_);
        if (!isMaskedField_(Form_VF_mem::idType::RS2, fixed_field_mask_)) {
            ss << ",v" << extract_(Form_VF_mem::idType::RS2, icode);
        }
        // Show the vm operand if masking mode is on
        if (!isMaskedField_(Form_VF_mem::idType::VM, fixed_field_mask_)) {
            if (!extract_(Form_VF_mem::idType::VM, icode)) {
                ss << ",v0.t";
            }
        }
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_VF_mem::idType::RS3, InstMetaData::OperandFieldID::RS3 },
                                   { Form_VF_mem::idType::RS1, InstMetaData::OperandFieldID::RS1},
                                   { Form_VF_mem::idType::RS2, InstMetaData::OperandFieldID::RS2 } });
        // Show the vm operand if masking mode is on
        if (!isMaskedField_(Form_VF_mem::idType::VM, fixed_field_mask_)) {
            if (!extract_(Form_VF_mem::idType::VM, icode)) {
                ss << ",v0.t";
            }
        }
        return ss.str();
    }

private:
    Extractor<Form_V_store>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_VF_mem>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_V extractor for vector-immediate (signed) instructions
 */
template<>
class Extractor<Form_V_simm> : public Extractor<Form_V>
{
public:
    Extractor<Form_V_simm>() :
        Extractor<Form_V>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_V_simm>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_V_simm::getName();
    }

    uint64_t getSourceRegs(const Opcode icode) const override
    {
        return extractUnmaskedIndexBit_(Form_V::idType::RS2, icode, fixed_field_mask_);
    }

    uint64_t getSourceOperTypeRegs(const Opcode icode,
                                   const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        if (meta->isNoneOperandType(kind)) {
            return 0;
        } else if (meta->isAllOperandType(kind)) {
            return getSourceRegs(icode);
        } else {
            uint64_t result = 0;
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind)) {
                result |= extractUnmaskedIndexBit_(Form_V::idType::RS2, icode, fixed_field_mask_);
            }
            return result;
        }
    }

    OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                   fixed_field_mask_, Form_V::idType::RS2,
                                   false, suppress_x0);
        return olist;
    }

    // TODO: add VM special fields
    uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
    {
        switch(sfid) {
            case SpecialField::VM:
                if (isMaskedField_(Form_V::idType::VM, fixed_field_mask_)) {
                    throw UnsupportedExtractorSpecialFieldID("VM", icode);
                } else {
                    return extract_(Form_V::idType::VM, icode);
                }
            case SpecialField::AQ:
            case SpecialField::AVL:
            case SpecialField::CSR:
            case SpecialField::FM:
            case SpecialField::NF:
            case SpecialField::PRED:
            case SpecialField::RL:
            case SpecialField::RM:
            case SpecialField::SUCC:
            case SpecialField::WD:
            case SpecialField::__N:
                return ExtractorBase::getSpecialField(sfid, icode);
        }
        return 0;
    }

    uint64_t getImmediate(const Opcode icode) const override
    {
        return extract_(Form_V::idType::SIMM5, icode & ~fixed_field_mask_);
    }

    int64_t getSignedOffset(const Opcode icode) const override
    {
        return signExtend_(getImmediate(icode), 4);
    }

    bool hasImmediate() const override
    {
        return true;
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\tv" << extract_(Form_V::idType::RD, icode & ~fixed_field_mask_)
           << ",v" << extract_(Form_V::idType::RS2, icode & ~fixed_field_mask_)
           << ",0x" << std::hex << getSignedOffset(icode);
        // Show the vm operand if masking mode is on
        if (!isMaskedField_(Form_V::idType::VM, fixed_field_mask_)) {
            if (!extract_(Form_V::idType::VM, icode)) {
                ss << ",v0.t";
            }
        }
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_V::idType::RD, InstMetaData::OperandFieldID::RD },
                                   { Form_V::idType::RS2, InstMetaData::OperandFieldID::RS2 } })
           << "," << std::dec << getSignedOffset(icode);
        // Show the vm operand if masking mode is on
        if (!isMaskedField_(Form_V::idType::VM, fixed_field_mask_)) {
            if (!extract_(Form_V::idType::VM, icode)) {
                ss << ",v0.t";
            }
        }
        return ss.str();
    }

private:
    Extractor<Form_V_simm>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_V>(ffmask, fset)
    {}
};

/**
 * Derivative of Form_V extractor for vector-immediate (signed) instructions
 */
template<>
class Extractor<Form_V_op> : public Extractor<Form_V>
{
public:
    Extractor<Form_V_op>() :
        Extractor<Form_V>()
    {}

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_V_op>(ffmask, fset));
    }

    const std::string &getName() const override
    {
        return Form_V_op::getName();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\tv" << extract_(Form_V::idType::RD, icode & ~fixed_field_mask_)
           << ",v" << extract_(Form_V::idType::RS2, icode & ~fixed_field_mask_)
           << ",v" << extract_(Form_V::idType::RS1, icode & ~fixed_field_mask_);
        // Show the vm operand if masking mode is on
        if (!isMaskedField_(Form_V::idType::VM, fixed_field_mask_)) {
            if (!extract_(Form_V::idType::VM, icode)) {
                ss << ",v0.t";
            }
        }
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_V::idType::RD, InstMetaData::OperandFieldID::RD },
                                   { Form_V::idType::RS2, InstMetaData::OperandFieldID::RS2 },
                                   { Form_V::idType::RS1, InstMetaData::OperandFieldID::RS1 } });
        // Show the vm operand if masking mode is on
        if (!isMaskedField_(Form_V::idType::VM, fixed_field_mask_)) {
            if (!extract_(Form_V::idType::VM, icode)) {
                ss << ",v0.t";
            }
        }
        return ss.str();
    }

private:
    Extractor<Form_V_op>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_V>(ffmask, fset)
    {}
};

/**
 * HV Load-Form Extractor
 */
template<>
class Extractor<Form_HV_load> : public Extractor<Form_R>
{
public:
    Extractor() = default;

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_HV_load>(ffmask, fset));
    }

    uint64_t getSourceAddressRegs(const Opcode icode) const override
    {
        return getSourceRegs(icode);
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
    // overloads are considered
    std::string dasmString(const std::string &mnemonic, const Opcode icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\t" << extract_(Form_R::idType::RD, icode & ~fixed_field_mask_)
           << "," << extract_(Form_R::idType::RS1, icode & ~fixed_field_mask_);
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_R::idType::RD, InstMetaData::OperandFieldID::RD },
                                   { Form_R::idType::RS1, InstMetaData::OperandFieldID::RS1} });
        return ss.str();
    }

protected:
    Extractor<Form_HV_load>(const uint64_t ffmask, const uint64_t fset) :
        Extractor<Form_R>(ffmask, fset)
    {}
};

/**
 * HV Store-Form Extractor
 */
template<>
class Extractor<Form_HV_store> : public ExtractorBase<Form_R>
{
public:
    Extractor() = default;

    ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask, const uint64_t fset) const override
    {
        return ExtractorIF::PtrType(new Extractor<Form_HV_store>(ffmask, fset));
    }

    uint64_t getSourceRegs(const Opcode icode) const override
    {
        return extractUnmaskedIndexBit_(Form_R::idType::RS1, icode, fixed_field_mask_) |
               extractUnmaskedIndexBit_(Form_R::idType::RS2, icode, fixed_field_mask_);
    }

    uint64_t getSourceOperTypeRegs(const Opcode icode,
                                   const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const override
    {
        if (meta->isNoneOperandType(kind)) {
            return 0;
        } else if (meta->isAllOperandType(kind)) {
            return getSourceRegs(icode);
        } else {
            uint64_t result = 0;
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind)) {
                result |= extractUnmaskedIndexBit_(Form_R::idType::RS1, icode, fixed_field_mask_);
            }
            if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind)) {
                result |= extractUnmaskedIndexBit_(Form_R::idType::RS2, icode, fixed_field_mask_);
            }
            return result;
        }
    }

    uint64_t getSourceAddressRegs(const Opcode icode) const override
    {
        return extractUnmaskedIndexBit_(Form_R::idType::RS1, icode, fixed_field_mask_);
    }

    uint64_t getSourceDataRegs(const Opcode icode) const override
    {
        return extractUnmaskedIndexBit_(Form_R::idType::RS2, icode, fixed_field_mask_);
    }

    OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        OperandInfo olist;
        appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                   fixed_field_mask_, Form_R::idType::RS1,
                                   false, suppress_x0);
        appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                   fixed_field_mask_, Form_R::idType::RS2,
                                   true, suppress_x0);
        return olist;
    }

    using ExtractorIF::dasmString; // tell the compiler all dasmString
    // overloads are considered
    std::string dasmString(const std::string &mnemonic, const Opcode icode) const override
    {
        std::stringstream ss;
        ss << mnemonic
           << "\t" << extract_(Form_R::idType::RS2, icode & ~fixed_field_mask_)  // source data
           << "," << extract_(Form_R::idType::RS1, icode & ~fixed_field_mask_);   // source address
        return ss.str();
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        std::stringstream ss;
        ss << mnemonic << "\t"
           << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                 { { Form_R::idType::RS2, InstMetaData::OperandFieldID::RS2 },
                                   { Form_R::idType::RS1, InstMetaData::OperandFieldID::RS1} });
        return ss.str();
    }

private:
    Extractor<Form_HV_store>(const uint64_t ffmask, const uint64_t fset) :
        fixed_field_mask_(ffmask)
    {}

    uint64_t fixed_field_mask_ = 0;
};

} // namespace mavis
