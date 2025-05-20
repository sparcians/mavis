#pragma once

#include "mavis/Extractor.h"
#include "mavis/DecoderTypes.h"
#include "mavis/DecoderConsts.h"
#include "impl/forms/CommonForms.h"
#include "impl/forms/CompressedForms.h"
#include "impl/forms/VectorForms.h"

#include <cinttypes>

namespace
{
    std::string dasmVsetImmediate(const uint64_t immediate)
    {
        constexpr uint64_t VTYPE_FIELD_VLMUL = 0x7;
        constexpr uint64_t VTYPE_FIELD_VSEW = 0x38;
        constexpr uint64_t VTYPE_FIELD_VTA = 0x40;
        constexpr uint64_t VTYPE_FIELD_VMA = 0x80;

        std::stringstream ss;
        ss << "e" << (1 << (3 + ((immediate & VTYPE_FIELD_VSEW) >> 3))) << ",";
        if (const auto val = immediate & VTYPE_FIELD_VLMUL; val < 4)
        {
            ss << "m" << (1 << val) << ",";
        }
        else
        {
            ss << "mf" << (1 << (8 - val)) << ",";
        }
        ss << ((immediate & VTYPE_FIELD_VTA) ? "ta," : "tu,");
        ss << ((immediate & VTYPE_FIELD_VMA) ? "ma" : "mu");
        return ss.str();
    }
} // namespace

namespace mavis
{

    /**
     * AMO-Form Extractor
     */
    template <> class Extractor<Form_AMO> : public ExtractorBase<Form_AMO>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_AMO>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_AMO::idType::RS1, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_AMO::idType::RS2, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_AMO::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_AMO::idType::RS1, icode, fixed_field_mask_);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_AMO::idType::RS2, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_AMO::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_AMO::idType::RS1, false,
                                       suppress_x0);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_AMO::idType::RS2, false,
                                       suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_AMO::idType::RD, false, suppress_x0);
            return olist;
        }

        uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
        {
            switch (sfid)
            {
                // NOTE: AQ and WD are aliases for each other in the AMO form
                // RL and VM are also aliases for each other
                // Technically, we should throw an exception if a vector inst
                // tries to access AQ or a normal inst tries to access WD
                // likewise for RL/VM
                case SpecialField::AQ:
                    return extract_(Form_AMO::idType::AQ, icode);
                case SpecialField::RL:
                    return extract_(Form_AMO::idType::RL, icode);
                case SpecialField::VM:
                    return extract_(Form_AMO::idType::VM, icode);
                case SpecialField::WD:
                    return extract_(Form_AMO::idType::WD, icode);
                case SpecialField::AVL:
                case SpecialField::RM:
                case SpecialField::CSR:
                case SpecialField::FM:
                case SpecialField::NF:
                case SpecialField::PRED:
                case SpecialField::HINT:
                case SpecialField::SUCC:
                case SpecialField::__N:
                    return ExtractorBase::getSpecialField(sfid, icode);
            }
            return 0;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_AMO::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_AMO::idType::RS1, icode & ~fixed_field_mask_) << ","
               << extract_(Form_AMO::idType::RS2, icode & ~fixed_field_mask_)
               << ", aq/wd=" << std::dec << getSpecialField(SpecialField::AQ, icode)
               << ", rl/vm=" << std::dec << getSpecialField(SpecialField::RL, icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_AMO::idType::RD, InstMetaData::OperandFieldID::RD},
                                      {Form_AMO::idType::RS1, InstMetaData::OperandFieldID::RS1},
                                      {Form_AMO::idType::RS2, InstMetaData::OperandFieldID::RS2}})
               << ", aq/wd=" << std::dec << getSpecialField(SpecialField::AQ, icode)
               << ", rl/vm=" << std::dec << getSpecialField(SpecialField::RL, icode);
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_AMO>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * B-Form Extractor
     */
    template <> class Extractor<Form_B> : public ExtractorBase<Form_B>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_B>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_B::idType::RS1, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_B::idType::RS2, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_B::idType::RS1, icode, fixed_field_mask_);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_B::idType::RS2, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_B::idType::RS1, false, suppress_x0);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_B::idType::RS2, false, suppress_x0);
            return olist;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm7 = extract_(Form_B::idType::IMM7, icode & ~fixed_field_mask_);
            const uint64_t imm5 = extract_(Form_B::idType::IMM5, icode & ~fixed_field_mask_);
            return ((imm7 & 0x40ull) << 6ull) |  // Bit 7 of imm7 is IMM[12]
                   ((imm5 & 0x01ull) << 11ull) | // Bit 0 of imm5 is IMM[11]
                   ((imm7 & 0x3full) << 5ull) |  // Bits 0-6 of imm7 is IMM[10:5]
                   (imm5 & 0x1eull);             // Bits 1-4 of imm5 is IMM[4:1]
        }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 12);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_B::idType::RS1, icode & ~fixed_field_mask_)
               << "," << extract_(Form_B::idType::RS2, icode & ~fixed_field_mask_) << " +0x"
               << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_B::idType::RS1, InstMetaData::OperandFieldID::RS1},
                                      {Form_B::idType::RS2, InstMetaData::OperandFieldID::RS2}})
               << " +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_B>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * C0-Form Extractor (intended for decode only, NOT EXTRACTION)
     */
    template <> class Extractor<Form_C0> : public ExtractorBase<Form_C0>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_C0::idType::RS1, icode,
                                                      fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_C0::idType::RD, icode,
                                                      fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= extractUnmaskedCompressedIndexBit_(Form_C0::idType::RS1, icode,
                                                                 fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |= extractUnmaskedCompressedIndexBit_(Form_C0::idType::RD, icode,
                                                                 fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RS1,
                                                 fixed_field_mask_, Form_C0::idType::RS1, false);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RD,
                                                 fixed_field_mask_, Form_C0::idType::RD, false);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extractCompressedRegister_(Form_C0::idType::RD, icode & ~fixed_field_mask_) << ","
               << extractCompressedRegister_(Form_C0::idType::RS1, icode & ~fixed_field_mask_);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatCompressedRegList_(
                      meta, icode, fixed_field_mask_,
                      {{Form_C0::idType::RD, InstMetaData::OperandFieldID::RD},
                       {Form_C0::idType::RS1, InstMetaData::OperandFieldID::RS1}});
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_C0>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * C2-Form Extractor (intended for decode only, NOT EXTRACTION)
     */
    template <> class Extractor<Form_C2> : public ExtractorBase<Form_C2>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_C2::idType::RS, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_C2::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_C2::idType::RS, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_C2::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_C2::idType::RS, false, suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_C2::idType::RD, false, suppress_x0);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_C2::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_C2::idType::RS, icode & ~fixed_field_mask_);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_C2::idType::RD, InstMetaData::OperandFieldID::RD},
                                      {Form_C2::idType::RS, InstMetaData::OperandFieldID::RS1}});
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_C2>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * Derivative of Form_C2_sp extractor for STORES
     * NOTE: SP is the address base, rs2 is the source data
     */
    template <> class Extractor<Form_C2_sp_store> : public ExtractorBase<Form_C2_sp_store>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_sp_store>(ffmask, fset));
        }

        uint64_t getSourceRegs(const uint64_t icode) const override
        {
            return extractUnmaskedIndexBit_(Form_C2_sp_store::idType::RS2, icode, fixed_field_mask_)
                   | (1ull << REGISTER_SP); // add SP
        }

        uint64_t getSourceAddressRegs(const uint64_t icode) const override
        {
            return (1ull << REGISTER_SP);
        }

        uint64_t getSourceDataRegs(const uint64_t icode) const override
        {
            return extractUnmaskedIndexBit_(Form_C2_sp_store::idType::RS2, icode,
                                            fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const uint64_t icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                // This form has an implied SP field
                // Use the type of RS1 for the type of SP
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= (1ull << REGISTER_SP);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_C2_sp_store::idType::RS2, icode,
                                                       fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            // This form has an implied SP field
            // Use the type of RS1 for the type of SP
            olist.addElement(InstMetaData::OperandFieldID::RS1,
                             meta->getOperandType(InstMetaData::OperandFieldID::RS1), REGISTER_SP,
                             false);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_C2_sp_store::idType::RS2, true,
                                       suppress_x0);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const uint64_t icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extract_(Form_C2_sp_store::idType::RS2, icode & ~fixed_field_mask_)
               << ", SP, IMM=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(
                      meta, icode, fixed_field_mask_,
                      {{Form_C2_sp_store::idType::RS2, InstMetaData::OperandFieldID::RS2}})
               << ", SP, IMM=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_C2_sp_store>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * CA-Form Extractor
     */
    template <> class Extractor<Form_CA> : public ExtractorBase<Form_CA>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CA>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_CA::idType::RS1, icode,
                                                      fixed_field_mask_)
                   | extractUnmaskedCompressedIndexBit_(Form_CA::idType::RS2, icode,
                                                        fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_CA::idType::RD, icode,
                                                      fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= extractUnmaskedCompressedIndexBit_(Form_CA::idType::RS1, icode,
                                                                 fixed_field_mask_);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |= extractUnmaskedCompressedIndexBit_(Form_CA::idType::RS2, icode,
                                                                 fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |= extractUnmaskedCompressedIndexBit_(Form_CA::idType::RD, icode,
                                                                 fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RS1,
                                                 fixed_field_mask_, Form_CA::idType::RS1, false);
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RS2,
                                                 fixed_field_mask_, Form_CA::idType::RS2, false);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RD,
                                                 fixed_field_mask_, Form_CA::idType::RD, false);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extractCompressedRegister_(Form_CA::idType::RD, icode & ~fixed_field_mask_)
               << ", "
               << extractCompressedRegister_(Form_CA::idType::RS1, icode & ~fixed_field_mask_)
               << ", "
               << extractCompressedRegister_(Form_CA::idType::RS2, icode & ~fixed_field_mask_);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatCompressedRegList_(
                      meta, icode, fixed_field_mask_,
                      {{Form_CA::idType::RD, InstMetaData::OperandFieldID::RD},
                       {Form_CA::idType::RS1, InstMetaData::OperandFieldID::RS1},
                       {Form_CA::idType::RS2, InstMetaData::OperandFieldID::RS2}});
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_CA>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * CB-Form Extractor
     */
    template <> class Extractor<Form_CB> : public ExtractorBase<Form_CB>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CB>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_CB::idType::RS1, icode,
                                                      fixed_field_mask_)
                   | (0x1ull << REGISTER_X0);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= extractUnmaskedCompressedIndexBit_(Form_CB::idType::RS1, icode,
                                                                 fixed_field_mask_);
                    result |= (0x1ull << REGISTER_X0);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RS1,
                                                 fixed_field_mask_, Form_CB::idType::RS1, false);
            olist.addElement(InstMetaData::OperandFieldID::RS2,
                             meta->getOperandType(InstMetaData::OperandFieldID::RS1), REGISTER_X0,
                             false);
            return olist;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm3 = extract_(Form_CB::idType::IMM3, icode);
            const uint64_t imm5 = extract_(Form_CB::idType::IMM5, icode);
            return ((imm3 & 0x04ull) << 6ull) | // Bit 2 of imm3 is IMM[8]
                   ((imm5 & 0x18ull) << 3ull) | // Bits 3-4 of imm5 is IMM[7:6]
                   ((imm5 & 0x01ull) << 5ull) | // Bit 0 of imm5 is IMM[5]
                   ((imm3 & 0x03ull) << 3ull) | // Bits 0-1 of imm3 is IMM[4:3]
                   (imm5 & 0x06ull);            // Bits 1-2 of imm5 is IMM[2:1]
        }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 8);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extractCompressedRegister_(Form_CB::idType::RS1, icode & ~fixed_field_mask_)
               << ", 0, +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatCompressedRegList_(
                      meta, icode, fixed_field_mask_,
                      {{Form_CB::idType::RS1, InstMetaData::OperandFieldID::RS1}})
               << ", x0, +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_CB>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * CI-Form Extractor (c.addi)
     */
    template <> class Extractor<Form_CI> : public ExtractorBase<Form_CI>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CI>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_CI::idType::RS1, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_CI::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_CI::idType::RS1, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_CI::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_CI::idType::RS1, false, suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_CI::idType::RD, false, suppress_x0);
            return olist;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return (extract_(Form_CI::idType::IMM1, icode) << 5ull)
                   | extract_(Form_CI::idType::IMM5, icode);
        }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 5);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_CI::idType::RD, icode & ~fixed_field_mask_)
               << ", " << extract_(Form_CI::idType::RS1, icode & ~fixed_field_mask_) << ", +0x"
               << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_CI::idType::RD, InstMetaData::OperandFieldID::RD},
                                      {Form_CI::idType::RS1, InstMetaData::OperandFieldID::RS1}});
            if ((!isMaskedField_(Form_CI::idType::RD, fixed_field_mask_))
                || (!isMaskedField_(Form_CI::idType::RS1, fixed_field_mask_)))
            {
                ss << ", ";
            }
            ss << "+0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_CI>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * CI_rD_only-Form Extractor (c.li)
     */
    // TODO: Should this be a derived extractor from CI?
    template <> class Extractor<Form_CI_rD_only> : public ExtractorBase<Form_CI_rD_only>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CI_rD_only>(ffmask, fset));
        }

        bool isHint(Opcode icode) const override
        {
            return (getDestRegs(icode) & (1ull << REGISTER_X0)) != 0;
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return (0x1ull << REGISTER_X0);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_CI_rD_only::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                if ((kind == InstMetaData::OperandTypes::LONG)
                    || (kind == InstMetaData::OperandTypes::WORD))
                {
                    return (0x1ull << REGISTER_X0);
                }
                else
                {
                    return 0;
                }
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_CI_rD_only::idType::RD, icode,
                                                       fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            if (!suppress_x0)
            {
                // Implicit x0 source, with same "type" as rD
                olist.addElement(InstMetaData::OperandFieldID::RS1,
                                 meta->getOperandType(InstMetaData::OperandFieldID::RD),
                                 REGISTER_X0, false);
            }
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_CI_rD_only::idType::RD, false,
                                       suppress_x0);
            return olist;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return (extract_(Form_CI_rD_only::idType::IMM1, icode) << 5ull)
                   | extract_(Form_CI_rD_only::idType::IMM5, icode);
        }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 5);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extract_(Form_CI_rD_only::idType::RD, icode & ~fixed_field_mask_) << ", 0, +0x"
               << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(
                      meta, icode, fixed_field_mask_,
                      {{Form_CI_rD_only::idType::RD, InstMetaData::OperandFieldID::RD}})
               << ", x0, +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_CI_rD_only>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * CIW-Form Extractor
     */
    template <> class Extractor<Form_CIW> : public ExtractorBase<Form_CIW>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CIW>(ffmask, fset));
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_CIW::idType::RD, icode,
                                                      fixed_field_mask_);
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |= extractUnmaskedCompressedIndexBit_(Form_CIW::idType::RD, icode,
                                                                 fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RD,
                                                 fixed_field_mask_, Form_CIW::idType::RD, false);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extractCompressedRegister_(Form_CIW::idType::RD, icode & ~fixed_field_mask_)
               << ", IMM=" << std::dec
               << extract_(Form_CIW::idType::IMM8, icode & ~fixed_field_mask_);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatCompressedRegList_(
                      meta, icode, fixed_field_mask_,
                      {{Form_CIW::idType::RD, InstMetaData::OperandFieldID::RD}})
               << ", IMM=" << std::dec
               << extract_(Form_CIW::idType::IMM8, icode & ~fixed_field_mask_);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_CIW>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * CIX-Form Extractor
     */
    template <> class Extractor<Form_CIX> : public ExtractorBase<Form_CIX>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CIX>(ffmask, fset));
        }

        bool isHint(Opcode icode) const override { return getImmediate(icode) == 0; }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_CIX::idType::RS1, icode,
                                                      fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_CIX::idType::RD, icode,
                                                      fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= extractUnmaskedCompressedIndexBit_(Form_CIX::idType::RS1, icode,
                                                                 fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |= extractUnmaskedCompressedIndexBit_(Form_CIX::idType::RD, icode,
                                                                 fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RS1,
                                                 fixed_field_mask_, Form_CIX::idType::RS1, false);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RD,
                                                 fixed_field_mask_, Form_CIX::idType::RD, false);
            return olist;
        }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return (extract_(Form_CIX::idType::SHAMT1, icode) << 5ull)
                   | extract_(Form_CIX::idType::SHAMT5, icode);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extractCompressedRegister_(Form_CIX::idType::RD, icode & ~fixed_field_mask_)
               << ", IMM=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatCompressedRegList_(
                      meta, icode, fixed_field_mask_,
                      {{Form_CIX::idType::RD, InstMetaData::OperandFieldID::RD}})
               << ", IMM=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_CIX>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * CJ-Form Extractor
     */
    template <> class Extractor<Form_CJ> : public ExtractorBase<Form_CJ>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CJ>(ffmask, fset));
        }

        uint64_t getDestRegs(const Opcode icode) const override { return (0x1ull << REGISTER_X0); }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                if ((kind == InstMetaData::OperandTypes::LONG)
                    || (kind == InstMetaData::OperandTypes::WORD))
                {
                    return (0x1ull << REGISTER_X0);
                }
                else
                {
                    return 0;
                }
            }
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            if (!suppress_x0)
            {
                // Implicit x0 dest, with "LONG" type
                olist.addElement(InstMetaData::OperandFieldID::RD, InstMetaData::OperandTypes::LONG,
                                 REGISTER_X0, false);
            }
            return olist;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm11 = extract_(Form_CJ::idType::IMM11, icode & ~fixed_field_mask_);
            return ((imm11 & 0x400ull) << 1ull) | // Bit 10 is IMM[11]
                   ((imm11 & 0x040ull) << 4ull) | // Bit 6 is IMM[10]
                   ((imm11 & 0x180ull) << 1ull) | // Bits 7-8  is IMM[9:8]
                   ((imm11 & 0x010ull) << 3ull) | // Bit 4 is IMM[7]
                   ((imm11 & 0x020ull) << 1ull) | // Bit 5 is IMM[6]
                   ((imm11 & 0x001ull) << 5ull) | // Bit 0 is IMM[5]
                   ((imm11 & 0x200ull) >> 5ull) | // Bit 9 is IMM[4]
                   (imm11 & 0x00Eull);            // Bits 1-3 is IMM[3:1]
        }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 11);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << "x0, +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

      protected:
        Extractor<Form_CJ>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * CJR-Form Extractor
     */
    template <> class Extractor<Form_CJR> : public ExtractorBase<Form_CJR>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CJR>(ffmask, fset));
        }

        bool isIllop(Opcode icode) const override
        {
            return (getSourceRegs(icode) & (1ull << REGISTER_X0)) != 0;
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_CJR::idType::RS1, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override { return (0x1ull << REGISTER_X0); }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_CJR::idType::RS1, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                if ((kind == InstMetaData::OperandTypes::LONG)
                    || (kind == InstMetaData::OperandTypes::WORD))
                {
                    return (0x1ull << REGISTER_X0);
                }
                else
                {
                    return 0;
                }
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_CJR::idType::RS1, false,
                                       suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            if (!suppress_x0)
            {
                // Implicit x0 dest, with "LONG" type
                olist.addElement(InstMetaData::OperandFieldID::RD, InstMetaData::OperandTypes::LONG,
                                 REGISTER_X0, false);
            }
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t0, "
               << extract_(Form_CJR::idType::RS1, icode & ~fixed_field_mask_);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\tx0, "
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_CJR::idType::RS1, InstMetaData::OperandFieldID::RS1}});
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_CJR>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * CMPP-Form Extractor
     */
    template <> class Extractor<Form_CMPP> : public ExtractorBase<Form_CMPP>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CMPP>(ffmask, fset));
        }

        bool isIllop(Opcode icode) const override
        {
            return extract_(Form_CMPP::idType::URLIST, icode) < MIN_URLIST_;
        }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return extract_(Form_CMPP::idType::SPIMM, icode) << 4;
        }

        uint64_t getSourceRegs(const Opcode) const override
        {
            return 1ull << REGISTER_SP;
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return (1ull << REGISTER_SP) | decodeRlist_(icode);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isAllOperandType(kind)
                || (kind == InstMetaData::OperandTypes::LONG)
                || (kind == InstMetaData::OperandTypes::WORD))
            {
                return getSourceRegs(icode);
            }
            else
            {
                return 0;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isAllOperandType(kind)
                || (kind == InstMetaData::OperandTypes::LONG)
                || (kind == InstMetaData::OperandTypes::WORD))
            {
                return getDestRegs(icode);
            }
            else
            {
                return 0;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            const auto op_type = meta->getOperandType(InstMetaData::OperandFieldID::RS1);

            OperandInfo olist;
            olist.addElement(InstMetaData::OperandFieldID::RS1, op_type, REGISTER_SP, false);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            const auto op_type = meta->getOperandType(InstMetaData::OperandFieldID::RS1);

            OperandInfo olist;
            olist.addElement(InstMetaData::OperandFieldID::RD, op_type, REGISTER_SP, false);
            convertRlistToOpcodeInfo_(olist, icode, op_type);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << '\t';
            formatRlist_<false>(ss, icode);
            ss << ", " << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << '\t';
            formatRlist_<true>(ss, icode);
            ss << ", " << getStackAdj_(icode, meta);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_CMPP>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        virtual int64_t getStackAdjBase_(const Opcode icode, const InstMetaData::PtrType & meta) const
        {
            const auto urlist = extract_(Form_CMPP::idType::URLIST, icode);
            const auto [begin, end] = getRListRange_(urlist);
            const auto num_regs = std::distance(begin, end);
            const auto reg_size_bytes = meta->getDataSize() / 8;
            // Round up to the nearest multiple of 16
            return (num_regs * reg_size_bytes + 15) & -16ll;
        }

        int64_t getStackAdj_(const Opcode icode, const InstMetaData::PtrType & meta) const
        {
            return getStackAdjBase_(icode, meta) + getSignedOffset(icode);
        }

        template<bool enable_prefix>
        static void formatRlist_(std::stringstream& ss, const Opcode icode)
        {
            const auto urlist = extract_(Form_CMPP::idType::URLIST, icode);

            const auto format_urlist = [&ss](const uint64_t urlist) {
                if constexpr(enable_prefix)
                {
                    ss << 'x';
                }

                ss << *std::prev(getRListRange_(urlist).second);
            };

            ss << "{";

            for(const auto& range: RLIST_RANGES_)
            {
                const auto urlist_begin = range.first;

                if(urlist < urlist_begin)
                {
                    break;
                }
                else if(urlist_begin > MIN_URLIST_)
                {
                    ss << ", ";
                }

                const auto urlist_end = std::min(range.second, urlist);

                format_urlist(urlist_begin);

                if(urlist_begin == urlist_end)
                {
                    return;
                }

                ss << '-';

                format_urlist(urlist_end);
            }

            ss << "}";
        }

        inline static constexpr uint64_t MIN_URLIST_ = 4;
        inline static constexpr size_t NUM_RLIST_ENTRIES_ = 13;
        using RListArray = std::array<uint32_t, NUM_RLIST_ENTRIES_>;
        inline static constexpr RListArray RLIST_{
            REGISTER_LINK,  // urlist >= 0x4
            8,              // urlist >= 0x5
            9,              // urlist >= 0x6
            18,             // urlist >= 0x7
            19,             // urlist >= 0x8
            20,             // urlist >= 0x9
            21,             // urlist >= 0xa
            22,             // urlist >= 0xb
            23,             // urlist >= 0xc
            24,             // urlist >= 0xd
            25,             // urlist >= 0xe
            // These two entries *both* belong to urlist == 0xf - this special case is handled in getRListRange_
            26,
            27
        };

        inline static constexpr std::array<std::pair<uint64_t, uint64_t>, 3> RLIST_RANGES_{{{MIN_URLIST_, MIN_URLIST_},
                                                                                            {0x5, 0x6},
                                                                                            {0x7, 0xf}}};

        static constexpr std::pair<RListArray::const_iterator, RListArray::const_iterator> getRListRange_(const uint64_t urlist)
        {
            return std::make_pair(RLIST_.begin(), urlist == 0xf ? RLIST_.end() : std::next(RLIST_.begin(), urlist - MIN_URLIST_ + 1));
        }

        static uint64_t decodeRlist_(const Opcode icode)
        {
            uint64_t regs = 0;

            auto urlist = extract_(Form_CMPP::idType::URLIST, icode);

            const auto [begin, end] = getRListRange_(urlist);

            for(auto it = begin; it != end; ++it)
            {
                regs |= (1ull << *it);
            }

            return regs;
        }

        static void convertRlistToOpcodeInfo_(OperandInfo& olist,
                                              const Opcode icode,
                                              const InstMetaData::OperandTypes op_type,
                                              InstMetaData::OperandFieldID op_id = InstMetaData::OperandFieldID::POP_RD1)
        {
            const auto urlist = extract_(Form_CMPP::idType::URLIST, icode);

            const auto append_op = [&olist, &op_id, op_type](const uint32_t reg){
                olist.addElement(op_id, op_type, reg, true);
                op_id = static_cast<InstMetaData::OperandFieldID>(static_cast<std::underlying_type_t<InstMetaData::OperandFieldID>>(op_id) + 1);
            };

            const auto [begin, end] = getRListRange_(urlist);

            for(auto it = begin; it != end; ++it)
            {
                append_op(*it);
            }
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * CMJT-Form Extractor
     */
    template <> class Extractor<Form_CMJT> : public ExtractorBase<Form_CMJT>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CMJT>(ffmask, fset));
        }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return extract_(Form_CMJT::idType::INDEX, icode);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            if(isJALT_(icode))
            {
                return REGISTER_LINK;
            }

            return 0;
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isAllOperandType(kind)
                || (kind == InstMetaData::OperandTypes::LONG)
                || (kind == InstMetaData::OperandTypes::WORD))
            {
                return getDestRegs(icode);
            }
            else
            {
                return 0;
            }
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            const auto op_type = meta->getOperandType(InstMetaData::OperandFieldID::RD);

            OperandInfo olist;
            olist.addElement(InstMetaData::OperandFieldID::RD, op_type, REGISTER_LINK, false);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << (isJALT_(icode) ? jalt_mnemonic_ : mnemonic) << '\t' << getImmediate(icode);
            return ss.str();
        }

      protected:
        Extractor<Form_CMJT>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        bool isJALT_(const Opcode icode) const
        {
            return getImmediate(icode) >= 32;
        }

        inline static const std::string jalt_mnemonic_{"cm.jalt"};
        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * CSR-Form Extractor
     */
    template <> class Extractor<Form_CSR> : public ExtractorBase<Form_CSR>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CSR>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_CSR::idType::RS1, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_CSR::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_CSR::idType::RS1, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_CSR::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_CSR::idType::RS1, false,
                                       suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_CSR::idType::RD, false, suppress_x0);
            return olist;
        }

        uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
        {
            switch (sfid)
            {
                case SpecialField::CSR:
                    return extract_(Form_CSR::idType::CSR, icode);
                case SpecialField::AQ:
                case SpecialField::AVL:
                case SpecialField::FM:
                case SpecialField::NF:
                case SpecialField::PRED:
                case SpecialField::HINT:
                case SpecialField::RL:
                case SpecialField::RM:
                case SpecialField::SUCC:
                case SpecialField::VM:
                case SpecialField::WD:
                case SpecialField::__N:
                    return ExtractorBase::getSpecialField(sfid, icode);
            }
            return 0;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_CSR::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_CSR::idType::RS1, icode & ~fixed_field_mask_) << ", CSR=0x"
               << std::hex << getSpecialField(SpecialField::CSR, icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_CSR::idType::RD, InstMetaData::OperandFieldID::RD},
                                      {Form_CSR::idType::RS1, InstMetaData::OperandFieldID::RS1}})
               << ", CSR=0x" << std::hex << getSpecialField(SpecialField::CSR, icode);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_CSR>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * CSRI-Form Extractor
     */
    template <> class Extractor<Form_CSRI> : public ExtractorBase<Form_CSRI>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CSRI>(ffmask, fset));
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_CSRI::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_CSRI::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_CSRI::idType::RD, false,
                                       suppress_x0);
            return olist;
        }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return extract_(Form_CSRI::idType::UIMM, icode & ~fixed_field_mask_);
        }

        uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
        {
            switch (sfid)
            {
                case SpecialField::CSR:
                    return extract_(Form_CSRI::idType::CSR, icode);
                case SpecialField::AQ:
                case SpecialField::AVL:
                case SpecialField::FM:
                case SpecialField::NF:
                case SpecialField::PRED:
                case SpecialField::HINT:
                case SpecialField::RL:
                case SpecialField::RM:
                case SpecialField::SUCC:
                case SpecialField::VM:
                case SpecialField::WD:
                case SpecialField::__N:
                    return ExtractorBase::getSpecialField(sfid, icode);
            }
            return 0;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_CSRI::idType::RD, icode & ~fixed_field_mask_)
               << ", IMM=" << std::dec << getImmediate(icode) << ", CSR=0x" << std::hex
               << getSpecialField(SpecialField::CSR, icode & ~fixed_field_mask_);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_CSRI::idType::RD, InstMetaData::OperandFieldID::RD}})
               << ", IMM=" << std::dec << getImmediate(icode) << ", CSR=0x" << std::hex
               << getSpecialField(SpecialField::CSR, icode & ~fixed_field_mask_);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_CSRI>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * FENCE-Form Extractor
     */
    template <> class Extractor<Form_FENCE> : public ExtractorBase<Form_FENCE>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_FENCE>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_FENCE::idType::RS1, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_FENCE::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_FENCE::idType::RS1, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_FENCE::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_FENCE::idType::RS1, false,
                                       suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_FENCE::idType::RD, false,
                                       suppress_x0);
            return olist;
        }

        uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
        {
            switch (sfid)
            {
                case SpecialField::FM:
                    return extract_(Form_FENCE::idType::FM, icode);
                case SpecialField::PRED:
                case SpecialField::HINT:
                    return extract_(Form_FENCE::idType::PRED, icode);
                case SpecialField::SUCC:
                    return extract_(Form_FENCE::idType::SUCC, icode);
                case SpecialField::AQ:
                case SpecialField::AVL:
                case SpecialField::CSR:
                case SpecialField::NF:
                case SpecialField::RL:
                case SpecialField::RM:
                case SpecialField::VM:
                case SpecialField::WD:
                case SpecialField::__N:
                    return ExtractorBase::getSpecialField(sfid, icode);
            }
            return 0;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_FENCE::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_FENCE::idType::RS1, icode & ~fixed_field_mask_) << ", fm=0x"
               << std::hex << getSpecialField(SpecialField::FM, icode) << ", pred=0x" << std::hex
               << getSpecialField(SpecialField::PRED, icode) << ", succ=0x" << std::hex
               << getSpecialField(SpecialField::SUCC, icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_FENCE::idType::RD, InstMetaData::OperandFieldID::RD},
                                      {Form_FENCE::idType::RS1, InstMetaData::OperandFieldID::RS1}})
               << ", fm=0x" << std::hex << getSpecialField(SpecialField::FM, icode) << ", pred=0x"
               << std::hex << getSpecialField(SpecialField::PRED, icode) << ", succ=0x" << std::hex
               << getSpecialField(SpecialField::SUCC, icode);
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_FENCE>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * I-Form Extractor
     */
    template <> class Extractor<Form_I> : public ExtractorBase<Form_I>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_I>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_I::idType::RS1, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_I::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_I::idType::RS1, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_I::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_I::idType::RS1, false, suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_I::idType::RD, false, suppress_x0);
            return olist;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return extract_(Form_I::idType::IMM, icode);
        }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 11);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_I::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_I::idType::RS1, icode & ~fixed_field_mask_) << ", +0x"
               << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_I::idType::RD, InstMetaData::OperandFieldID::RD},
                                      {Form_I::idType::RS1, InstMetaData::OperandFieldID::RS1}})
               << ", +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_I>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * ISH-Form Extractor
     */
    template <> class Extractor<Form_ISH> : public ExtractorBase<Form_ISH>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_ISH>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_ISH::idType::RS1, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_ISH::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_ISH::idType::RS1, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_ISH::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_ISH::idType::RS1, false,
                                       suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_ISH::idType::RD, false, suppress_x0);
            return olist;
        }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return extract_(Form_ISH::idType::SHAMT, icode);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_ISH::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_ISH::idType::RS1, icode & ~fixed_field_mask_);
            if (!isMaskedField_(Form_ISH::idType::SHAMT, fixed_field_mask_))
            {
                ss << ", SHAMT=" << std::dec << getImmediate(icode);
            }
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_ISH::idType::RD, InstMetaData::OperandFieldID::RD},
                                      {Form_ISH::idType::RS1, InstMetaData::OperandFieldID::RS1}});
            if (!isMaskedField_(Form_ISH::idType::SHAMT, fixed_field_mask_))
            {
                ss << ", SHAMT=" << std::dec << getImmediate(icode);
            }
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_ISH>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * ISHW-Form Extractor
     */
    template <> class Extractor<Form_ISHW> : public ExtractorBase<Form_ISHW>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_ISHW>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_ISHW::idType::RS1, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_ISHW::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_ISHW::idType::RS1, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_ISHW::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_ISHW::idType::RS1, false,
                                       suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_ISHW::idType::RD, false,
                                       suppress_x0);
            return olist;
        }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return extract_(Form_ISHW::idType::SHAMTW, icode);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_ISHW::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_ISHW::idType::RS1, icode & ~fixed_field_mask_)
               << ", SHAMTW=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_ISHW::idType::RD, InstMetaData::OperandFieldID::RD},
                                      {Form_ISHW::idType::RS1, InstMetaData::OperandFieldID::RS1}})
               << ", SHAMTW=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_ISHW>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * J-Form Extractor
     */
    template <> class Extractor<Form_J> : public ExtractorBase<Form_J>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_J>(ffmask, fset));
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_J::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_J::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_J::idType::RD, false, suppress_x0);
            return olist;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm20 = extract_(Form_J::idType::IMM20, icode & ~fixed_field_mask_);
            return ((imm20 & 0x80000ull) << 1ull) |  // Bit 19 is IMM[20]
                   ((imm20 & 0x000FFull) << 12ull) | // Bits 0-7 is IMM[19:12]
                   ((imm20 & 0x00100ull) << 3ull) |  // Bit 8 is IMM[11]
                   ((imm20 & 0x7FE00ull) >> 8ull);   // Bits 9-18 is IMM[10:1]
        }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 20);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_J::idType::RD, icode & ~fixed_field_mask_)
               << ", +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_J::idType::RD, InstMetaData::OperandFieldID::RD}})
               << ", +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_J>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * R-Form Extractor
     */
    template <> class Extractor<Form_R> : public ExtractorBase<Form_R>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_R>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_R::idType::RS1, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_R::idType::RS2, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_R::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_R::idType::RS1, icode, fixed_field_mask_);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_R::idType::RS2, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_R::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_R::idType::RS1, false, suppress_x0);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_R::idType::RS2, false, suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_R::idType::RD, false, suppress_x0);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_R::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_R::idType::RS1, icode & ~fixed_field_mask_) << ","
               << extract_(Form_R::idType::RS2, icode & ~fixed_field_mask_);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_R::idType::RD, InstMetaData::OperandFieldID::RD},
                                      {Form_R::idType::RS1, InstMetaData::OperandFieldID::RS1},
                                      {Form_R::idType::RS2, InstMetaData::OperandFieldID::RS2}});
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_R>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * Rfloat-Form Extractor
     */
    template <> class Extractor<Form_Rfloat> : public ExtractorBase<Form_Rfloat>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_Rfloat>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_Rfloat::idType::RS1, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_Rfloat::idType::RS2, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_Rfloat::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_Rfloat::idType::RS1, icode,
                                                       fixed_field_mask_);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_Rfloat::idType::RS2, icode,
                                                       fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_Rfloat::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_Rfloat::idType::RS1, false,
                                       suppress_x0);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_Rfloat::idType::RS2, false,
                                       suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_Rfloat::idType::RD, false,
                                       suppress_x0);
            return olist;
        }

        uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
        {
            switch (sfid)
            {
                case SpecialField::RM:
                    return extract_(Form_Rfloat::idType::RM, icode);
                case SpecialField::AQ:
                case SpecialField::AVL:
                case SpecialField::CSR:
                case SpecialField::FM:
                case SpecialField::NF:
                case SpecialField::PRED:
                case SpecialField::HINT:
                case SpecialField::RL:
                case SpecialField::SUCC:
                case SpecialField::VM:
                case SpecialField::WD:
                case SpecialField::__N:
                    return ExtractorBase::getSpecialField(sfid, icode);
            }
            return 0;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_Rfloat::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_Rfloat::idType::RS1, icode & ~fixed_field_mask_) << ","
               << extract_(Form_Rfloat::idType::RS2, icode & ~fixed_field_mask_);
            // Show the rm operand if field is not fixed (part of the encoding)
            if (!isMaskedField_(Form_Rfloat::idType::RM, fixed_field_mask_))
            {
                ss << ", RM=" << getSpecialField(SpecialField::RM, icode);
            }
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(
                      meta, icode, fixed_field_mask_,
                      {{Form_Rfloat::idType::RD, InstMetaData::OperandFieldID::RD},
                       {Form_Rfloat::idType::RS1, InstMetaData::OperandFieldID::RS1},
                       {Form_Rfloat::idType::RS2, InstMetaData::OperandFieldID::RS2}});
            // Show the rm operand if field is not fixed (part of the encoding)
            if (!isMaskedField_(Form_Rfloat::idType::RM, fixed_field_mask_))
            {
                ss << ", RM=" << getSpecialField(SpecialField::RM, icode);
            }
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_Rfloat>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask)
        {
        }
    };

    /**
     * R4-Form Extractor
     */
    template <> class Extractor<Form_R4> : public ExtractorBase<Form_R4>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_R4>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_R4::idType::RS1, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_R4::idType::RS2, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_R4::idType::RS3, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_R4::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_R4::idType::RS1, icode, fixed_field_mask_);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_R4::idType::RS2, icode, fixed_field_mask_);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS3, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_R4::idType::RS3, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_R4::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_R4::idType::RS1, false, suppress_x0);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_R4::idType::RS2, false, suppress_x0);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS3,
                                       fixed_field_mask_, Form_R4::idType::RS3, false, suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_R4::idType::RD, false, suppress_x0);
            return olist;
        }

        uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
        {
            switch (sfid)
            {
                case SpecialField::RM:
                    return extract_(Form_R4::idType::RM, icode);
                case SpecialField::AQ:
                case SpecialField::AVL:
                case SpecialField::CSR:
                case SpecialField::FM:
                case SpecialField::NF:
                case SpecialField::PRED:
                case SpecialField::HINT:
                case SpecialField::RL:
                case SpecialField::SUCC:
                case SpecialField::VM:
                case SpecialField::WD:
                case SpecialField::__N:
                    return ExtractorBase::getSpecialField(sfid, icode);
            }
            return 0;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_R4::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_R4::idType::RS1, icode & ~fixed_field_mask_) << ","
               << extract_(Form_R4::idType::RS2, icode & ~fixed_field_mask_) << ","
               << extract_(Form_R4::idType::RS3, icode & ~fixed_field_mask_);
            // Show the rm operand if field is not fixed (part of the encoding)
            if (!isMaskedField_(Form_R4::idType::RM, fixed_field_mask_))
            {
                ss << ", RM=" << getSpecialField(SpecialField::RM, icode);
            }
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_R4::idType::RD, InstMetaData::OperandFieldID::RD},
                                      {Form_R4::idType::RS1, InstMetaData::OperandFieldID::RS1},
                                      {Form_R4::idType::RS2, InstMetaData::OperandFieldID::RS2},
                                      {Form_R4::idType::RS3, InstMetaData::OperandFieldID::RS3}});
            // Show the rm operand if field is not fixed (part of the encoding)
            if (!isMaskedField_(Form_R4::idType::RM, fixed_field_mask_))
            {
                ss << ", RM=" << getSpecialField(SpecialField::RM, icode);
            }
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_R4>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * S-Form Extractor (int and float STORES)
     * NOTE: rs1 is the address register (base)
     * and rs2 is the source data register
     */
    template <> class Extractor<Form_S> : public ExtractorBase<Form_S>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_S>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_S::idType::RS1, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_S::idType::RS2, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_S::idType::RS1, icode, fixed_field_mask_);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_S::idType::RS2, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getSourceAddressRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_S::idType::RS1, icode, fixed_field_mask_);
        }

        uint64_t getSourceDataRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_S::idType::RS2, icode, fixed_field_mask_);
        }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return (extract_(Form_S::idType::IMM7, icode & ~fixed_field_mask_) << 5ull)
                   | extract_(Form_S::idType::IMM5, icode & ~fixed_field_mask_);
        }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 11);
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_S::idType::RS1, false, suppress_x0);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_S::idType::RS2, true, suppress_x0);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extract_(Form_S::idType::RS2, icode & ~fixed_field_mask_)        // source data
               << "," << extract_(Form_S::idType::RS1, icode & ~fixed_field_mask_) // base address
               << ", +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_S::idType::RS2, InstMetaData::OperandFieldID::RS2},
                                      {Form_S::idType::RS1, InstMetaData::OperandFieldID::RS1}})
               << ", +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_S>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * U-Form Extractor
     */
    template <> class Extractor<Form_U> : public ExtractorBase<Form_U>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_U>(ffmask, fset));
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return 1ull << extract_(Form_U::idType::RD, icode & ~fixed_field_mask_);
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_U::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_U::idType::RD, false, suppress_x0);
            return olist;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return extract_(Form_U::idType::IMM20, icode & ~fixed_field_mask_) << 12;
        }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 31);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_U::idType::RD, icode & ~fixed_field_mask_)
               << ", +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_U::idType::RD, InstMetaData::OperandFieldID::RD}})
               << ", +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_U>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask) {}
    };

    /**
     * V-Form Extractor
     */
    template <> class Extractor<Form_V> : public ExtractorBase<Form_V>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_V::idType::RS1, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_V::idType::RS2, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_V::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_V::idType::RS1, icode, fixed_field_mask_);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_V::idType::RS2, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_V::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_V::idType::RS1, false, suppress_x0);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_V::idType::RS2, false, suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_V::idType::RD, false, suppress_x0);
            return olist;
        }

        // TODO: add VM special fields
        uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
        {
            switch (sfid)
            {
                // TODO: All forms should be using this pattern...
                case SpecialField::VM:
                    if (isMaskedField_(Form_V::idType::VM, fixed_field_mask_))
                    {
                        throw UnsupportedExtractorSpecialFieldID("VM", icode);
                    }
                    else
                    {
                        return extract_(Form_V::idType::VM, icode);
                    }
                case SpecialField::RM:
                case SpecialField::AQ:
                case SpecialField::AVL:
                case SpecialField::CSR:
                case SpecialField::FM:
                case SpecialField::NF:
                case SpecialField::PRED:
                case SpecialField::HINT:
                case SpecialField::RL:
                case SpecialField::SUCC:
                case SpecialField::WD:
                case SpecialField::__N:
                    return ExtractorBase::getSpecialField(sfid, icode);
            }
            return 0;
        }

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\tv" << extract_(Form_V::idType::RD, icode & ~fixed_field_mask_)
               << ",v" << extract_(Form_V::idType::RS1, icode & ~fixed_field_mask_) << ",v"
               << extract_(Form_V::idType::RS2, icode & ~fixed_field_mask_);
            // Show the vm operand if masking mode is on
            if (!isMaskedField_(Form_V::idType::VM, fixed_field_mask_))
            {
                if (!extract_(Form_V::idType::VM, icode))
                {
                    ss << ",v0.t";
                }
            }
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_V::idType::RD, InstMetaData::OperandFieldID::RD},
                                      {Form_V::idType::RS1, InstMetaData::OperandFieldID::RS1},
                                      {Form_V::idType::RS2, InstMetaData::OperandFieldID::RS2}});
            // Show the vm operand if masking mode is on
            if (!isMaskedField_(Form_V::idType::VM, fixed_field_mask_))
            {
                if (!extract_(Form_V::idType::VM, icode))
                {
                    ss << ",v0.t";
                }
            }
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_V>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * VF_mem-Form Extractor
     */
    template <> class Extractor<Form_VF_mem> : public ExtractorBase<Form_VF_mem>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_VF_mem>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_VF_mem::idType::RS1, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_VF_mem::idType::RS2, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_VF_mem::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_VF_mem::idType::RS1, icode,
                                                       fixed_field_mask_);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_VF_mem::idType::RS2, icode,
                                                       fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_VF_mem::idType::RD, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_VF_mem::idType::RS1, false,
                                       suppress_x0);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_VF_mem::idType::RS2, false,
                                       suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_VF_mem::idType::RD, false,
                                       suppress_x0);
            return olist;
        }

        // TODO: add NF and VM special fields
        uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
        {
            switch (sfid)
            {
                // TODO: All forms should be using this pattern...
                case SpecialField::NF:
                    if (isMaskedField_(Form_VF_mem::idType::NF, fixed_field_mask_))
                    {
                        throw UnsupportedExtractorSpecialFieldID("NF", icode);
                    }
                    else
                    {
                        return extract_(Form_VF_mem::idType::NF, icode);
                    }
                case SpecialField::VM:
                    if (isMaskedField_(Form_VF_mem::idType::VM, fixed_field_mask_))
                    {
                        throw UnsupportedExtractorSpecialFieldID("VM", icode);
                    }
                    else
                    {
                        return extract_(Form_VF_mem::idType::VM, icode);
                    }
                case SpecialField::RM:
                case SpecialField::AQ:
                case SpecialField::AVL:
                case SpecialField::CSR:
                case SpecialField::FM:
                case SpecialField::PRED:
                case SpecialField::HINT:
                case SpecialField::RL:
                case SpecialField::SUCC:
                case SpecialField::WD:
                case SpecialField::__N:
                    return ExtractorBase::getSpecialField(sfid, icode);
            }
            return 0;
        }

        std::string getVectorMemoryMnemonic(const std::string & mnemonic, const Opcode icode) const
        {
            constexpr auto MEWOP_UNITSTRIDE = 0;
            constexpr auto MEWOP_UNORDERED_INDEX = 1;
            constexpr auto MEWOP_STRIDE = 2;
            constexpr auto MEWOP_ORDERED_INDEX = 3;

            if (!isMaskedField_(Form_VF_mem::idType::NF, fixed_field_mask_))
            {
                if (const auto nf = extract_(Form_VF_mem::idType::NF, icode); nf)
                {
                    switch (extract_(Form_VF_mem::idType::MEWOP, icode))
                    {
                        case MEWOP_UNITSTRIDE:
                            return mnemonic.substr(0, 2) + "seg" + std::to_string(nf + 1)
                                   + mnemonic.substr(2, std::string::npos);
                        case MEWOP_UNORDERED_INDEX:
                        case MEWOP_ORDERED_INDEX:
                            return mnemonic.substr(0, 4) + "seg" + std::to_string(nf + 1)
                                   + mnemonic.substr(4, std::string::npos);
                        case MEWOP_STRIDE:
                            return mnemonic.substr(0, 3) + "seg" + std::to_string(nf + 1)
                                   + mnemonic.substr(3, std::string::npos);
                        default:
                            throw UnsupportedExtractorSpecialFieldID("MEWOP", icode);
                    }
                }
            }
            return mnemonic;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << getVectorMemoryMnemonic(mnemonic, icode) << "\tv"
               << extract_(Form_VF_mem::idType::RD, icode & ~fixed_field_mask_) << ",v"
               << extract_(Form_VF_mem::idType::RS1, icode & ~fixed_field_mask_) << ",v"
               << extract_(Form_VF_mem::idType::RS2, icode & ~fixed_field_mask_);
            // Show the vm operand if masking mode is on
            if (!isMaskedField_(Form_VF_mem::idType::VM, fixed_field_mask_))
            {
                if (!extract_(Form_VF_mem::idType::VM, icode))
                {
                    ss << ",v0.t";
                }
            }
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << getVectorMemoryMnemonic(mnemonic, icode) << "\t"
               << dasmFormatRegList_(
                      meta, icode, fixed_field_mask_,
                      {{Form_VF_mem::idType::RD, InstMetaData::OperandFieldID::RD},
                       {Form_VF_mem::idType::RS1, InstMetaData::OperandFieldID::RS1},
                       {Form_VF_mem::idType::RS2, InstMetaData::OperandFieldID::RS2}});
            // Show the vm operand if masking mode is on
            if (!isMaskedField_(Form_VF_mem::idType::VM, fixed_field_mask_))
            {
                if (!extract_(Form_VF_mem::idType::VM, icode))
                {
                    ss << ",v0.t";
                }
            }
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor<Form_VF_mem>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };

    /**
     * V_vsetvli-Form Extractor
     */
    template <> class Extractor<Form_V_vsetvli> : public ExtractorBase<Form_V_vsetvli>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V_vsetvli>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_V_vsetvli::idType::RS1, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_V_vsetvli::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_V_vsetvli::idType::RS1, icode,
                                                       fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_V_vsetvli::idType::RD, icode,
                                                       fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_V_vsetvli::idType::RS1, false,
                                       suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_V_vsetvli::idType::RD, false,
                                       suppress_x0);
            return olist;
        }

        // TODO: add VM special fields
        uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
        {
            switch (sfid)
            {
                case SpecialField::AQ:
                case SpecialField::AVL:
                case SpecialField::CSR:
                case SpecialField::FM:
                case SpecialField::NF:
                case SpecialField::PRED:
                case SpecialField::HINT:
                case SpecialField::RL:
                case SpecialField::RM:
                case SpecialField::SUCC:
                case SpecialField::VM:
                case SpecialField::WD:
                case SpecialField::__N:
                    return ExtractorBase::getSpecialField(sfid, icode);
            }
            return 0;
        }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return extract_(Form_V_vsetvli::idType::IMM11, icode & ~fixed_field_mask_);
        }

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\tx"
               << extract_(Form_V_vsetvli::idType::RD, icode & ~fixed_field_mask_) << ",x"
               << extract_(Form_V_vsetvli::idType::RS1, icode & ~fixed_field_mask_) << ", "
               << dasmVsetImmediate(getImmediate(icode));
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(
                      meta, icode, fixed_field_mask_,
                      {{Form_V_vsetvli::idType::RD, InstMetaData::OperandFieldID::RD},
                       {Form_V_vsetvli::idType::RS1, InstMetaData::OperandFieldID::RS1}})
               << ", " << dasmVsetImmediate(getImmediate(icode));
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_V_vsetvli>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask)
        {
        }
    };

    /**
     * V_vsetivli-Form Extractor
     */
    template <> class Extractor<Form_V_vsetivli> : public ExtractorBase<Form_V_vsetivli>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V_vsetivli>(ffmask, fset));
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_V_vsetivli::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_V_vsetivli::idType::RD, icode,
                                                       fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_V_vsetivli::idType::RD, false,
                                       suppress_x0);
            return olist;
        }

        // TODO: add VM special fields
        uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
        {
            switch (sfid)
            {
                case SpecialField::AVL:
                    return extract_(Form_V_vsetivli::idType::AVL, icode);
                case SpecialField::AQ:
                case SpecialField::CSR:
                case SpecialField::FM:
                case SpecialField::NF:
                case SpecialField::PRED:
                case SpecialField::HINT:
                case SpecialField::RL:
                case SpecialField::RM:
                case SpecialField::SUCC:
                case SpecialField::VM:
                case SpecialField::WD:
                case SpecialField::__N:
                    return ExtractorBase::getSpecialField(sfid, icode);
            }
            return 0;
        }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return extract_(Form_V_vsetivli::idType::IMM10, icode & ~fixed_field_mask_);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\tx"
               << extract_(Form_V_vsetivli::idType::RD, icode & ~fixed_field_mask_)
               << ",avl=" << std::dec << getSpecialField(SpecialField::AVL, icode) << ", "
               << dasmVsetImmediate(getImmediate(icode));
            return ss.str();
        }

      private:
        Extractor<Form_V_vsetivli>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask)
        {
        }
    };

    /**
     * V_vsetvl-Form Extractor
     */
    template <> class Extractor<Form_V_vsetvl> : public ExtractorBase<Form_V_vsetvl>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V_vsetvl>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_V_vsetvl::idType::RS1, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_V_vsetvl::idType::RS2, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_V_vsetvl::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_V_vsetvl::idType::RS1, icode,
                                                       fixed_field_mask_);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_V_vsetvl::idType::RS2, icode,
                                                       fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_V_vsetvl::idType::RD, icode,
                                                       fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_V_vsetvl::idType::RS1, false,
                                       suppress_x0);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_V_vsetvl::idType::RS2, false,
                                       suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_V_vsetvl::idType::RD, false,
                                       suppress_x0);
            return olist;
        }

        // TODO: add VM special fields
        uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
        {
            switch (sfid)
            {
                case SpecialField::AQ:
                case SpecialField::AVL:
                case SpecialField::CSR:
                case SpecialField::FM:
                case SpecialField::NF:
                case SpecialField::PRED:
                case SpecialField::HINT:
                case SpecialField::RL:
                case SpecialField::RM:
                case SpecialField::SUCC:
                case SpecialField::VM:
                case SpecialField::WD:
                case SpecialField::__N:
                    return ExtractorBase::getSpecialField(sfid, icode);
            }
            return 0;
        }

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\tx"
               << extract_(Form_V_vsetvl::idType::RD, icode & ~fixed_field_mask_) << ",x"
               << extract_(Form_V_vsetvl::idType::RS1, icode & ~fixed_field_mask_) << ",x"
               << extract_(Form_V_vsetvl::idType::RS2, icode & ~fixed_field_mask_);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(
                      meta, icode, fixed_field_mask_,
                      {{Form_V_vsetvl::idType::RD, InstMetaData::OperandFieldID::RD},
                       {Form_V_vsetvl::idType::RS1, InstMetaData::OperandFieldID::RS1},
                       {Form_V_vsetvl::idType::RS2, InstMetaData::OperandFieldID::RS2}});
            return ss.str();
        }

        // clang-format on

      private:
        Extractor<Form_V_vsetvl>(const uint64_t ffmask, const uint64_t fset) : ExtractorBase(ffmask)
        {
        }
    };

    /**
     * V_uimm6-Form Extractor
     */
    template <> class Extractor<Form_V_uimm6> : public ExtractorBase<Form_V_uimm6>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V_uimm6>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_V_uimm6::idType::RS2, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_V_uimm6::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getSourceRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_V_uimm6::idType::RS2, icode,
                                                       fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return getDestRegs(icode);
            }
            else
            {
                uint64_t result = 0;
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_V_uimm6::idType::RD, icode,
                                                       fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_V_uimm6::idType::RS2, false,
                                       suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                       fixed_field_mask_, Form_V_uimm6::idType::RD, false,
                                       suppress_x0);
            return olist;
        }

        // TODO: add VM special fields
        uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
        {
            switch (sfid)
            {
                // TODO: All forms should be using this pattern...
                case SpecialField::VM:
                    if (isMaskedField_(Form_V_uimm6::idType::VM, fixed_field_mask_))
                    {
                        throw UnsupportedExtractorSpecialFieldID("VM", icode);
                    }
                    else
                    {
                        return extract_(Form_V_uimm6::idType::VM, icode);
                    }
                case SpecialField::RM:
                case SpecialField::AQ:
                case SpecialField::AVL:
                case SpecialField::CSR:
                case SpecialField::FM:
                case SpecialField::NF:
                case SpecialField::PRED:
                case SpecialField::HINT:
                case SpecialField::RL:
                case SpecialField::SUCC:
                case SpecialField::WD:
                case SpecialField::__N:
                    return ExtractorBase::getSpecialField(sfid, icode);
            }
            return 0;
        }

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\tv"
               << extract_(Form_V_uimm6::idType::RD, icode & ~fixed_field_mask_) << ",v"
               << extract_(Form_V_uimm6::idType::RS2, icode & ~fixed_field_mask_)
               << ", IMM=" << std::dec << getImmediate(icode);
            // Show the vm operand if masking mode is on
            if (!isMaskedField_(Form_V_uimm6::idType::VM, fixed_field_mask_))
            {
                if (!extract_(Form_V_uimm6::idType::VM, icode))
                {
                    ss << ",v0.t";
                }
            }
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(
                      meta, icode, fixed_field_mask_,
                      {{Form_V_uimm6::idType::RD, InstMetaData::OperandFieldID::RD},
                       {Form_V_uimm6::idType::RS2, InstMetaData::OperandFieldID::RS2}})
               << ", IMM=" << std::dec << getImmediate(icode);
            // Show the vm operand if masking mode is on
            if (!isMaskedField_(Form_V_uimm6::idType::VM, fixed_field_mask_))
            {
                if (!extract_(Form_V_uimm6::idType::VM, icode))
                {
                    ss << ",v0.t";
                }
            }
            return ss.str();
        }

        // clang-format on

        uint64_t getImmediate(const Opcode icode) const override
        {
            return (extract_(Form_V_uimm6::idType::I5, icode & ~fixed_field_mask_) << 5)
                   | (extract_(Form_V_uimm6::idType::UIMM5, icode & ~fixed_field_mask_));
            return 0;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::UNSIGNED; }

      protected:
        Extractor<Form_V_uimm6>(const uint64_t ffmask, const uint64_t fset) :
            ExtractorBase(ffmask),
            fixed_field_set_(fset)
        {
        }

        const uint64_t fixed_field_set_ = 0;
    };
} // namespace mavis
