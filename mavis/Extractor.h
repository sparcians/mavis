#pragma once

#include "ExtractorIF.h"
#include "Swizzler.hpp"
#include <ostream>
#include <sstream>
#include <string>
#include <memory>

namespace mavis {

// TODO: Instructions will need to extract arbitrary fields in the Form
// TODO: Flesh out all the operand type interfaces
// TODO: Need a separate extractor for FCVT

/**
 * Extractor Base Class -- provides extractor convenience methods
 * for derived classes
 */
template<typename FormType>
class ExtractorBase : public ExtractorIF
{
public:
    const std::string& getName() const override
    {
        return FormType::getName();
    }

    bool isIllop(Opcode) const override
    {
        return false;
    }

    bool isHint(Opcode) const override
    {
        return false;
    }

    // Default implementation returns 0 bitset
    uint64_t getSourceRegs(const Opcode) const override
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

    // Default implementation returns 0 bitset
    uint64_t
    getSourceOperTypeRegs(const Opcode, const InstMetaData::PtrType &, InstMetaData::OperandTypes) const override
    {
        return 0;
    }

    // Default implementation returns 0 bitset
    uint64_t
    getDestOperTypeRegs(const Opcode, const InstMetaData::PtrType &, InstMetaData::OperandTypes) const override
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

    // Default implementation returns 0
    uint64_t getImmediate(const Opcode icode) const override
    {
        return 0;
    }

    // Default implementation returns 0
    int64_t getSignedOffset(const Opcode icode) const override
    {
        return 0;
    }

    uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
    {
        switch(sfid) {
            case SpecialField::AQ:
                throw UnsupportedExtractorSpecialFieldID("AQ", icode);
            case SpecialField::AVL:
                throw UnsupportedExtractorSpecialFieldID("AVL", icode);
            case SpecialField::CSR:
                throw UnsupportedExtractorSpecialFieldID("CSR", icode);
            case SpecialField::FM:
                throw UnsupportedExtractorSpecialFieldID("FM", icode);
            case SpecialField::NF:
                throw UnsupportedExtractorSpecialFieldID("NF", icode);
            case SpecialField::PRED:
                throw UnsupportedExtractorSpecialFieldID("PRED", icode);
            case SpecialField::RL:
                throw UnsupportedExtractorSpecialFieldID("RL", icode);
            case SpecialField::RM:
                throw UnsupportedExtractorSpecialFieldID("RM", icode);
            case SpecialField::SUCC:
                throw UnsupportedExtractorSpecialFieldID("SUCC", icode);
            case SpecialField::VM:
                throw UnsupportedExtractorSpecialFieldID("VM", icode);
            case SpecialField::WD:
                throw UnsupportedExtractorSpecialFieldID("WD", icode);
            case SpecialField::__N:
                throw InvalidExtractorSpecialFieldID("__N", icode);
        }
        return 0;
    }

    bool hasImmediate() const override
    {
        return FormType::hasImmediate();
    }

    // TODO: If we need annotations for disassembly for normal extractors, we
    // can add it here. See the implementation in ExtractorDirectInfoBase as a
    // reference
    void dasmAnnotate(const std::string& txt) override
    {
        assert(false);
    }
    const std::string& getDasmAnnotation() const override
    {
        assert(false);
        static const std::string unknown_str("<UNKNOWN>");
        return unknown_str;
    }

    void print(std::ostream& os) const override
    {
        os << "Extractor '" << getName() << "'"
           << std::endl;
    }

protected:
    static inline uint64_t extract_(const typename FormType::idType fid, const Opcode icode)
    {
        return FormType::getField(fid).extract(icode);
    }

    static inline bool isFixedField_(const typename FormType::idType fid, const uint64_t fset)
    {
        return ((1ull << static_cast<uint64_t>(fid)) & fset) != 0;
    }

    static inline bool isMaskedField_(const typename FormType::idType fid, const uint64_t mask)
    {
        return (FormType::getField(fid).getShiftedMask() & mask) != 0ull;
    }

    // TODO: Deprecate all uses of fixed_field_set! It's DANGEROUS
    // The problem with fixed_field_set is that it is generated from the DECODE form, using the
    // enum definition in that form's class. When we're extracting, we may use an "xform" which likely
    // has an incompatible field enum (where field id enum values are different between the form and xform)
    static inline uint64_t
    extractIndexBit_(const typename FormType::idType fid, const Opcode icode, const uint64_t fset)
    {
        if (!isFixedField_(fid, fset)) {
            return 1ull << extract_(fid, icode);
        }
        return 0;
    }

    static inline uint64_t
    extractUnmaskedIndexBit_(const typename FormType::idType fid, const Opcode icode, const uint64_t mask)
    {
        if (!isMaskedField_(fid, mask)) {
            return 1ull << extract_(fid, icode);
        }
        return 0;
    }

    static inline uint64_t extractCompressedRegister_(const typename FormType::idType fid, const Opcode icode)
    {
        return FormType::getField(fid).extract(icode) + 8; // Compressed registers are offset by 8
    }

    static inline uint64_t
    extractCompressedIndexBit_(const typename FormType::idType fid, const Opcode icode, const uint64_t fset)
    {
        if (!isFixedField_(fid, fset)) {
            return 1ull << extractCompressedRegister_(fid, icode);
        }
        return 0;
    }

    static inline uint64_t
    extractUnmaskedCompressedIndexBit_(const typename FormType::idType fid, const Opcode icode, const uint64_t mask)
    {
        if (!isMaskedField_(fid, mask)) {
            return 1ull << extractCompressedRegister_(fid, icode);
        }
        return 0;
    }

    // TODO: Deprecate all uses of fixed_field_set! It's DANGEROUS
    // The problem with fixed_field_set is that it is generated from the DECODE form, using the
    // enum definition in that form's class. When we're extracting, we may use an "xform" which likely
    // has an incompatible field enum (where field id enum values are different between the form and xform)
    static inline void appendOperandInfo_(OperandInfo& olist, Opcode icode,
                                          const InstMetaData::PtrType &meta, InstMetaData::OperandFieldID mid,
                                          const uint64_t fset, typename FormType::idType fid,
                                          bool is_store_data = false, bool suppress_x0 = false)
    {
        if (!isFixedField_(fid, fset)) {
            uint64_t reg = extract_(fid, icode);
            if (!suppress_x0 || (reg != REGISTER_X0)) {
                olist.addElement(mid, meta->getOperandType(mid), reg, is_store_data);
            }
        }
    }

    static inline void appendUnmaskedOperandInfo_(OperandInfo& olist, Opcode icode,
                                          const InstMetaData::PtrType &meta, InstMetaData::OperandFieldID mid,
                                          const uint64_t mask, typename FormType::idType fid,
                                          bool is_store_data = false, bool suppress_x0 = false)
    {
        if (!isMaskedField_(fid, mask)) {
            uint64_t reg = extract_(fid, icode);
            if (!suppress_x0 || (reg != REGISTER_X0)) {
                olist.addElement(mid, meta->getOperandType(mid), reg, is_store_data);
            }
        }
    }

    static inline void appendCompressedOperandInfo_(OperandInfo& olist, Opcode icode,
                                                    const InstMetaData::PtrType &meta, InstMetaData::OperandFieldID mid,
                                                    const uint64_t fset, typename FormType::idType fid,
                                                    bool is_store_data = false)
    {
        if (!isFixedField_(fid, fset)) {
            uint64_t reg = extractCompressedRegister_(fid, icode);
            olist.addElement(mid, meta->getOperandType(mid), reg, is_store_data);
        }
    }

    static inline void appendUnmaskedCompressedOperandInfo_(OperandInfo& olist, Opcode icode,
                                                    const InstMetaData::PtrType &meta, InstMetaData::OperandFieldID mid,
                                                    const uint64_t mask, typename FormType::idType fid,
                                                    bool is_store_data = false)
    {
        if (!isMaskedField_(fid, mask)) {
            uint64_t reg = extractCompressedRegister_(fid, icode);
            olist.addElement(mid, meta->getOperandType(mid), reg, is_store_data);
        }
    }

    static inline int64_t signExtend_(const uint64_t uval, uint32_t sign_bit_pos)
    {
        assert(sign_bit_pos < (sizeof(uval) * 8));
        const uint32_t sign_shift = (sizeof(uval) * 8) - sign_bit_pos - 1;
        return static_cast<int64_t>(uval) << sign_shift >> sign_shift;
    }

    static inline std::string dasmFormatReg_(const InstMetaData::PtrType& meta,
                                             InstMetaData::OperandFieldID mid,
                                             uint32_t regnum)
    {
        std::stringstream    ss;
        switch(meta->getOperandType(mid))
        {
            case InstMetaData::OperandTypes::WORD:
            case InstMetaData::OperandTypes::LONG:
                ss << "x" << std::dec << regnum;
                break;
            case InstMetaData::OperandTypes::SINGLE:
            case InstMetaData::OperandTypes::DOUBLE:
            case InstMetaData::OperandTypes::QUAD:
                ss << "f" << std::dec << regnum;
                break;
            case InstMetaData::OperandTypes::VECTOR:
                ss << "v" << std::dec << regnum;
                break;
            case InstMetaData::OperandTypes::NONE:
                ss << std::dec << regnum;
                break;
        }
        return ss.str();
    }

    struct RegType_ {
        typename FormType::idType     fid;
        InstMetaData::OperandFieldID  mid;
    };
    typedef std::vector<RegType_> RegTypeList_;

    static inline std::string dasmFormatRegList_(const InstMetaData::PtrType& meta,
                                                 const Opcode icode,
                                                 uint64_t fixed_field_mask,
                                                 const RegTypeList_& rtlist)
    {
        bool first = true;
        std::stringstream ss;
        for (const auto& r : rtlist) {
            if (!isMaskedField_(r.fid, fixed_field_mask)) {
                if (!first) {
                    ss << ",";
                } else {
                    first = false;
                }
                ss << dasmFormatReg_(meta, r.mid, extract_(r.fid, icode));
            }
        }
        return ss.str();
    }

    static inline std::string dasmFormatCompressedRegList_(const InstMetaData::PtrType& meta,
                                                           const Opcode icode,
                                                           uint64_t fixed_field_mask,
                                                           const RegTypeList_& rtlist)
    {
        bool first = true;
        std::stringstream ss;
        for (const auto& r : rtlist) {
            if (!isMaskedField_(r.fid, fixed_field_mask)) {
                if (!first) {
                    ss << ",";
                } else {
                    first = false;
                }
                ss << dasmFormatReg_(meta, r.mid, extractCompressedRegister_(r.fid, icode));
            }
        }
        return ss.str();
    }
};

/**
 * Concrete Extractors
 * @tparam T Form type identifier (const char)
 */
// Only specialized Forms will compile
template<typename FormType>
class Extractor;

/**
 * ExtractorIF ostream insertion operator
 * @param os
 * @param f
 * @return
 */
inline std::ostream &operator<<(std::ostream &os, const ExtractorIF::PtrType &f)
{
    f->print(os);
    return os;
}

} // namespace mavis
