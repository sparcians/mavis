#pragma once

#include "mavis/Extractor.h"
#include "mavis/DecoderConsts.h"
#include "mavis/FormStub.h"
#include "impl/forms/ExtractorForms.h"

namespace mavis
{

    /**
     * Derivative of Form_I extractor for MV (addi overlay)
     */
    template <> class Extractor<Form_I_mv> : public Extractor<Form_I>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_I_mv>(ffmask, fset));
        }

        std::string getName() const override { return Form_I_mv::name; }

        ImmediateType getImmediateType() const override { return ImmediateType::NONE; }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_I>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_I extractor for LOADS
     * NOTE: rs1 is the address base, rd is the dest data
     */
    template <> class Extractor<Form_I_load> : public Extractor<Form_I>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_I_load>(ffmask, fset));
        }

        std::string getName() const override { return Form_I_load::name; }

        uint64_t getSourceAddressRegs(const Opcode icode) const override
        {
            return getSourceRegs(icode);
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_I>(ffmask, fset) {}

        friend Extractor<Form_I_load_pair>;
    };

    /**
     * Derivative of Form_I_load extractor for LOAD pairs in RV32 NOTE:
     * rs1 is the address base, rd is the dest data starting register.
     * The second register added will be rd+1.  Also, checks for alignment
     * of those registers.
     */
    template <> class Extractor<Form_I_load_pair> : public Extractor<Form_I_load>
    {
      public:
        Extractor() = default;

        bool isIllop(Opcode icode) const override
        {
            // The load pair instruction is illegal if the first operand
            // is odd
            const uint32_t reg = extract_(Form_I::idType::RD, icode);
            return (reg & 0b1) != 0;
        }

        // Take the standard load and append a second destination to it.
        uint64_t getDestRegs(const Opcode icode) const override
        {
            uint64_t dest_mask = 0;
            if (const uint32_t reg = extract_(Form_I::idType::RD, icode); reg != REGISTER_X0)
            {
                dest_mask = extractUnmaskedIndexBit_(Form_I::idType::RD, icode, fixed_field_mask_);

                const uint32_t rd_val_pos = 64 - __builtin_clzll(dest_mask);
                dest_mask |= (0x1ull << rd_val_pos);
            }
            return dest_mask;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            if (const uint32_t reg = extract_(Form_I::idType::RD, icode); reg != REGISTER_X0)
            {
                olist = Extractor<Form_I_load>::getDestOperandInfo(icode, meta, suppress_x0);
                auto rd2_elem = olist.getElements().at(0);
                rd2_elem.field_id = InstMetaData::OperandFieldID::RD2;
                ++rd2_elem.field_value;

                // Add the second RD
                olist.addElement(rd2_elem);
            }
            return olist;
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_I_load>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C0 extractor for LOADS
     */
    template <> class Extractor<Form_C0_load> : public Extractor<Form_C0>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0_load>(ffmask, fset));
        }

        std::string getName() const override { return Form_C0_load::name; }

        uint64_t getSourceAddressRegs(const Opcode icode) const override
        {
            return getSourceRegs(icode);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extractCompressedRegister_(Form_C0::idType::RD, icode & ~fixed_field_mask_) << ","
               << extractCompressedRegister_(Form_C0::idType::RS1, icode & ~fixed_field_mask_)
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
                      {{Form_C0::idType::RD, InstMetaData::OperandFieldID::RD},
                       {Form_C0::idType::RS1, InstMetaData::OperandFieldID::RS1}})
               << ", IMM=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_C0>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_C0 extractor for LOAD BYTE (part of ZCB extension)
     */
    template <> class Extractor<Form_C0_load_byte> : public Extractor<Form_C0_load>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0_load_byte>(ffmask, fset));
        }

        std::string getName() const override { return Form_C0_load_byte::name; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm = extract_(Form_C0::idType::IMM2, icode);
            using R = Swizzler::Range;
            // Bit ranges to extract from imm, starting with LSB
            return Swizzler::extract(imm, R{1}, R{0});
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C0_load>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C0 extractor for LOAD HALF (part of ZCB extension)
     */
    template <> class Extractor<Form_C0_load_half> : public Extractor<Form_C0_load>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0_load_half>(ffmask, fset));
        }

        std::string getName() const override { return Form_C0_load_half::name; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return (extract_(Form_C0::idType::IMM2, icode) & 0x1ull) << 1;
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C0_load>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C0_load extractor for LOAD WORDS
     */
    template <> class Extractor<Form_C0_load_word> : public Extractor<Form_C0_load>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0_load_word>(ffmask, fset));
        }

        std::string getName() const override { return Form_C0_load_word::name; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm = (extract_(Form_C0::idType::IMM3, icode) << 2ull)
                                 | extract_(Form_C0::idType::IMM2, icode);
            using R = Swizzler::Range;
            // Bit ranges to extract from imm, starting with LSB
            return Swizzler::extract(imm, R{6}, R{2}, R{3, 5});
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C0_load>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C0 extractor for LOAD DOUBLES
     */
    template <> class Extractor<Form_C0_load_double> : public Extractor<Form_C0_load>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0_load_double>(ffmask, fset));
        }

        std::string getName() const override { return Form_C0_load_double::name; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm = (extract_(Form_C0::idType::IMM3, icode) << 2ull)
                                 | extract_(Form_C0::idType::IMM2, icode);
            using R = Swizzler::Range;
            // Bit ranges to extract from imm, starting with LSB
            return Swizzler::extract(imm, R{6, 7}, R{3, 5});
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C0_load>(ffmask, fset)
        {
        }
        friend class Extractor<Form_C0_load_word_pair>;
    };

    /**
     * Derivative of Form_C0_load_double extractor for LOAD WORD PAIRS (RV32)
     */
    template <> class Extractor<Form_C0_load_word_pair> : public Extractor<Form_C0_load_double>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0_load_word_pair>(ffmask, fset));
        }

        std::string getName() const override { return Form_C0_load_word_pair::name; }

        bool isIllop(Opcode icode) const override
        {
            // The load pair instruction is illegal if the first operand
            // is odd
            const uint32_t reg = extract_(Form_C0::idType::RD, icode);
            return (reg & 0b1) != 0;
        }

        // Take the standard load and append a second destination to it.
        uint64_t getDestRegs(const Opcode icode) const override
        {
            uint64_t dest_mask =
                extractUnmaskedCompressedIndexBit_(Form_C0::idType::RD, icode, fixed_field_mask_);

            const uint32_t rd_val_pos = 64 - __builtin_clzll(dest_mask);
            dest_mask |= (0x1ull << rd_val_pos);
            return dest_mask;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RD,
                                                 fixed_field_mask_, Form_C0::idType::RD, false);
            auto rd2_elem = olist.getElements().at(0);
            rd2_elem.field_id = InstMetaData::OperandFieldID::RD2;
            ++rd2_elem.field_value;

            // Add the second RD
            olist.addElement(rd2_elem);
            return olist;
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C0_load_double>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of the Form_S extractor for Store Pair, RV32
     */
    template <> class Extractor<Form_S_Pair> : public Extractor<Form_S>
    {
      public:
        Extractor() = default;

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            // The mask for all of the source regs (addr + data) is
            // actually the original RS1/RS2 | RS3, but RS3 is not part of
            // the icode
            return Extractor<Form_S>::getSourceRegs(icode) | getSourceDataRegs(icode);
        }

        bool isIllop(Opcode icode) const override
        {
            // The load pair instruction is illegal if the first operand
            // is odd
            const uint32_t reg = extract_(Form_S::idType::RS2, icode);
            return (reg & 0b1) != 0;
        }

        uint64_t getSourceDataRegs(const Opcode icode) const override
        {
            // Add a second source to the pair
            uint64_t src_mask = 0;
            if (const uint32_t reg = extract_(Form_S::idType::RS2, icode); reg != REGISTER_X0)
            {
                src_mask = extractUnmaskedIndexBit_(Form_S::idType::RS2, icode, fixed_field_mask_);
                const uint32_t rs2_val_pos = 64 - __builtin_clzll(src_mask);
                src_mask |= (0x1ull << rs2_val_pos);
            }
            return src_mask;
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_S::idType::RS1, false, suppress_x0);
            if (const uint32_t reg = extract_(Form_S::idType::RS2, icode); reg != REGISTER_X0)
            {
                appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                           fixed_field_mask_, Form_S::idType::RS2, false,
                                           suppress_x0);
                auto rs3_elem = olist.getElements().at(1);
                assert(rs3_elem.field_id == InstMetaData::OperandFieldID::RS2);

                rs3_elem.field_id = InstMetaData::OperandFieldID::RS3;
                ++rs3_elem.field_value;

                // Add the second RS
                olist.addElement(rs3_elem);
            }
            return olist;
        }
    };

    /**
     * Derivative of Form_C0 extractor for STORES
     * NOTE: rs1 is the address base, rs2 is the source data
     */
    template <> class Extractor<Form_C0_store> : public Extractor<Form_C0>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0_store>(ffmask, fset));
        }

        std::string getName() const override { return Form_C0_store::name; }

        uint64_t getDestRegs(const uint64_t icode) const override { return 0; }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_C0::idType::RS1, icode,
                                                      fixed_field_mask_)
                   | extractUnmaskedCompressedIndexBit_(Form_C0::idType::RD, icode,
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
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |= extractUnmaskedCompressedIndexBit_(Form_C0::idType::RD, icode,
                                                                 fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getSourceAddressRegs(const uint64_t icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_C0::idType::RS1, icode,
                                                      fixed_field_mask_);
        }

        // RS2 is in Form_C0's RD slot
        uint64_t getSourceDataRegs(const uint64_t icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_C0::idType::RD, icode,
                                                      fixed_field_mask_);
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RS1,
                                                 fixed_field_mask_, Form_C0::idType::RS1, false);
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RS2,
                                                 fixed_field_mask_, Form_C0::idType::RD, true);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType &,
                                       bool suppress_x0 = false) const override
        {
            return {};
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const uint64_t icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extractCompressedRegister_(Form_C0::idType::RD, icode & ~fixed_field_mask_) << ","
               << extractCompressedRegister_(Form_C0::idType::RS1, icode & ~fixed_field_mask_)
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
                      {{Form_C0::idType::RD, InstMetaData::OperandFieldID::RS2},
                       {Form_C0::idType::RS1, InstMetaData::OperandFieldID::RS1}})
               << ", IMM=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_C0>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_C0_store> extractor for STORE BYTE (ZCB extension)
     * NOTE: rs1 is the address base, rs2 is the source data
     */
    template <> class Extractor<Form_C0_store_byte> : public Extractor<Form_C0_store>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0_store_byte>(ffmask, fset));
        }

        std::string getName() const override { return Form_C0_store_byte::name; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm = extract_(Form_C0::idType::IMM2, icode);
            using R = Swizzler::Range;
            // Bit ranges to extract from imm, starting with LSB
            return Swizzler::extract(imm, R{1}, R{0});
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C0_store>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C0_store extractor for STORE HALF (ZCB extension)
     * NOTE: rs1 is the address base, rs2 is the source data
     */
    template <> class Extractor<Form_C0_store_half> : public Extractor<Form_C0_store>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0_store_half>(ffmask, fset));
        }

        std::string getName() const override { return Form_C0_store_half::name; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return (extract_(Form_C0::idType::IMM2, icode) & 0x1ull) << 1;
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C0_store>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C0_store extractor for STORE WORDS
     * NOTE: rs1 is the address base, rs2 is the source data
     */
    template <> class Extractor<Form_C0_store_word> : public Extractor<Form_C0_store>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0_store_word>(ffmask, fset));
        }

        std::string getName() const override { return Form_C0_store_word::name; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm = (extract_(Form_C0::idType::IMM3, icode) << 2ull)
                                 | extract_(Form_C0::idType::IMM2, icode);
            using R = Swizzler::Range;
            // Bit ranges to extract from imm, starting with LSB
            return Swizzler::extract(imm, R{6}, R{2}, R{3, 5});
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C0_store>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C0_store> extractor for STORE DOUBLES
     * NOTE: rs1 is the address base, rs2 is the source data
     */
    template <> class Extractor<Form_C0_store_double> : public Extractor<Form_C0_store>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0_store_double>(ffmask, fset));
        }

        std::string getName() const override { return Form_C0_store_double::name; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm = (extract_(Form_C0::idType::IMM3, icode) << 2ull)
                                 | extract_(Form_C0::idType::IMM2, icode);
            using R = Swizzler::Range;
            // Bit ranges to extract from imm, starting with LSB
            return Swizzler::extract(imm, R{6, 7}, R{3, 5});
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C0_store>(ffmask, fset)
        {
        }

        friend Extractor<Form_C0_store_word_pair>;
    };

    /**
     * Derivative of Form_C0_store_double extractor for STORE WORD PAIR RV32
     * NOTE: rs1 is the address base, rs2 and rs2+1 are the source data
     */
    template <> class Extractor<Form_C0_store_word_pair> : public Extractor<Form_C0_store_double>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C0_store_word_pair>(ffmask, fset));
        }

        std::string getName() const override { return Form_C0_store_word_pair::name; }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            // The mask for all of the source regs (addr + data) is
            // actually the original RS1/RS2 | RS3, but RS3 is not part of
            // the icode
            return Extractor<Form_C0_store>::getSourceRegs(icode) | getSourceDataRegs(icode);
        }

        bool isIllop(Opcode icode) const override
        {
            // The store pair instruction is illegal if the rs2 operand
            // is odd
            // RS2 is in Form_C0's RD slot
            const uint32_t reg = extract_(Form_C0::idType::RD, icode);
            return (reg & 0b1) != 0;
        }

        uint64_t getSourceDataRegs(const Opcode icode) const override
        {
            // Add a second source to the pair
            // RS2 is in Form_C0's RD slot
            uint64_t src_mask =
                extractUnmaskedCompressedIndexBit_(Form_C0::idType::RD, icode, fixed_field_mask_);
            const uint32_t rs2_val_pos = 64 - __builtin_clzll(src_mask);
            src_mask |= (0x1ull << rs2_val_pos);
            return src_mask;
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist =
                Extractor<Form_C0_store>::getSourceOperandInfo(icode, meta, suppress_x0);

            auto rs3_elem = olist.getElements().at(1);
            // RS2 is in Form_C0's RD slot
            assert(rs3_elem.field_id == InstMetaData::OperandFieldID::RS2);

            rs3_elem.field_id = InstMetaData::OperandFieldID::RS3;
            ++rs3_elem.field_value;

            // Add the second RS
            olist.addElement(rs3_elem);
            return olist;
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C0_store_double>(ffmask, fset)
        {
        }
    };

    /**
     * C1_rsd-Form Extractor
     */
    template <> class Extractor<Form_C1_rsd> : public ExtractorBase<Form_C1>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C1_rsd>(ffmask, fset));
        }

        std::string getName() const override { return Form_C1_rsd::name; }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_C1::idType::RS1, icode,
                                                      fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return extractUnmaskedCompressedIndexBit_(Form_C1::idType::RD, icode,
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
                    result |= extractUnmaskedCompressedIndexBit_(Form_C1::idType::RS1, icode,
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
                    result |= extractUnmaskedCompressedIndexBit_(Form_C1::idType::RD, icode,
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
                                                 fixed_field_mask_, Form_C1::idType::RS1, false);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RD,
                                                 fixed_field_mask_, Form_C1::idType::RD, false);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extractCompressedRegister_(Form_C1::idType::RD, icode & ~fixed_field_mask_) << ","
               << extractCompressedRegister_(Form_C1::idType::RS1, icode & ~fixed_field_mask_);
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
                      {{Form_C1::idType::RD, InstMetaData::OperandFieldID::RD},
                       {Form_C1::idType::RS1, InstMetaData::OperandFieldID::RS1}});
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            fixed_field_mask_(ffmask),
            fixed_field_set_(fset)
        {
        }

        uint64_t fixed_field_mask_ = 0;
        uint64_t fixed_field_set_ = 0;
    };

    /**
     * Derivative of Form_C1_rsd> extractor with implied 0 immediate
     */
    template <> class Extractor<Form_C1_rsd_I0> : public Extractor<Form_C1_rsd>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C1_rsd_I0>(ffmask, fset));
        }

        std::string getName() const override { return Form_C1_rsd_I0::name; }

        ImmediateType getImmediateType() const override { return ImmediateType::UNSIGNED; }

        uint64_t getImmediate(const Opcode icode) const override { return 0; }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_C1_rsd>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C1_rsd> extractor with implied 0 immediate for ZEXT[.hw]
     * RS2 is fixed for these encodings, and should be extracted as X0
     */
    template <> class Extractor<Form_C1_rsd_zext_I0> : public Extractor<Form_C1_rsd>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C1_rsd_zext_I0>(ffmask, fset));
        }

        std::string getName() const override { return Form_C1_rsd_zext_I0::name; }

        uint64_t getSourceRegs(const uint64_t icode) const override
        {
            return Extractor<Form_C1_rsd>::getSourceRegs(icode) | (1ull << REGISTER_X0);
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
                return Extractor<Form_C1_rsd>::getSourceOperTypeRegs(icode, meta, kind)
                       | (1ull << REGISTER_X0);
            }
            else
            {
                uint64_t result = Extractor<Form_C1_rsd>::getSourceOperTypeRegs(icode, meta, kind);
                // Use RS's type as the type of the X0 source register
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= (1ull << REGISTER_X0);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist =
                Extractor<Form_C1_rsd>::getSourceOperandInfo(icode, meta, suppress_x0);
            olist.addElement(InstMetaData::OperandFieldID::RS2,
                             meta->getOperandType(InstMetaData::OperandFieldID::RS1), REGISTER_X0,
                             false);
            return olist;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::UNSIGNED; }

        uint64_t getImmediate(const Opcode icode) const override { return 0; }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_C1_rsd>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C1_rsd> extractor with implied -1 immediate
     */
    template <> class Extractor<Form_C1_rsd_Ineg1> : public Extractor<Form_C1_rsd>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C1_rsd_Ineg1>(ffmask, fset));
        }

        std::string getName() const override { return Form_C1_rsd_Ineg1::name; }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        uint64_t getImmediate(const Opcode icode) const override { return -1ull; }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_C1_rsd>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C1_rsd> extractor with implied 0xFF immediate
     */
    template <> class Extractor<Form_C1_rsd_I0xFF> : public Extractor<Form_C1_rsd>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C1_rsd_I0xFF>(ffmask, fset));
        }

        std::string getName() const override { return Form_C1_rsd_I0xFF::name; }

        ImmediateType getImmediateType() const override { return ImmediateType::UNSIGNED; }

        uint64_t getImmediate(const Opcode icode) const override { return 0xFF; }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_C1_rsd>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_CI extractor for c.addi (hint special cases)
     */
    template <> class Extractor<Form_CI_addi> : public Extractor<Form_CI>
    {
      public:
        Extractor() = default;

        bool isHint(Opcode icode) const override { return getImmediate(icode) == 0; }

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CI_addi>(ffmask, fset));
        }

        std::string getName() const override { return Form_CI_addi::name; }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_CI>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_CI extractor for c.addiw (illop special cases)
     */
    template <> class Extractor<Form_CI_addiw> : public Extractor<Form_CI>
    {
      public:
        Extractor() = default;

        bool isIllop(Opcode icode) const override
        {
            return (getDestRegs(icode) & (1ull << REGISTER_X0)) != 0;
        }

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CI_addiw>(ffmask, fset));
        }

        std::string getName() const override { return Form_CI_addiw::name; }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_CI>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_CI extractor with SP source and destination
     * SP is explicitly encoded (c.addi16sp)
     */
    template <> class Extractor<Form_CI_sp> : public Extractor<Form_CI>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CI_sp>(ffmask, fset));
        }

        bool isIllop(Opcode icode) const override
        {
            // Don't call getImmediate() here, since we're just checking for 0
            // in both fields and don't need to shift or swizzle
            return ((extract_(Form_CI::idType::IMM1, icode) == 0)
                    && (extract_(Form_CI::idType::IMM5, icode) == 0));
        }

        std::string getName() const override { return Form_CI_sp::name; }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm = (extract_(Form_CI::idType::IMM1, icode) << 5ull)
                                 | extract_(Form_CI::idType::IMM5, icode);
            using R = Swizzler::Range;
            // Bit ranges to extract from imm, starting with LSB
            return Swizzler::extract(imm, R{5}, R{7, 8}, R{6}, R{4}, R{9});
        }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 9);
        }

      private:
        static uint64_t generateFFMask(uint64_t ffmask)
        {
            // Re-enable the fixed fields for RS1 and RD in the parent's mask
            // This will keep the parent from ignoring SP (encoded in RS1/RD)
            // for getSourceRegs(), getDestRegs(), getOperTypeRegs(), and dasmString()
            ffmask &= ~(Form_CI::fields[Form_CI::idType::RS1].getShiftedMask()
                        | Form_CI::fields[Form_CI::idType::RD].getShiftedMask());
            ffmask &= ~((1ull << static_cast<uint32_t>(Form_CI::idType::RS1))
                        | (1ull << static_cast<uint32_t>(Form_CI::idType::RD)));
            return ffmask;
        }

        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_CI>(generateFFMask(ffmask), fset)
        {
        }
    };

    /**
     * Derivative of Form_CIW extractor with SP source
     * SP is IMPLICITLY encoded
     *
     * Used by c.addi4spn
     */
    template <> class Extractor<Form_CIW_sp> : public Extractor<Form_CIW>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CIW_sp>(ffmask, fset));
        }

        bool isIllop(Opcode icode) const override
        {
            // Don't call getImmediate() here, since we're just checking for 0 and don't
            // need to swizzle
            return extract_(Form_CIW::idType::IMM8, icode & ~fixed_field_mask_) == 0;
        }

        std::string getName() const override { return Form_CIW_sp::name; }

        // TODO Form_CIW (base class) has no sources, so this is overkill...
        uint64_t getSourceRegs(const uint64_t icode) const override
        {
            return (Extractor<Form_CIW>::getSourceRegs(icode) | (1ull << REGISTER_SP))
                   & ~0x1ull; // add SP, remove X0
        }

        // RegListType getSourceList(const Opcode icode, bool suppress_x0 = false) const override
        //{
        //     return {REGISTER_SP};
        // }

        uint64_t getSourceOperTypeRegs(const uint64_t icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return (Extractor<Form_CIW>::getSourceOperTypeRegs(icode, meta, kind)
                        | (1ull << REGISTER_SP))
                       & ~0x1ull;
            }
            else
            {
                uint64_t result = Extractor<Form_CIW>::getSourceOperTypeRegs(icode, meta, kind);
                // RD is the only register we have in this extraction form (c.addi4spn)
                // Use RD's type as the type of the SP source register
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |= (1ull << REGISTER_SP);
                    result &= ~0x1ull; // remove x0
                }
                return result;
            }
        }

        // RegListType getSourceOperTypeList(const Opcode icode,
        //                                   const InstMetaData::PtrType &meta,
        //                                   InstMetaData::OperandTypes kind, bool suppress_x0 =
        //                                   false) const override
        //{
        //     if (meta->isNoneOperandType(kind)) {
        //         return {};
        //     } else if (meta->isAllOperandType(kind)) {
        //         return getSourceList(icode, suppress_x0);
        //     } else {
        //         if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind)) {
        //  RD is the only register we have in this extraction form (c.addi4spn)
        //  Use RD's type as the type of the SP source register
        // return {REGISTER_SP};
        // }
        // return {};
        // }
        // }

        OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            olist.addElement(InstMetaData::OperandFieldID::RS1,
                             meta->getOperandType(InstMetaData::OperandFieldID::RD), REGISTER_SP,
                             false);
            return olist;
        }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm = extract_(Form_CIW::idType::IMM8, icode & ~fixed_field_mask_);
            using R = Swizzler::Range;
            return Swizzler::extract(imm, R{3}, R{2}, R{6, 9}, R{4, 5});
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const uint64_t icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extractCompressedRegister_(Form_CIW::idType::RD, icode)
               << ", SP, IMM=" << std::dec << getImmediate(icode);
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
               << ", SP, IMM=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format on

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_CIW>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_CIX extractor for c.andi (hint special cases)
     */
    template <> class Extractor<Form_CIX_andi> : public Extractor<Form_CIX>
    {
      public:
        Extractor() = default;

        bool isHint(Opcode icode) const override { return false; }

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CIX_andi>(ffmask, fset));
        }

        std::string getName() const override { return Form_CIX_andi::name; }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 5);
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_CIX>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_CJ extractor with x1 (link) destination
     */
    template <> class Extractor<Form_CJAL> : public Extractor<Form_CJ>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CJAL>(ffmask, fset));
        }

        std::string getName() const override { return Form_CJAL::name; }

        uint64_t getDestRegs(const uint64_t) const override { return (1ull << REGISTER_LINK); }

        uint64_t getDestOperTypeRegs(const uint64_t icode, const InstMetaData::PtrType & meta,
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
                    return (0x1ull << REGISTER_LINK);
                }
                else
                {
                    return 0;
                }
            }
        }

        OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            olist.addElement(InstMetaData::OperandFieldID::RD,
                             meta->getOperandType(InstMetaData::OperandFieldID::RS1), REGISTER_LINK,
                             false);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const uint64_t icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << "x1, +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_CJ>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_CJR extractor with x1 (link) destination
     * IMPLICITLY encoded
     */
    template <> class Extractor<Form_CJALR> : public Extractor<Form_CJR>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CJALR>(ffmask, fset));
        }

        std::string getName() const override { return Form_CJALR::name; }

        uint64_t getDestRegs(const uint64_t) const override { return (1ull << REGISTER_LINK); }

        // RegListType getDestList(const Opcode icode, bool suppress_x0 = false) const override
        //{
        //     return {REGISTER_LINK};
        // }

        // TODO: Form_CJR (base class) has no destinations, this is overkill...
        uint64_t getDestOperTypeRegs(const uint64_t icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isNoneOperandType(kind))
            {
                return 0;
            }
            else if (meta->isAllOperandType(kind))
            {
                return (Extractor<Form_CJR>::getDestOperTypeRegs(icode, meta, kind)
                        | (1ull << REGISTER_LINK))
                       & ~0x1ull;
            }
            else
            {
                uint64_t result = Extractor<Form_CJR>::getDestOperTypeRegs(icode, meta, kind);
                // This form has an implied LINK field (c.jalr)
                // Use the type of RS1 for the type of LINK
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= (1ull << REGISTER_LINK);
                    result &= ~0x1ull; // remove x0
                }
                return result;
            }
        }

        // RegListType getDestOperTypeList(const Opcode icode,
        //                                 const InstMetaData::PtrType &meta,
        //                                 InstMetaData::OperandTypes kind, bool suppress_x0 =
        //                                 false) const override
        //{
        //     if (meta->isNoneOperandType(kind)) {
        //         return {};
        //     } else if (meta->isAllOperandType(kind)) {
        //         return getDestList(icode, suppress_x0);
        //     } else {
        //  This form has an implied LINK field (c.jalr)
        //  Use the type of RS1 for the type of LINK
        // if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind)) {
        //     return {REGISTER_LINK};
        // }
        // return {};
        // }
        // }

        OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            olist.addElement(InstMetaData::OperandFieldID::RD,
                             meta->getOperandType(InstMetaData::OperandFieldID::RS1), REGISTER_LINK,
                             false);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const uint64_t icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t " << REGISTER_LINK << ", "
               << extract_(Form_CJR::idType::RS1, icode) << ", IMM=" << std::dec
               << getImmediate(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t x" << REGISTER_LINK << ", "
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_CJR::idType::RS1, InstMetaData::OperandFieldID::RS1}})
               << ", IMM=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format on

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_CJR>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_C2 extractor for c.add (hint special case)
     */
    template <> class Extractor<Form_C2_add> : public Extractor<Form_C2>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_add>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_add::name; }

        bool isHint(Opcode icode) const override
        {
            return (getDestRegs(icode) & (1ull << REGISTER_X0)) != 0;
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_C2::idType::RS1, icode, fixed_field_mask_)
                   | // Alias for rD
                   extractUnmaskedIndexBit_(Form_C2::idType::RS2, icode,
                                            fixed_field_mask_); // Alias for rS
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
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_C2::idType::RS1, icode, fixed_field_mask_);
                }
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS2, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_C2::idType::RS2, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_C2::idType::RS1,
                                       false); // Could also use RD here
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_C2::idType::RS2, false);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_C2::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_C2::idType::RS1, icode & ~fixed_field_mask_) << ","
               << extract_(Form_C2::idType::RS2, icode & ~fixed_field_mask_);
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
                                      {Form_C2::idType::RS1, InstMetaData::OperandFieldID::RS1},
                                      {Form_C2::idType::RS2, InstMetaData::OperandFieldID::RS2}});
            return ss.str();
        }

        // clang-format on

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_C2>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_C2 extractor with implicit x0 source
     * For c.mv extraction
     *
     * NOTE: the x0 implicit source insertion is not being used right
     * now.
     */
    template <> class Extractor<Form_C2_mv> : public Extractor<Form_C2>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_mv>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_mv::name; }

        bool isHint(Opcode icode) const override
        {
            return (getDestRegs(icode) & (1ull << REGISTER_X0)) != 0;
        }

        uint64_t getSourceRegs(const uint64_t icode) const override
        {
            return (Extractor<Form_C2>::getSourceRegs(icode) | (0x1ull << REGISTER_X0));
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
                return (Extractor<Form_C2>::getSourceOperTypeRegs(icode, meta, kind)
                        | (1ull << REGISTER_X0));
            }
            else
            {
                uint64_t result = Extractor<Form_C2>::getSourceOperTypeRegs(icode, meta, kind);
                // This form has an implied x0 source register
                // Use the type of RS1 for the type of x0
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= (1ull << REGISTER_X0);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            olist.addElement(InstMetaData::OperandFieldID::RS1,
                             meta->getOperandType(InstMetaData::OperandFieldID::RS1), REGISTER_X0,
                             false);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_C2::idType::RS, false, suppress_x0);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const uint64_t icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_C2::idType::RD, icode) << ", "
               << extract_(Form_C2::idType::RS2, icode); // RS2 == RS
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_C2::idType::RD, InstMetaData::OperandFieldID::RD}})
               << ", "
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_C2::idType::RS2, InstMetaData::OperandFieldID::RS2}});
            return ss.str();
        }

        // clang-format on

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_C2>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_C2 extractor for c.slli (hint special case)
     *
     * NOTE: RS1 is an alias for RD
     */
    template <> class Extractor<Form_C2_slli> : public Extractor<Form_C2>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_slli>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_slli::name; }

        bool isHint(Opcode icode) const override
        {
            return ((getDestRegs(icode) & (1ull << REGISTER_X0)) != 0)
                   || (getImmediate(icode) == 0);
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_C2::idType::RS1, icode, fixed_field_mask_);
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
                        extractUnmaskedIndexBit_(Form_C2::idType::RS1, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_C2::idType::RS1, false, suppress_x0);
            return olist;
        }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return (extract_(Form_C2::idType::SHAMT1, icode) << 5ull)
                   | extract_(Form_C2::idType::SHAMT5, icode);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_C2::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_C2::idType::RS1, icode & ~fixed_field_mask_)
               << ", IMM=" << std::dec << getImmediate(icode);
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
                                      {Form_C2::idType::RS1, InstMetaData::OperandFieldID::RS1}})
               << ", IMM=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format on

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_C2>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_C2 extractor with SP source
     * SP is IMPLICITLY encoded
     *
     * This is a base class for C2_sp_load and C2_sp_store
     */
    template <> class Extractor<Form_C2_sp> : public Extractor<Form_C2>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_sp>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_sp::name; }

        uint64_t getSourceRegs(const uint64_t icode) const override
        {
            return (Extractor<Form_C2>::getSourceRegs(icode) | (1ull << REGISTER_SP))
                   & ~0x1ull; // add SP, remove X0
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
                return (Extractor<Form_C2>::getSourceOperTypeRegs(icode, meta, kind)
                        | (1ull << REGISTER_SP))
                       & ~0x1ull;
            }
            else
            {
                uint64_t result = Extractor<Form_C2>::getSourceOperTypeRegs(icode, meta, kind);
                // This form has an implied SP field
                // Use the type of RS1 for the type of SP
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS1, kind))
                {
                    result |= (1ull << REGISTER_SP);
                    result &= ~0x1ull; // remove x0
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            olist.addElement(InstMetaData::OperandFieldID::RS1,
                             meta->getOperandType(InstMetaData::OperandFieldID::RS1), REGISTER_SP,
                             false);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_C2::idType::RS, false, suppress_x0);
            return olist;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::UNSIGNED; }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const uint64_t icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_C2::idType::RD, icode)
               << ", SP, IMM=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_C2::idType::RD, InstMetaData::OperandFieldID::RD}})
               << ", SP, IMM=" << std::dec << getImmediate(icode);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_C2>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_C2_sp> extractor for LOADS
     */
    template <> class Extractor<Form_C2_sp_load> : public Extractor<Form_C2_sp>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_sp_load>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_sp_load::name; }

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

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            olist.addElement(InstMetaData::OperandFieldID::RS1,
                             meta->getOperandType(InstMetaData::OperandFieldID::RS1), REGISTER_SP,
                             false);
            return olist;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::UNSIGNED; }

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_C2_sp>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C2_sp_load extractor for LOAD WORD
     */
    template <> class Extractor<Form_C2_sp_load_word> : public Extractor<Form_C2_sp_load>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_word>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_sp_load_word::name; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm = (extract_(Form_C2::idType::SHAMT1, icode) << 5ull)
                                 | extract_(Form_C2::idType::SHAMT5, icode);
            using R = Swizzler::Range;
            // Bit ranges to extract from imm, starting with LSB
            return Swizzler::extract(imm, R{6, 7}, R{2, 4}, R{5});
        }

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C2_sp_load>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C2_sp_load extractor for LOAD DOUBLE
     */
    template <> class Extractor<Form_C2_sp_load_double> : public Extractor<Form_C2_sp_load>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_double>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_sp_load_double::name; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm = (extract_(Form_C2::idType::SHAMT1, icode) << 5ull)
                                 | extract_(Form_C2::idType::SHAMT5, icode);
            using R = Swizzler::Range;
            // Bit ranges to extract from imm, starting with LSB
            return Swizzler::extract(imm, R{6, 8}, R{3, 4}, R{5});
        }

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C2_sp_load>(ffmask, fset)
        {
        }

        friend class Extractor<Form_C2_sp_load_word_pair>;
    };

    /**
     * Derivative of Form_C2_sp_load_double extractor for LOAD WORD PAIR (32-bit)
     */
    template <>
    class Extractor<Form_C2_sp_load_word_pair> : public Extractor<Form_C2_sp_load_double>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_double>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_sp_load_word_pair::name; }

        bool isIllop(Opcode icode) const override
        {
            // The load pair instruction is illegal if the first operand
            // is odd
            const uint32_t reg = extract_(Form_C2::idType::RD, icode);
            return (reg & 0b1) != 0;
        }

        // Take the standard load and append a second destination to it.
        uint64_t getDestRegs(const Opcode icode) const override
        {
            uint64_t dest_mask = 0;
            if (const uint32_t reg = extract_(Form_C2::idType::RD, icode); reg != REGISTER_X0)
            {
                dest_mask = extractUnmaskedIndexBit_(Form_C2::idType::RD, icode, fixed_field_mask_);

                const uint32_t rd_val_pos = 64 - __builtin_clzll(dest_mask);
                dest_mask |= (0x1ull << rd_val_pos);
            }
            return dest_mask;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            if (const uint32_t reg = extract_(Form_C2::idType::RD, icode); reg != REGISTER_X0)
            {
                olist = Extractor<Form_C2_sp_load>::getDestOperandInfo(icode, meta, suppress_x0);
                auto rd2_elem = olist.getElements().at(0);
                rd2_elem.field_id = InstMetaData::OperandFieldID::RD2;
                ++rd2_elem.field_value;

                // Add the second RD
                olist.addElement(rd2_elem);
            }
            return olist;
        }

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C2_sp_load_double>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C2_sp_load extractor for FP loads (SINGLE PRECISION)
     */
    template <>
    class Extractor<Form_C2_sp_load_float_single> : public Extractor<Form_C2_sp_load_word>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_float_single>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_sp_load_float_single::name; }

        bool isIllop(Opcode) const override { return false; }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C2_sp_load_word>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C2_sp_load extractor for FP loads (DOUBLE PRECISION)
     */
    template <>
    class Extractor<Form_C2_sp_load_float_double> : public Extractor<Form_C2_sp_load_double>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_sp_load_float_double>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_sp_load_float_double::name; }

        bool isIllop(Opcode) const override { return false; }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C2_sp_load_double>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C2_sp_store extractor for STORE WORD
     */
    template <> class Extractor<Form_C2_sp_store_word> : public Extractor<Form_C2_sp_store>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_sp_store_word>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_sp_store_word::name; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm = extract_(Form_C2_sp_store::idType::IMM, icode);
            using R = Swizzler::Range;
            // Bit ranges to extract from imm, starting with LSB
            return Swizzler::extract(imm, R{6, 7}, R{2, 5});
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C2_sp_store>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_C2_sp_store extractor for STORE DOUBLE
     */
    template <> class Extractor<Form_C2_sp_store_double> : public Extractor<Form_C2_sp_store>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_sp_store_double>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_sp_store_double::name; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            const uint64_t imm = extract_(Form_C2_sp_store::idType::IMM, icode);
            using R = Swizzler::Range;
            // Bit ranges to extract from imm, starting with LSB
            return Swizzler::extract(imm, R{6, 8}, R{3, 5});
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C2_sp_store>(ffmask, fset)
        {
        }

        friend class Extractor<Form_C2_sp_store_word_pair>;
    };

    /**
     * Derivative of Form_C2_sp_store_double extractor for STORE WORD PAIR (RV32)
     */
    template <>
    class Extractor<Form_C2_sp_store_word_pair> : public Extractor<Form_C2_sp_store_double>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_C2_sp_store_double>(ffmask, fset));
        }

        std::string getName() const override { return Form_C2_sp_store_word_pair::name; }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            // The mask for all of the source regs (addr + data) is
            // actually the original RS1/RS2 | RS3, but RS3 is not part of
            // the icode
            return Extractor<Form_C2_sp_store_double>::getSourceRegs(icode)
                   | getSourceDataRegs(icode);
        }

        bool isIllop(Opcode icode) const override
        {
            // The load pair instruction is illegal if the first operand
            // is odd
            const uint32_t reg = extract_(Form_C2_sp_store::idType::RS2, icode);
            return (reg & 0b1) != 0;
        }

        uint64_t getSourceDataRegs(const Opcode icode) const override
        {
            // Add a second source to the pair
            uint64_t src_mask = 0;
            if (const uint32_t reg = extract_(Form_C2_sp_store::idType::RS2, icode);
                reg != REGISTER_X0)
            {
                src_mask = extractUnmaskedIndexBit_(Form_C2_sp_store::idType::RS2, icode,
                                                    fixed_field_mask_);
                const uint32_t rs2_val_pos = 64 - __builtin_clzll(src_mask);
                src_mask |= (0x1ull << rs2_val_pos);
            }
            return src_mask;
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            if (const uint32_t reg = extract_(Form_C2_sp_store::idType::RS2, icode);
                reg != REGISTER_X0)
            {
                olist = Extractor<Form_C2_sp_store_double>::getSourceOperandInfo(icode, meta,
                                                                                 suppress_x0);

                auto rs3_elem = olist.getElements().at(1);
                assert(rs3_elem.field_id == InstMetaData::OperandFieldID::RS2);

                rs3_elem.field_id = InstMetaData::OperandFieldID::RS3;
                ++rs3_elem.field_value;

                // Add the second RS
                olist.addElement(rs3_elem);
            }
            return olist;
        }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_C2_sp_store_double>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_CI_rD_only extractor with SP source and destination
     * SP is explicitly encoded
     */
    template <> class Extractor<Form_CI_rD_shifted> : public Extractor<Form_CI_rD_only>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_CI_rD_shifted>(ffmask, fset));
        }

        std::string getName() const override { return Form_CI_rD_shifted::name; }

        bool isIllop(Opcode icode) const override { return getImmediate(icode) == 0; }

        uint64_t getSourceRegs(const Opcode) const override { return 0; }

        // RegListType getSourceList(const Opcode, bool suppress_x0 = false) const override
        //{
        //     return {};
        // }

        uint64_t getSourceOperTypeRegs(const Opcode, const InstMetaData::PtrType &,
                                       InstMetaData::OperandTypes) const override
        {
            return 0;
        }

        // RegListType getSourceOperTypeList(const Opcode,
        //                                   const InstMetaData::PtrType&,
        //                                   InstMetaData::OperandTypes, bool suppress_x0 = false)
        //                                   const override
        //{
        //     return {};
        // }

        OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType &,
                                         bool suppress_x0 = false) const override
        {
            return {};
        }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

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
        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extract_(Form_CI_rD_only::idType::RD, icode & ~fixed_field_mask_) << ", +0x"
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
               << ", +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format on

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) :
            Extractor<Form_CI_rD_only>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of CMPP Extractor that handles cm.push instructions
     */
    template <> class Extractor<Form_CMPP_push> : public Extractor<Form_CMPP>
    {
      public:
        int64_t getSignedOffset(const Opcode icode) const override
        {
            return -1 * getImmediate(icode);
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            // Form_CMPP handles pop-type instructions. On a push, the source and dest regs are
            // swapped
            return Extractor<Form_CMPP>::getDestRegs(icode);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            // Form_CMPP handles pop-type instructions. On a push, the source and dest regs are
            // swapped
            return Extractor<Form_CMPP>::getSourceRegs(icode);
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            const auto op_type = meta->getOperandType(InstMetaData::OperandFieldID::RS1);

            OperandInfo olist;
            olist.addElement(InstMetaData::OperandFieldID::RS1, op_type, REGISTER_SP, false);
            convertRListToOpcodeInfo_(olist, icode, op_type);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            const auto op_type = meta->getOperandType(InstMetaData::OperandFieldID::RS1);

            OperandInfo olist;
            olist.addElement(InstMetaData::OperandFieldID::RD, op_type, REGISTER_SP, false);
            return olist;
        }

      protected:
        InstMetaData::OperandFieldID getFirstOperandID_() const override
        {
            return InstMetaData::OperandFieldID::PUSH_RS1;
        }

        int64_t getStackAdjBase_(const Opcode icode, const uint32_t data_size) const override
        {
            return -1 * Extractor<Form_CMPP>::getStackAdjBase_(icode, data_size);
        }
    };

    /**
     * Derivative of CMPP Extractor that handles cm.popretz instructions
     */
    template <> class Extractor<Form_CMPP_popretz> : public Extractor<Form_CMPP>
    {
      public:
        uint64_t getDestRegs(const Opcode icode) const override
        {
            return Extractor<Form_CMPP>::getDestRegs(icode) | (1ull << 10);
        }
    };

    /**
     * Derivative of CA extractor for cm.mva01s instructions
     */
    template <> class Extractor<Form_CMMV_mva01s> : public Extractor<Form_CA>
    {
      public:
        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return Extractor<Form_CA>::getSourceRegs(icode);
        }

        uint64_t getDestRegs(const Opcode) const override
        {
            // Dests are always a0 and a1
            return (1ull << 10) | (1ull << 11);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isAllOperandType(kind) || (kind == InstMetaData::OperandTypes::LONG)
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
            if (meta->isAllOperandType(kind) || (kind == InstMetaData::OperandTypes::LONG)
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
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RD1,
                                                 fixed_field_mask_, Form_CA::idType::RS1, false);
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RD2,
                                                 fixed_field_mask_, Form_CA::idType::RS2, false);
            return olist;
        }

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
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
                      {{Form_CA::idType::RS1, InstMetaData::OperandFieldID::RS1},
                       {Form_CA::idType::RS2, InstMetaData::OperandFieldID::RS2}});
            return ss.str();
        }

        // clang-format on
    };

    /**
     * Derivative of CMMV_mva01s extractor for cm.mvsa01 instructions
     * The instructions are identical, except the sources and dests are swapped
     */
    template <> class Extractor<Form_CMMV_mvsa01> : public Extractor<Form_CMMV_mva01s>
    {
      public:
        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return Extractor<Form_CMMV_mva01s>::getDestRegs(icode);
        }

        uint64_t getDestRegs(const Opcode icode) const override
        {
            return Extractor<Form_CMMV_mva01s>::getSourceRegs(icode);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            return Extractor<Form_CMMV_mva01s>::getDestOperTypeRegs(icode, meta, kind);
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            return Extractor<Form_CMMV_mva01s>::getSourceOperTypeRegs(icode, meta, kind);
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            const auto op_type = meta->getOperandType(InstMetaData::OperandFieldID::RS1);

            OperandInfo olist;
            olist.addElement(InstMetaData::OperandFieldID::RS1, op_type, 10, false);
            olist.addElement(InstMetaData::OperandFieldID::RS2, op_type, 11, false);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RD1,
                                                 fixed_field_mask_, Form_CA::idType::RS1, false);
            appendUnmaskedCompressedOperandInfo_(olist, icode, meta,
                                                 InstMetaData::OperandFieldID::RD2,
                                                 fixed_field_mask_, Form_CA::idType::RS2, false);
            return olist;
        }
    };

    /**
     * Derivative of Form_VF_mem extractor for vector loads
     */
    template <> class Extractor<Form_V_load> : public Extractor<Form_VF_mem>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V_load>(ffmask, fset));
        }

        std::string getName() const override { return Form_V_load::name; }

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_VF_mem>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_VF_mem extractor for vector stores
     */
    template <> class Extractor<Form_V_store> : public Extractor<Form_VF_mem>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V_store>(ffmask, fset));
        }

        std::string getName() const override { return Form_V_store::name; }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_VF_mem::idType::RS1, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_VF_mem::idType::RS2, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_VF_mem::idType::RS3, icode, fixed_field_mask_);
        }

        uint64_t getSourceAddressRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_VF_mem::idType::RS1, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_VF_mem::idType::RS2, icode, fixed_field_mask_);
        }

        uint64_t getSourceDataRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_VF_mem::idType::RS3, icode, fixed_field_mask_);
        }

        uint64_t getDestRegs(const Opcode icode) const override { return 0; }

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
                if (meta->isOperandType(InstMetaData::OperandFieldID::RS3, kind))
                {
                    result |= extractUnmaskedIndexBit_(Form_VF_mem::idType::RS3, icode,
                                                       fixed_field_mask_);
                }
                return result;
            }
        }

        uint64_t getDestOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            return 0;
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
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS3,
                                       fixed_field_mask_, Form_VF_mem::idType::RS3, true,
                                       suppress_x0);
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            return {};
        }

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << getVectorMemoryMnemonic(mnemonic, icode) << "\t"
               << "v" << extract_(Form_VF_mem::idType::RS3, icode & ~fixed_field_mask_) << ",x"
               << extract_(Form_VF_mem::idType::RS1, icode & ~fixed_field_mask_);
            if (!isMaskedField_(Form_VF_mem::idType::RS2, fixed_field_mask_))
            {
                ss << ",v" << extract_(Form_VF_mem::idType::RS2, icode);
            }
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
                      {{Form_VF_mem::idType::RS3, InstMetaData::OperandFieldID::RS3},
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

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_VF_mem>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_V extractor for vector-immediate (unsigned) instructions
     */
    template <> class Extractor<Form_V_uimm> : public Extractor<Form_V>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V_uimm>(ffmask, fset));
        }

        std::string getName() const override { return Form_V_uimm::name; }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_V::idType::RS2, icode, fixed_field_mask_);
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
                        extractUnmaskedIndexBit_(Form_V::idType::RS2, icode, fixed_field_mask_);
                }
                return result;
            }
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_V::idType::RS2, false, suppress_x0);
            return olist;
        }

        // TODO: add VM special fields
        uint64_t getSpecialField(SpecialField sfid, Opcode icode,
                                 const InstMetaData::PtrType & meta) const override
        {
            if (SpecialField::VM == sfid)
            {
                if (isMaskedField_(Form_V::idType::VM, fixed_field_mask_))
                {
                    throw UnsupportedExtractorSpecialFieldID("VM", icode);
                }
                else
                {
                    return extract_(Form_V::idType::VM, icode);
                }
            }
            return ExtractorBase::getSpecialField(sfid, icode, meta);
        }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return extract_(Form_V::idType::SIMM5, icode & ~fixed_field_mask_);
        }

        ImmediateType getImmediateType() const override { return ImmediateType::UNSIGNED; }

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\tv" << extract_(Form_V::idType::RD, icode & ~fixed_field_mask_)
               << ",v" << extract_(Form_V::idType::RS2, icode & ~fixed_field_mask_)
               << ", IMM=" << std::dec << getImmediate(icode);
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
                                      {Form_V::idType::RS2, InstMetaData::OperandFieldID::RS2}})
               << ", IMM=" << std::dec << getImmediate(icode);
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
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_V>(ffmask, fset) {}
    };

    /**
     * Derivative of Form_V_uimm> extractor for vector-immediate (signed) instructions
     */
    template <> class Extractor<Form_V_simm> : public Extractor<Form_V_uimm>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V_simm>(ffmask, fset));
        }

        std::string getName() const override { return Form_V_simm::name; }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_V::idType::RS2, icode, fixed_field_mask_);
        }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 4);
        }

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\tv" << extract_(Form_V::idType::RD, icode & ~fixed_field_mask_)
               << ",v" << extract_(Form_V::idType::RS2, icode & ~fixed_field_mask_) << ",0x"
               << std::hex << getSignedOffset(icode);
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
                                      {Form_V::idType::RS2, InstMetaData::OperandFieldID::RS2}})
               << "," << std::dec << getSignedOffset(icode);
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
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_V_uimm>(ffmask, fset)
        {
        }
    };

    /**
     * Derivative of Form_V extractor for vector-immediate (signed) instructions
     */
    template <> class Extractor<Form_V_op> : public Extractor<Form_V>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V_op>(ffmask, fset));
        }

        std::string getName() const override { return Form_V_op::name; }

        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\tv" << extract_(Form_V::idType::RD, icode & ~fixed_field_mask_)
               << ",v" << extract_(Form_V::idType::RS2, icode & ~fixed_field_mask_) << ",v"
               << extract_(Form_V::idType::RS1, icode & ~fixed_field_mask_);
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
                                      {Form_V::idType::RS2, InstMetaData::OperandFieldID::RS2},
                                      {Form_V::idType::RS1, InstMetaData::OperandFieldID::RS1}});
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
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_V>(ffmask, fset) {}
    };

    /**
     * HV Load-Form Extractor
     */
    template <> class Extractor<Form_HV_load> : public Extractor<Form_R>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_HV_load>(ffmask, fset));
        }

        uint64_t getSourceAddressRegs(const Opcode icode) const override
        {
            return getSourceRegs(icode);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t" << extract_(Form_R::idType::RD, icode & ~fixed_field_mask_)
               << "," << extract_(Form_R::idType::RS1, icode & ~fixed_field_mask_);
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
                                      {Form_R::idType::RS1, InstMetaData::OperandFieldID::RS1}});
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_R>(ffmask, fset) {}
    };

    /**
     * HV Store-Form Extractor
     */
    template <> class Extractor<Form_HV_store> : public ExtractorBase<Form_R>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_HV_store>(ffmask, fset));
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_R::idType::RS1, icode, fixed_field_mask_)
                   | extractUnmaskedIndexBit_(Form_R::idType::RS2, icode, fixed_field_mask_);
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

        uint64_t getSourceAddressRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_R::idType::RS1, icode, fixed_field_mask_);
        }

        uint64_t getSourceDataRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_R::idType::RS2, icode, fixed_field_mask_);
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_R::idType::RS1, false, suppress_x0);
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                       fixed_field_mask_, Form_R::idType::RS2, true, suppress_x0);
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extract_(Form_R::idType::RS2, icode & ~fixed_field_mask_) // source data
               << ","
               << extract_(Form_R::idType::RS1, icode & ~fixed_field_mask_); // source address
            return ss.str();
        }

        // clang-format off
        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << dasmFormatRegList_(meta, icode, fixed_field_mask_,
                                     {{Form_R::idType::RS2, InstMetaData::OperandFieldID::RS2},
                                      {Form_R::idType::RS1, InstMetaData::OperandFieldID::RS1}});
            return ss.str();
        }

        // clang-format on

      private:
        Extractor(const uint64_t ffmask, const uint64_t fset) : fixed_field_mask_(ffmask) {}

        uint64_t fixed_field_mask_ = 0;
    };

    /**
     * V_implied-Form Extractor
     * This is an extraction-only form (xform) which adds the destination
     * as an implied source
     */
    template <> class Extractor<Form_V_implied> : public Extractor<Form_V>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V_implied>(ffmask, fset));
        }

        std::string getName() const override { return Form_V_implied::name; }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return Extractor<Form_V>::getSourceRegs(icode)
                   | extractUnmaskedIndexBit_(Form_V::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            uint64_t result = Extractor<Form_V>::getSourceOperTypeRegs(icode, meta, kind);
            if (!(meta->isNoneOperandType(kind) || meta->isAllOperandType(kind)))
            {
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_V::idType::RD, icode, fixed_field_mask_);
                }
            }
            return result;
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist = Extractor<Form_V>::getSourceOperandInfo(icode, meta, suppress_x0);
            appendUnmaskedImpliedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                              fixed_field_mask_, Form_V::idType::RD, false,
                                              suppress_x0);
            return olist;
        }

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_V>(ffmask, fset) {}
    };

    /**
     * V_op_implied-Form Extractor
     * This is an extraction-only form (xform) which adds the destination
     * as an implied source
     */
    template <> class Extractor<Form_V_op_implied> : public Extractor<Form_V_op>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V_op_implied>(ffmask, fset));
        }

        std::string getName() const override { return Form_V_op_implied::name; }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return Extractor<Form_V_op>::getSourceRegs(icode)
                   | extractUnmaskedIndexBit_(Form_V::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            uint64_t result = Extractor<Form_V_op>::getSourceOperTypeRegs(icode, meta, kind);
            if (!(meta->isNoneOperandType(kind) || meta->isAllOperandType(kind)))
            {
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_V::idType::RD, icode, fixed_field_mask_);
                }
            }
            return result;
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist =
                Extractor<Form_V_op>::getSourceOperandInfo(icode, meta, suppress_x0);
            appendUnmaskedImpliedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                              fixed_field_mask_, Form_V::idType::RD, false,
                                              suppress_x0);
            return olist;
        }

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_V_op>(ffmask, fset)
        {
        }
    };

    /**
     * V_uimm_implied-Form Extractor
     * This is an extraction-only form (xform) which adds the destination
     * as an implied source
     */
    template <> class Extractor<Form_V_uimm_implied> : public Extractor<Form_V_uimm>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_V_uimm_implied>(ffmask, fset));
        }

        std::string getName() const override { return Form_V_uimm_implied::name; }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return Extractor<Form_V_uimm>::getSourceRegs(icode)
                   | extractUnmaskedIndexBit_(Form_V::idType::RD, icode, fixed_field_mask_);
        }

        uint64_t getSourceOperTypeRegs(const Opcode icode, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            uint64_t result = Extractor<Form_V_uimm>::getSourceOperTypeRegs(icode, meta, kind);
            if (!(meta->isNoneOperandType(kind) || meta->isAllOperandType(kind)))
            {
                if (meta->isOperandType(InstMetaData::OperandFieldID::RD, kind))
                {
                    result |=
                        extractUnmaskedIndexBit_(Form_V::idType::RD, icode, fixed_field_mask_);
                }
            }
            return result;
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist =
                Extractor<Form_V_uimm>::getSourceOperandInfo(icode, meta, suppress_x0);
            appendUnmaskedImpliedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RD,
                                              fixed_field_mask_, Form_V::idType::RD, false,
                                              suppress_x0);
            return olist;
        }

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) : Extractor<Form_V_uimm>(ffmask, fset)
        {
        }
    };

    /**
     * NTL_hint-Form Extractor
     */
    template <> class Extractor<Form_NTL_hint> : public ExtractorBase<Form_R>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_NTL_hint>(ffmask, fset));
        }

        std::string getName() const override { return Form_NTL_hint::name; }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            return mnemonic;
        }

        std::string dasmString(const std::string & mnemonic, const Opcode icode,
                               const InstMetaData::PtrType & meta) const override
        {
            return mnemonic;
        }

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) : fixed_field_mask_(ffmask) {}

        uint64_t fixed_field_mask_ = 0;
    };

    /**
     * PF_hint-Form Extractor
     */
    template <> class Extractor<Form_PF_hint> : public ExtractorBase<Form_I>
    {
      public:
        Extractor() = default;

        ExtractorIF::PtrType specialCaseClone(const uint64_t ffmask,
                                              const uint64_t fset) const override
        {
            return ExtractorIF::PtrType(new Extractor<Form_PF_hint>(ffmask, fset));
        }

        std::string getName() const override { return Form_PF_hint::name; }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            return extractUnmaskedIndexBit_(Form_I::idType::RS1, icode, fixed_field_mask_);
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
            return 0;
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
            return {};
        }

        ImmediateType getImmediateType() const override { return ImmediateType::SIGNED; }

        uint64_t getImmediate(const Opcode icode) const override
        {
            return extract_(Form_I::idType::IMM, icode) >> 5;
        }

        int64_t getSignedOffset(const Opcode icode) const override
        {
            return signExtend_(getImmediate(icode), 7);
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const Opcode icode) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t"
               << extract_(Form_I::idType::RS1, icode & ~fixed_field_mask_) // base address
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
                                     {{Form_I::idType::RS1, InstMetaData::OperandFieldID::RS1}})
               << ", +0x" << std::hex << getSignedOffset(icode);
            return ss.str();
        }

        // clang-format on

      protected:
        Extractor(const uint64_t ffmask, const uint64_t fset) : fixed_field_mask_(ffmask) {}

        uint64_t fixed_field_mask_ = 0;
    };

    /**
     * Atomic Double which has reads a pair of memory locations and can
     * write a pair of memory locations
     * RV32 NOTE: rs1 is the address base, rd is the dest data starting register.
     * The second register added will be rd+1 and rs2+1
     * Also, checks for alignment of those registers.
     */
    template <> class Extractor<Form_AMO_pair> : public Extractor<Form_AMO>
    {
      public:
        Extractor() = default;

        bool isIllop(Opcode icode) const override
        {
            // illegal if the destination reg is odd
            const uint32_t dest_reg = extract_(Form_AMO::idType::RD, icode);
            // illegal if the source reg is odd
            const uint32_t src_reg = extract_(Form_AMO::idType::RS2, icode);
            return (dest_reg & 0b1) != 0 || (src_reg & 0b1) != 0;
        }

        // Take the standard load and append a second destination to it.
        uint64_t getDestRegs(const Opcode icode) const override
        {
            uint64_t dest_mask = 0;
            if (const uint32_t reg = extract_(Form_AMO::idType::RD, icode); reg != REGISTER_X0)
            {
                dest_mask =
                    extractUnmaskedIndexBit_(Form_AMO::idType::RD, icode, fixed_field_mask_);

                const uint32_t rd_val_pos = 64 - __builtin_clzll(dest_mask);
                dest_mask |= (0x1ull << rd_val_pos);
            }
            return dest_mask;
        }

        OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            if (const uint32_t reg = extract_(Form_AMO::idType::RD, icode); reg != REGISTER_X0)
            {
                olist = Extractor<Form_AMO>::getDestOperandInfo(icode, meta, suppress_x0);
                auto rd2_elem = olist.getElements().at(0);
                rd2_elem.field_id = InstMetaData::OperandFieldID::RD2;
                ++rd2_elem.field_value;

                // Add the second RD
                olist.addElement(rd2_elem);
            }
            return olist;
        }

        uint64_t getSourceRegs(const Opcode icode) const override
        {
            // The mask for all of the source regs (addr + data) is
            // actually the original RS1/RS2 | RS3, but RS3 is not part of
            // the icode
            return Extractor<Form_AMO>::getSourceRegs(icode) | getSourceDataRegs(icode);
        }

        uint64_t getSourceDataRegs(const Opcode icode) const override
        {
            // Add a second source to the pair
            uint64_t src_mask = 0;
            if (const uint32_t reg = extract_(Form_AMO::idType::RS2, icode); reg != REGISTER_X0)
            {
                src_mask =
                    extractUnmaskedIndexBit_(Form_AMO::idType::RS2, icode, fixed_field_mask_);
                const uint32_t rs2_val_pos = 64 - __builtin_clzll(src_mask);
                src_mask |= (0x1ull << rs2_val_pos);
            }
            return src_mask;
        }

        OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS1,
                                       fixed_field_mask_, Form_AMO::idType::RS1, false,
                                       suppress_x0);
            if (const uint32_t reg = extract_(Form_AMO::idType::RS2, icode); reg != REGISTER_X0)
            {
                appendUnmaskedOperandInfo_(olist, icode, meta, InstMetaData::OperandFieldID::RS2,
                                           fixed_field_mask_, Form_AMO::idType::RS2, false,
                                           suppress_x0);
                auto rs3_elem = olist.getElements().at(1);
                assert(rs3_elem.field_id == InstMetaData::OperandFieldID::RS2);

                rs3_elem.field_id = InstMetaData::OperandFieldID::RS3;
                ++rs3_elem.field_value;

                // Add the second RS
                olist.addElement(rs3_elem);
            }
            return olist;
        }
    };

} // namespace mavis
