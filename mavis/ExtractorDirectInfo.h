#pragma once

#include "DecoderConsts.h"
#include "DecoderExceptions.h"
#include "Extractor.h"
#include <string>

namespace mavis
{

    /**
     * \brief Interface for direct extractors
     */
    class ExtractorDirectInfoIF : public ExtractorIF
    {
      public:
        virtual ExtractorIF::PtrType clone() const = 0;

        virtual std::string getMnemonic() const = 0;

        virtual InstructionUniqueID getUID() const = 0;

        bool isIllop(Opcode) const override { return false; }

        bool isHint(Opcode) const override { return false; }
    };

    /**
     * \brief Base class of common and convenience methods
     */
    class ExtractorDirectBase : public ExtractorDirectInfoIF
    {
      public:
        explicit ExtractorDirectBase(const std::string & mnemonic) :
            mnemonic_(mnemonic),
            uid_(INVALID_UID),
            immediate_(0),
            immediate_type_(ImmediateType::NONE)
        {
        }

        explicit ExtractorDirectBase(const std::string & mnemonic, uint64_t imm,
                                     ImmediateType itype = ImmediateType::UNSIGNED) :
            mnemonic_(mnemonic),
            uid_(INVALID_UID),
            immediate_(imm),
            immediate_type_(itype)
        {
        }

        explicit ExtractorDirectBase(const std::string & mnemonic, const InstMetaData::SpecialFieldsMap & specials) :
            mnemonic_(mnemonic),
            uid_(INVALID_UID),
            specials_(specials),
            immediate_(0),
            immediate_type_(ImmediateType::NONE)
        {
        }

        explicit ExtractorDirectBase(const std::string & mnemonic, const InstMetaData::SpecialFieldsMap & specials, uint64_t imm,
                                     ImmediateType itype = ImmediateType::UNSIGNED) :
            mnemonic_(mnemonic),
            uid_(INVALID_UID),
            specials_(specials),
            immediate_(imm),
            immediate_type_(itype)
        {
        }

        explicit ExtractorDirectBase(const InstructionUniqueID uid) :
            mnemonic_("UNSET-in-ExtractorDirectBase"),
            uid_(uid),
            immediate_(0),
            immediate_type_(ImmediateType::NONE)
        {
        }

        explicit ExtractorDirectBase(const InstructionUniqueID uid, const InstMetaData::SpecialFieldsMap & specials) :
            mnemonic_("UNSET-in-ExtractorDirectBase"),
            uid_(uid),
            specials_(specials),
            immediate_(0),
            immediate_type_(ImmediateType::NONE)
        {
        }

        explicit ExtractorDirectBase(const InstructionUniqueID uid, uint64_t imm,
                                     ImmediateType itype = ImmediateType::UNSIGNED) :
            mnemonic_("UNSET-in-ExtractorDirectBase"),
            uid_(uid),
            immediate_(imm),
            immediate_type_(itype)
        {
        }

        explicit ExtractorDirectBase(const InstructionUniqueID uid, const InstMetaData::SpecialFieldsMap & specials, uint64_t imm,
                                     ImmediateType itype = ImmediateType::UNSIGNED) :
            mnemonic_("UNSET-in-ExtractorDirectBase"),
            uid_(uid),
            specials_(specials),
            immediate_(imm),
            immediate_type_(itype)
        {
        }

        ExtractorDirectBase(const ExtractorDirectBase & other) = default;

        std::string getMnemonic() const override { return mnemonic_; }

        InstructionUniqueID getUID() const override { return uid_; }

        // Default implementation returns 0 bitset
        uint64_t getSourceRegs(const Opcode) const override { return 0; }

        // Default implementation returns 0 bitset
        uint64_t getSourceAddressRegs(const Opcode) const override { return 0; }

        // Default implementation returns 0 bitset
        uint64_t getSourceDataRegs(const Opcode) const override { return 0; }

        // Default implementation returns 0 bitset
        uint64_t getDestRegs(const Opcode) const override { return 0; }

        // Default implementation returns empty list
        OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType &,
                                         bool suppress_x0 = false) const override
        {
            (void)suppress_x0;
            return {};
        }

        // Default implementation returns empty list
        OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType &,
                                       bool suppress_x0 = false) const override
        {
            (void)suppress_x0;
            return {};
        }

        ImmediateType getImmediateType() const override { return immediate_type_; }

        uint64_t getImmediate(const uint64_t) const override { return immediate_; }

        int64_t getSignedOffset(const uint64_t icode) const override { return getImmediate(icode); }

        InstMetaData::SpecialFieldsMap getSpecialFields(Opcode, const InstMetaData::PtrType &) const override
        {
            return specials_;
        }

        uint64_t getSourceOperTypeRegs(const uint64_t, const InstMetaData::PtrType & meta,
                                       InstMetaData::OperandTypes kind) const override
        {
            if (meta->isAllOperandType(kind) || meta->isAnySourceOperandType(kind))
            {
                return getSourceRegs(0);
            }
            else
            {
                return 0;
            }
        }

        uint64_t getDestOperTypeRegs(const uint64_t, const InstMetaData::PtrType & meta,
                                     InstMetaData::OperandTypes kind) const override
        {
            if (meta->isAllOperandType(kind) || meta->isAnyDestOperandType(kind))
            {
                return getDestRegs(0);
            }
            else
            {
                return 0;
            }
        }

        void dasmAnnotate(const std::string & txt) override { annotation_ = txt; }

        const std::string & getDasmAnnotation() const override { return annotation_; }

        void print(std::ostream &) const override
        {
            // TODO: Need a print function
        }

      private:
        ExtractorIF::PtrType specialCaseClone(const uint64_t, const uint64_t) const override
        {
            throw std::runtime_error("Unimplemented");
            return nullptr;
        }

      protected:
        const std::string mnemonic_;
        const InstructionUniqueID uid_ = INVALID_UID;
        const InstMetaData::SpecialFieldsMap specials_ = {};
        const uint64_t immediate_;
        const ImmediateType immediate_type_;

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
            if (bitcnt > 0)
            {
                uint32_t found_bits = 0;
                for (uint32_t i = 0; (found_bits != bitcnt); ++i)
                {
                    if (bits & (0x1ull << i))
                    {
                        if (found_bits != 0)
                        {
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
            if (bitcnt > 0)
            {
                uint32_t found_bits = 0;
                for (uint32_t i = 0; (found_bits != bitcnt); ++i)
                {
                    if (bits & (0x1ull << i))
                    {
                        rlist.push_back(i);
                        ++found_bits;
                    }
                }
            }
            return rlist;
        }

        const std::string & dasmGetAnnotation_() const { return annotation_; }

      private:
        std::string annotation_;
    };

    class ExtractorDirectInfo : public ExtractorDirectBase
    {
      private:
        static inline const std::string name_{"ExtractorDirectInfo"};

      public:
        ExtractorDirectInfo(const std::string & mnemonic, const RegListType & sources,
                            const RegListType & dests) :
            ExtractorDirectBase(mnemonic),
            sources_(sources),
            dests_(dests)
        {
        }

        ExtractorDirectInfo(const std::string & mnemonic, const RegListType & sources,
                            const RegListType & dests, uint64_t imm) :
            ExtractorDirectBase(mnemonic, imm),
            sources_(sources),
            dests_(dests)
        {
        }

        ExtractorDirectInfo(const std::string & mnemonic, const RegListType & sources,
                            const RegListType & dests, const InstMetaData::SpecialFieldsMap & specials) :
            ExtractorDirectBase(mnemonic),
            sources_(sources),
            dests_(dests),
            specials_(specials)
        {
        }

        ExtractorDirectInfo(const std::string & mnemonic, const RegListType & sources,
                            const RegListType & dests, const InstMetaData::SpecialFieldsMap & specials,
                            uint64_t imm) :
            ExtractorDirectBase(mnemonic, imm),
            sources_(sources),
            dests_(dests),
            specials_(specials)
        {
        }

        ExtractorDirectInfo(const InstructionUniqueID uid, const RegListType & sources,
                            const RegListType & dests) :
            ExtractorDirectBase(uid),
            sources_(sources),
            dests_(dests)
        {
        }

        ExtractorDirectInfo(const InstructionUniqueID uid, const RegListType & sources,
                            const RegListType & dests, uint64_t imm) :
            ExtractorDirectBase(uid, imm),
            sources_(sources),
            dests_(dests)
        {
        }

        ExtractorDirectInfo(const InstructionUniqueID uid, const RegListType & sources,
                            const RegListType & dests, const InstMetaData::SpecialFieldsMap & specials) :
            ExtractorDirectBase(uid),
            sources_(sources),
            dests_(dests),
            specials_(specials)
        {
        }

        ExtractorDirectInfo(const InstructionUniqueID uid, const RegListType & sources,
                            const RegListType & dests, const InstMetaData::SpecialFieldsMap & specials,
                            uint64_t imm) :
            ExtractorDirectBase(uid, imm),
            sources_(sources),
            dests_(dests),
            specials_(specials)
        {
        }

        ExtractorDirectInfo(const ExtractorDirectInfo & other) = default;

        ExtractorIF::PtrType clone() const override
        {
            return std::make_shared<ExtractorDirectInfo>(*this);
        }

        std::string getName() const override { return name_; }

        uint64_t getSourceRegs(const uint64_t) const override
        {
            uint64_t src_bits = 0;
            for (const auto reg : sources_)
            {
                if (reg > MAX_REG_NUM)
                {
                    throw InvalidRegisterNumber(mnemonic_, reg);
                }
                else
                {
                    src_bits |= (1ull << reg);
                }
            }
            return src_bits;
        }

        uint64_t getDestRegs(const uint64_t) const override
        {
            uint64_t dst_bits = 0;
            for (const auto reg : dests_)
            {
                if (reg > MAX_REG_NUM)
                {
                    throw InvalidRegisterNumber(mnemonic_, reg);
                }
                else
                {
                    dst_bits |= (1ull << reg);
                }
            }
            return dst_bits;
        }

        OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType & meta,
                                         bool suppress_x0 = false) const override
        {
            (void)suppress_x0;
            OperandInfo olist;
            std::underlying_type_t<InstMetaData::OperandFieldID> field_id =
                static_cast<std::underlying_type_t<InstMetaData::OperandFieldID>>(
                    InstMetaData::OperandFieldID::RS1);
            constexpr std::underlying_type_t<InstMetaData::OperandFieldID> max_sources =
                static_cast<std::underlying_type_t<InstMetaData::OperandFieldID>>(
                    InstMetaData::OperandFieldID::RS_MAX)
                - static_cast<std::underlying_type_t<InstMetaData::OperandFieldID>>(
                    InstMetaData::OperandFieldID::RS1);
            if (sources_.size() > max_sources) [[unlikely]]
            {
                std::ostringstream ss;
                ss << "There are " << sources_.size() << " source operands, but the maximum is "
                   << max_sources;
                throw std::length_error(ss.str());
            }

            for (const auto & reg : sources_)
            {
                while ((field_id < max_sources)
                       && meta->isOperandFixed(static_cast<InstMetaData::OperandFieldID>(field_id)))
                {
                    ++field_id;
                }
                if (field_id >= static_cast<std::underlying_type_t<InstMetaData::OperandFieldID>>(
                        InstMetaData::OperandFieldID::RS_MAX)) [[unlikely]]
                {
                    throw std::out_of_range("Exceeded maximum number of source operands");
                }

                const InstMetaData::OperandFieldID op_field_id =
                    static_cast<InstMetaData::OperandFieldID>(field_id);
                // FIXME: workaround for RS3 in vector unit-stride store
                const bool is_rs3 = op_field_id == InstMetaData::OperandFieldID::RS3;
                olist.addElement(op_field_id, meta->getOperandType(op_field_id), reg, is_rs3);
                ++field_id;
            }
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType & meta,
                                       bool suppress_x0 = false) const override
        {
            (void)suppress_x0;
            OperandInfo olist;
            std::underlying_type_t<InstMetaData::OperandFieldID> field_id =
                static_cast<std::underlying_type_t<InstMetaData::OperandFieldID>>(
                    InstMetaData::OperandFieldID::RD1);
            constexpr std::underlying_type_t<InstMetaData::OperandFieldID> max_dests =
                static_cast<std::underlying_type_t<InstMetaData::OperandFieldID>>(
                    InstMetaData::OperandFieldID::RD_MAX)
                - static_cast<std::underlying_type_t<InstMetaData::OperandFieldID>>(
                    InstMetaData::OperandFieldID::RD1);
            if (dests_.size() > max_dests) [[unlikely]]
            {
                std::ostringstream ss;
                ss << "There are " << dests_.size() << " dest operands, but the maximum is "
                   << max_dests;
                throw std::length_error(ss.str());
            }

            for (const auto & reg : dests_)
            {
                while ((field_id < max_dests)
                       && meta->isOperandFixed(static_cast<InstMetaData::OperandFieldID>(field_id)))
                {
                    ++field_id;
                }
                if (field_id >= static_cast<std::underlying_type_t<InstMetaData::OperandFieldID>>(
                        InstMetaData::OperandFieldID::RD_MAX)) [[unlikely]]
                {
                    throw std::out_of_range("Exceeded maximum number of dest operands");
                }

                const InstMetaData::OperandFieldID op_field_id =
                    static_cast<InstMetaData::OperandFieldID>(field_id);
                olist.addElement(op_field_id, meta->getDefaultDestType(), reg, false);
                ++field_id;
            }
            return olist;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, const uint64_t) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t";
            bool first = true;
            for (const auto reg : dests_)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    ss << ",";
                }
                ss << static_cast<uint32_t>(reg);
            }
            for (const auto reg : sources_)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    ss << ",";
                }
                ss << static_cast<uint32_t>(reg);
            }
            if (hasImmediate())
            {
                ss << ", 0x" << std::hex << immediate_;
            }
            return ss.str();
        }

      private:
        const RegListType sources_;
        const RegListType dests_;
        const InstMetaData::SpecialFieldsMap specials_;
    };

    class ExtractorDirectOpInfoList : public ExtractorDirectBase
    {
      private:
        static inline const std::string name_{"ExtractorDirectOpInfoList"};

      public:
        ExtractorDirectOpInfoList(const std::string & mnemonic, const OperandInfo & sources,
                                  const OperandInfo & dests) :
            ExtractorDirectBase(mnemonic),
            sources_(sources),
            dests_(dests)
        {
        }

        ExtractorDirectOpInfoList(const std::string & mnemonic, const OperandInfo & sources,
                                  const OperandInfo & dests, uint64_t imm) :
            ExtractorDirectBase(mnemonic, imm),
            sources_(sources),
            dests_(dests)
        {
        }

        ExtractorDirectOpInfoList(const std::string & mnemonic, const OperandInfo & sources,
                                  const OperandInfo & dests, const InstMetaData::SpecialFieldsMap & specials) :
            ExtractorDirectBase(mnemonic, specials),
            sources_(sources),
            dests_(dests)
        {
        }

        ExtractorDirectOpInfoList(const std::string & mnemonic, const OperandInfo & sources,
                                  const OperandInfo & dests, const InstMetaData::SpecialFieldsMap & specials,
                                  uint64_t imm) :
            ExtractorDirectBase(mnemonic, specials, imm),
            sources_(sources),
            dests_(dests)
        {
        }

        ExtractorDirectOpInfoList(const InstructionUniqueID uid, const OperandInfo & sources,
                                  const OperandInfo & dests) :
            ExtractorDirectBase(uid),
            sources_(sources),
            dests_(dests)
        {
        }

        ExtractorDirectOpInfoList(const InstructionUniqueID uid, const OperandInfo & sources,
                                  const OperandInfo & dests, uint64_t imm) :
            ExtractorDirectBase(uid, imm),
            sources_(sources),
            dests_(dests)
        {
        }

        ExtractorDirectOpInfoList(const InstructionUniqueID uid, const OperandInfo & sources,
                                  const OperandInfo & dests, const InstMetaData::SpecialFieldsMap & specials) :
            ExtractorDirectBase(uid, specials),
            sources_(sources),
            dests_(dests)
        {
        }

        ExtractorDirectOpInfoList(const InstructionUniqueID uid, const OperandInfo & sources,
                                  const OperandInfo & dests, const InstMetaData::SpecialFieldsMap & specials,
                                  uint64_t imm) :
            ExtractorDirectBase(uid, specials, imm),
            sources_(sources),
            dests_(dests)
        {
        }

        ExtractorDirectOpInfoList(const ExtractorDirectOpInfoList & other) = default;

        ExtractorIF::PtrType clone() const override
        {
            return std::make_shared<ExtractorDirectOpInfoList>(*this);
        }

        std::string getName() const override { return name_; }

        uint64_t getSourceRegs(const uint64_t) const override
        {
            uint64_t src_bits = 0;
            for (const auto & el : sources_.getElements())
            {
                if (el.field_value > MAX_REG_NUM)
                {
                    throw InvalidRegisterNumber(mnemonic_, el.field_value);
                }
                else
                {
                    src_bits |= (1ull << el.field_value);
                }
            }
            return src_bits;
        }

        uint64_t getDestRegs(const uint64_t) const override
        {
            uint64_t dst_bits = 0;
            for (const auto & el : dests_.getElements())
            {
                if (el.field_value > MAX_REG_NUM)
                {
                    throw InvalidRegisterNumber(mnemonic_, el.field_value);
                }
                else
                {
                    dst_bits |= (1ull << el.field_value);
                }
            }
            return dst_bits;
        }

        OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType &,
                                         bool suppress_x0 = false) const override
        {
            (void)suppress_x0;
            return sources_;
        }

        OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType &,
                                       bool suppress_x0 = false) const override
        {
            (void)suppress_x0;
            return dests_;
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString

        // overloads are considered
        std::string dasmString(const std::string & mnemonic, const uint64_t) const override
        {
            std::stringstream ss;
            ss << mnemonic << "\t";
            bool first = true;
            for (const auto & el : dests_.getElements())
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    ss << ",";
                }
                ss << static_cast<uint32_t>(el.field_value);
            }
            for (const auto & el : sources_.getElements())
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    ss << ",";
                }
                ss << static_cast<uint32_t>(el.field_value);
            }
            if (hasImmediate())
            {
                ss << " 0x" << std::hex << immediate_;
            }
            return ss.str();
        }

      private:
        const OperandInfo sources_;
        const OperandInfo dests_;
    };
} // namespace mavis
