#pragma once

#include "InstMetaData.h"
#include "OperandInfo.hpp"
#include "DecoderConsts.h"
#include "Swizzler.hpp"
#include <map>
#include <algorithm>

namespace mavis
{
    /**
     * ExtractorIF: Interface to Extractors
     */
    class ExtractorIF
    {
        friend class ExtractorWrap;

      public:
        using OpcodeFieldValueType = OperandInfo::OpcodeFieldValueType;

        typedef std::shared_ptr<ExtractorIF> PtrType;
        typedef std::vector<OpcodeFieldValueType> RegListType;
        typedef std::vector<uint32_t> ValueListType;

        static inline const std::string & getSpecialFieldName(InstMetaData::SpecialField sid)
        {
            return InstMetaData::sf_to_string_map.at(sid);
        }

        virtual ~ExtractorIF() = default;

        virtual ExtractorIF::PtrType specialCaseClone(uint64_t ffmask, uint64_t fset) const = 0;

        virtual std::string getName() const = 0;

        virtual bool isIllop(Opcode icode) const = 0;

        virtual bool isHint(Opcode icode) const = 0;

        virtual uint64_t getSourceRegs(Opcode icode) const = 0;

        virtual uint64_t getSourceAddressRegs(Opcode icode) const = 0;

        virtual uint64_t getSourceDataRegs(Opcode icode) const = 0;

        virtual uint64_t getDestRegs(Opcode icode) const = 0;

        virtual uint64_t getSourceOperTypeRegs(Opcode icode, const InstMetaData::PtrType & meta,
                                               InstMetaData::OperandTypes kind) const = 0;

        virtual uint64_t getDestOperTypeRegs(Opcode icode, const InstMetaData::PtrType & meta,
                                             InstMetaData::OperandTypes kind) const = 0;

        // TODO: Remove this, unless used outside of DecodedInstInfo
        virtual uint64_t getOperTypeRegs(Opcode icode, const InstMetaData::PtrType & meta,
                                         InstMetaData::OperandTypes kind) const
        {
            return getSourceOperTypeRegs(icode, meta, kind)
                   | getDestOperTypeRegs(icode, meta, kind);
        }

        virtual OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                                 bool suppress_x0 = false) const = 0;

        virtual OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                               bool suppress_x0 = false) const = 0;

        virtual ImmediateType getImmediateType() const = 0;

        bool hasImmediate() const { return getImmediateType() != ImmediateType::NONE; }

        virtual uint64_t getImmediate(Opcode icode) const = 0;

        // Just an alias to getImmediate() for symmetry with getSignedOffset()
        uint64_t getUnsignedOffset(Opcode icode) const { return getImmediate(icode); }

        // TODO: Consider a default implementation for this, by
        // moving the ExtractorBase version here, and getting rid
        // of the identical versions for ExtractorDirectInfo and
        // ExtractorTraceInfo. Do we really need separate versions anymore?
        virtual int64_t getSignedOffset(Opcode icode) const = 0;

        // Special fields are cached in DecodedInstInfo
        virtual InstMetaData::SpecialFieldsMap getSpecialFields(Opcode icode, const InstMetaData::PtrType & meta) const = 0;

        virtual std::string dasmString(const std::string & mnemonic, Opcode icode) const = 0;

        // This version of dasmString can be overridden in the derived classes to take advantage
        // of operand type information in the InstMetaData object (e.g. for providing the correct
        // register prefix characters such as "x" for 64-bit int, "f" for float, and "v" for
        // vector). If unimplemented, we revert to the standard call to dasmString()
        virtual std::string dasmString(const std::string & mnemonic, Opcode icode,
                                       const InstMetaData::PtrType &) const
        {
            return dasmString(mnemonic, icode);
        }

        virtual void dasmAnnotate(const std::string & txt) = 0;
        virtual const std::string & getDasmAnnotation() const = 0;

        virtual void print(std::ostream & os) const = 0;
    };

    // TODO: Instructions will need to extract arbitrary fields in the Form
    // TODO: Flesh out all the operand type interfaces
    // TODO: Need a separate extractor for FCVT

    /**
     * Extractor Base Class -- provides extractor convenience methods
     * for derived classes
     */
    template <typename FormType> class ExtractorBase : public ExtractorIF
    {
      public:
        std::string getName() const override { return FormType::name; }

        bool isIllop(Opcode) const override { return false; }

        bool isHint(Opcode) const override { return false; }

        // Default implementation returns 0 bitset
        uint64_t getSourceRegs(const Opcode) const override { return 0; }

        // Default implementation returns 0 bitset
        uint64_t getSourceAddressRegs(const Opcode) const override { return 0; }

        // Default implementation returns 0 bitset
        uint64_t getSourceDataRegs(const Opcode) const override { return 0; }

        // Default implementation returns 0 bitset
        uint64_t getDestRegs(const Opcode) const override { return 0; }

        // Default implementation returns 0 bitset
        uint64_t getSourceOperTypeRegs(const Opcode, const InstMetaData::PtrType &,
                                       InstMetaData::OperandTypes) const override
        {
            return 0;
        }

        // Default implementation returns 0 bitset
        uint64_t getDestOperTypeRegs(const Opcode, const InstMetaData::PtrType &,
                                     InstMetaData::OperandTypes) const override
        {
            return 0;
        }

        // Default implementation returns empty list
        OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType &,
                                         bool suppress_x0 = false) const override
        {
            return {};
        }

        // Default implementation returns empty list
        OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType &,
                                       bool suppress_x0 = false) const override
        {
            return {};
        }

        // Default implementation returns 0
        uint64_t getImmediate(const Opcode icode) const override { return 0; }

        int64_t getSignedOffset(const Opcode icode) const override { return getImmediate(icode); }

        InstMetaData::SpecialFieldsMap getSpecialFields(Opcode icode, const InstMetaData::PtrType &) const override
        {
            return InstMetaData::SpecialFieldsMap();
        }

        ImmediateType getImmediateType() const override { return FormType::immediate_type; }

        // TODO: If we need annotations for disassembly for normal extractors, we
        // can add it here. See the implementation in ExtractorDirectInfoBase as a
        // reference
        void dasmAnnotate(const std::string &) override { assert(false); }

        const std::string & getDasmAnnotation() const override
        {
            static const std::string empty_string;
            assert(false);
            return empty_string;
        }

        void print(std::ostream & os) const override { os << "Extractor '" << getName() << "'"; }

      protected:
        ExtractorBase() = default;

        ExtractorBase(uint64_t fixed_field_mask) : fixed_field_mask_(fixed_field_mask) {}

        // Common to all Forms
        const uint64_t fixed_field_mask_ = 0;

        static inline uint64_t extract_(const typename FormType::idType fid, const Opcode icode)
        {
            return FormType::fields[fid].extract(icode);
        }

        static inline bool isFixedField_(const typename FormType::idType fid, const uint64_t fset)
        {
            return ((1ull << static_cast<uint64_t>(fid)) & fset) != 0;
        }

        static inline bool isMaskedField_(const typename FormType::idType fid, const uint64_t mask)
        {
            return (FormType::fields[fid].getShiftedMask() & mask) != 0ull;
        }

        // TODO: Deprecate all uses of fixed_field_set! It's DANGEROUS
        // The problem with fixed_field_set is that it is generated from the DECODE form, using the
        // enum definition in that form's class. When we're extracting, we may use an "xform" which
        // likely has an incompatible field enum (where field id enum values are different between
        // the form and xform)
        static inline uint64_t extractIndexBit_(const typename FormType::idType fid,
                                                const Opcode icode, const uint64_t fset)
        {
            if (!isFixedField_(fid, fset))
            {
                return 1ull << extract_(fid, icode);
            }
            return 0;
        }

        static inline uint64_t extractUnmaskedIndexBit_(const typename FormType::idType fid,
                                                        const Opcode icode, const uint64_t mask)
        {
            if (!isMaskedField_(fid, mask))
            {
                return 1ull << extract_(fid, icode);
            }
            return 0;
        }

        static inline uint64_t extractCompressedRegister_(const typename FormType::idType fid,
                                                          const Opcode icode)
        {
            return FormType::fields[fid].extract(icode) + 8; // Compressed registers are offset by 8
        }

        static inline uint64_t extractCompressedIndexBit_(const typename FormType::idType fid,
                                                          const Opcode icode, const uint64_t fset)
        {
            if (!isFixedField_(fid, fset))
            {
                return 1ull << extractCompressedRegister_(fid, icode);
            }
            return 0;
        }

        static inline uint64_t
        extractUnmaskedCompressedIndexBit_(const typename FormType::idType fid, const Opcode icode,
                                           const uint64_t mask)
        {
            if (!isMaskedField_(fid, mask))
            {
                return 1ull << extractCompressedRegister_(fid, icode);
            }
            return 0;
        }

        // TODO: Deprecate all uses of fixed_field_set! It's DANGEROUS
        // The problem with fixed_field_set is that it is generated from the DECODE form, using the
        // enum definition in that form's class. When we're extracting, we may use an "xform" which
        // likely has an incompatible field enum (where field id enum values are different between
        // the form and xform)
        static inline void appendOperandInfo_(OperandInfo & olist, Opcode icode,
                                              const InstMetaData::PtrType & meta,
                                              InstMetaData::OperandFieldID mid, const uint64_t fset,
                                              typename FormType::idType fid,
                                              bool is_store_data = false, bool suppress_x0 = false)
        {
            if (!isFixedField_(fid, fset))
            {
                uint64_t reg = extract_(fid, icode);
                if (!suppress_x0 || (reg != REGISTER_X0))
                {
                    olist.addElement(mid, meta->getOperandType(mid), reg, is_store_data);
                }
            }
        }

        static inline void appendUnmaskedOperandInfo_(
            OperandInfo & olist, Opcode icode, const InstMetaData::PtrType & meta,
            InstMetaData::OperandFieldID mid, const uint64_t mask, typename FormType::idType fid,
            bool is_store_data = false, bool suppress_x0 = false)
        {
            if (!isMaskedField_(fid, mask))
            {
                uint64_t reg = extract_(fid, icode);
                if (!suppress_x0 || (reg != REGISTER_X0))
                {
                    olist.addElement(mid, meta->getOperandType(mid), reg, is_store_data);
                }
            }
        }

        static inline void appendUnmaskedImpliedOperandInfo_(
            OperandInfo & olist, Opcode icode, const InstMetaData::PtrType & meta,
            InstMetaData::OperandFieldID mid, const uint64_t mask, typename FormType::idType fid,
            bool is_store_data = false, bool suppress_x0 = false)
        {
            if (!isMaskedField_(fid, mask))
            {
                uint64_t reg = extract_(fid, icode);
                if (!suppress_x0 || (reg != REGISTER_X0))
                {
                    olist.addElement(mid, meta->getOperandType(mid), reg, is_store_data, true);
                }
            }
        }

        static inline void appendCompressedOperandInfo_(OperandInfo & olist, Opcode icode,
                                                        const InstMetaData::PtrType & meta,
                                                        InstMetaData::OperandFieldID mid,
                                                        const uint64_t fset,
                                                        typename FormType::idType fid,
                                                        bool is_store_data = false)
        {
            if (!isFixedField_(fid, fset))
            {
                uint64_t reg = extractCompressedRegister_(fid, icode);
                olist.addElement(mid, meta->getOperandType(mid), reg, is_store_data);
            }
        }

        static inline void appendUnmaskedCompressedOperandInfo_(OperandInfo & olist, Opcode icode,
                                                                const InstMetaData::PtrType & meta,
                                                                InstMetaData::OperandFieldID mid,
                                                                const uint64_t mask,
                                                                typename FormType::idType fid,
                                                                bool is_store_data = false)
        {
            if (!isMaskedField_(fid, mask))
            {
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

        static inline std::string dasmFormatReg_(const InstMetaData::PtrType & meta,
                                                 InstMetaData::OperandFieldID mid, uint32_t regnum)
        {
            std::stringstream ss;
            switch (meta->getOperandType(mid))
            {
                case InstMetaData::OperandTypes::WORD:
                case InstMetaData::OperandTypes::LONG:
                    ss << "x" << std::dec << regnum;
                    break;
                case InstMetaData::OperandTypes::HALF:
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

        struct RegType_
        {
            typename FormType::idType fid;
            InstMetaData::OperandFieldID mid;
        };

        typedef std::vector<RegType_> RegTypeList_;

        static inline std::string dasmFormatRegList_(const InstMetaData::PtrType & meta,
                                                     const Opcode icode, uint64_t fixed_field_mask,
                                                     const RegTypeList_ & rtlist)
        {
            bool first = true;
            std::stringstream ss;
            for (const auto & r : rtlist)
            {
                if (!isMaskedField_(r.fid, fixed_field_mask))
                {
                    if (!first)
                    {
                        ss << ",";
                    }
                    else
                    {
                        first = false;
                    }
                    ss << dasmFormatReg_(meta, r.mid, extract_(r.fid, icode));
                }
            }
            return ss.str();
        }

        static inline std::string dasmFormatCompressedRegList_(const InstMetaData::PtrType & meta,
                                                               const Opcode icode,
                                                               uint64_t fixed_field_mask,
                                                               const RegTypeList_ & rtlist)
        {
            bool first = true;
            std::stringstream ss;
            for (const auto & r : rtlist)
            {
                if (!isMaskedField_(r.fid, fixed_field_mask))
                {
                    if (!first)
                    {
                        ss << ",";
                    }
                    else
                    {
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
    template <typename FormType> class Extractor;

    /**
     * ExtractorIF ostream insertion operator
     * @param os
     * @param f
     * @return
     */
    inline std::ostream & operator<<(std::ostream & os, const ExtractorIF::PtrType & f)
    {
        f->print(os);
        return os;
    }

} // namespace mavis
