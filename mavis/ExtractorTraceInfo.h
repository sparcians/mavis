#pragma once

#include "Extractor.h"

namespace mavis
{

    /**
     * TraceInfoType Extractor
     */
    template <class TraceInfoType> class ExtractorTraceInfo : public ExtractorIF
    {
      private:
        // TODO: Move this to a central place (shared by DecodedInstInfo, ExtractorDirectInfo, and
        // ExtractorTraceInfo)
        constexpr uint32_t count1Bits_(const uint64_t n) const
        {
            uint64_t x = n - ((n >> 1u) & 0x5555555555555555ull);
            x = (x & 0x3333333333333333ull) + ((x >> 2u) & 0x3333333333333333ull);
            x = (x + (x >> 4u)) & 0x0F0F0F0F0F0F0F0Full;
            x = x + (x >> 8u);
            x = x + (x >> 16u);
            x = x + (x >> 32u);
            return x & 0x7Full;
        }

        // TODO: Move this to a central place (shared by DecodedInstInfo, ExtractorDirectInfo, and
        // ExtractorTraceInfo)
        RegListType bitmaskToRegList_(uint64_t bits) const
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

      public:
        explicit ExtractorTraceInfo(const TraceInfoType & tinfo) : tinfo_(tinfo) {}

        std::string getName() const override { return name_; }

        bool isIllop(Opcode) const override { return false; }

        bool isHint(Opcode) const override { return false; }

        // uint64_t getFunction(const Opcode) const override
        //{
        //     return tinfo_.getFunction();
        // }

        uint64_t getSourceRegs(const Opcode) const override { return tinfo_.getSourceRegs(); }

        // RegListType getSourceList(const Opcode, bool suppress_x0 = false) const override
        //{
        //     uint64_t sources = getSourceRegs(0) & ~(suppress_x0 ? 0x1ull : 0x0ull);
        //     return bitmaskToRegList_(sources);
        // }

        uint64_t getDestRegs(const Opcode) const override { return tinfo_.getDestRegs(); }

        // RegListType getDestList(const Opcode, bool suppress_x0 = false) const override
        //{
        //     uint64_t dests = getDestRegs(0) & ~(suppress_x0 ? 0x1ull : 0x0ull);
        //     return bitmaskToRegList_(dests);
        // }

        // TODO: Finish this
        uint64_t getSourceOperTypeRegs(const Opcode, const InstMetaData::PtrType &,
                                       InstMetaData::OperandTypes) const override
        {
            return 0;
        }

        // Default implementation returns empty list
        // RegListType getSourceOperTypeList(const Opcode, const InstMetaData::PtrType &,
        // InstMetaData::OperandTypes,
        //                                  bool suppress_x0 = false) const override
        //{
        //    return {};
        //}

        // TODO: Finish this
        uint64_t getDestOperTypeRegs(const Opcode, const InstMetaData::PtrType &,
                                     InstMetaData::OperandTypes) const override
        {
            return 0;
        }

        // Default implementation returns empty list
        // RegListType getDestOperTypeList(const Opcode, const InstMetaData::PtrType &,
        // InstMetaData::OperandTypes,
        //                                bool suppress_x0 = false) const override
        //{
        //    return {};
        //}

        // TODO: Finish this
        uint64_t getSourceAddressRegs(const Opcode) const override { return 0; }

        // Default implementation returns empty list
        // RegListType getSourceAddressList(const Opcode, bool suppress_x0 = false) const override
        //{
        //    return {};
        //}

        uint64_t getSourceDataRegs(const Opcode) const override { return 0; }

        // Default implementation returns empty list
        // RegListType getSourceDataList(const Opcode, bool suppress_x0 = false) const override
        //{
        //    return {};
        //}

        OperandInfo getSourceOperandInfo(Opcode, const InstMetaData::PtrType &,
                                         bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            uint64_t sources = getSourceRegs(0) & ~(suppress_x0 ? 0x1ull : 0x0ull);
            RegListType source_list = bitmaskToRegList_(sources);
            for (const auto & reg : source_list)
            {
                olist.addElement(InstMetaData::OperandFieldID::NONE,
                                 InstMetaData::OperandTypes::NONE, reg, false);
            }
            return olist;
        }

        OperandInfo getDestOperandInfo(Opcode, const InstMetaData::PtrType &,
                                       bool suppress_x0 = false) const override
        {
            OperandInfo olist;
            uint64_t dests = getDestRegs(0) & ~(suppress_x0 ? 0x1ull : 0x0ull);
            RegListType dest_list = bitmaskToRegList_(dests);
            for (const auto & reg : dest_list)
            {
                olist.addElement(InstMetaData::OperandFieldID::NONE,
                                 InstMetaData::OperandTypes::NONE, reg, false);
            }
            return olist;
        }

        ImmediateType getImmediateType() const override { return ImmediateType::NONE; }

        uint64_t getImmediate(const Opcode) const override { return tinfo_.getImmediate(); }

        int64_t getSignedOffset(const Opcode icode) const override { return getImmediate(icode); }

        // For hashing...
        // uint32_t getIFCIndex(const Opcode icode) const override
        //{
        // Just generate a random number?
        // return icode;
        //}

        // For matching...
        // uint32_t getIFCTag(const Opcode icode) const override
        //{
        // For now...
        // return icode;
        //}

        InstMetaData::SpecialFieldsMap getSpecialFields(Opcode, const InstMetaData::PtrType &) const override
        {
            return InstMetaData::SpecialFieldsMap();
        }

        using ExtractorIF::dasmString; // tell the compiler all dasmString
                                       // overloads are considered

        std::string dasmString(const std::string & mnemonic, Opcode) const override
        {
            return mnemonic + " (from " + getName() + ")";
        }

        // TODO: If we need annotations for disassembly for trace extractors, we
        // can add it here. See the implementation in ExtractorDirectInfoBase as a
        // reference
        void dasmAnnotate(const std::string &) override { assert(false); }

        const std::string & getDasmAnnotation() const override
        {
            static const std::string empty;
            assert(false);
            return empty;
        }

        void print(std::ostream &) const override
        {
            // TODO: Need a print function
        }

      private:
        ExtractorIF::PtrType specialCaseClone(const uint64_t, const uint64_t) const override
        {
            assert(false);
            return nullptr;
        }

        const TraceInfoType tinfo_;
        const std::string name_ = "ExtractorTraceInfo";
    };

} // namespace mavis
