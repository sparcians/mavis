#pragma once

#include <string>
#include <memory>
#include <bitset>
#include <vector>
#include <iostream>

#include "DecoderConsts.h"
#include "Extractor.h"

namespace mavis
{

    // TODO: Flesh out the rest of the operand type bit sets
    struct DecodedInstructionInfo
    {
      public:
        typedef std::shared_ptr<DecodedInstructionInfo> PtrType;
        typedef std::bitset<64> BitMask;
        // typedef std::vector<uint8_t>                    OperandArray;
        using OperandArray = ExtractorIF::RegListType;
        using OpInfoList = OperandInfo::ElementList;

        // Sub-types for the JAL/JALR instructions
        // See RV ISA Spec, section
        enum class ExtractedInstTypes : uint64_t
        {
            NONE = 0,
            CALL = 1ull << 0,
            RETURN = 1ull << 1,
            INDIRECT = 1ull << 2
        };

      private:
        // TODO: Move this to a central place in mavis
        constexpr uint32_t count1Bits_(const uint64_t n) const
        {
            uint64_t x = n - ((n >> 1) & 0x5555555555555555ull);
            x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
            x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0Full;
            x = x + (x >> 8);
            x = x + (x >> 16);
            x = x + (x >> 32);
            return x & 0x7F;
        }

        /**
         * \brief Returns a vector of bit indices from combined (AND-ed) BitMasks
         * \tparam ArgTypes
         * \param args
         * \return
         */
        // TODO: Move this to a central place in mavis
        OperandArray getVectorOfBitIndices_(uint64_t bits) const
        {
            // Iterate through the combined mask, adding 1-bit positions to the
            // returned vector
            OperandArray vals;
            const uint32_t bitcnt = count1Bits_(bits);
            if (bitcnt > 0)
            {
                vals.reserve(bitcnt);
                uint32_t found_bits = 0;
                for (uint32_t i = 0; (found_bits != bitcnt); ++i)
                {
                    if (bits & (0x1ull << i))
                    {
                        ++found_bits;
                        vals.emplace_back(i);
                    }
                }
            }
            return vals;
        }

        constexpr bool hasSourceReg_(uint64_t srcs) const
        {
            return (srcs != 0) && (srcs != (1ull << REGISTER_X0));
        }

        OperandArray combineVals_(const OperandArray & x, const OperandArray & y) const
        {
            OperandArray result;
            std::merge(x.begin(), x.end(), y.begin(), y.end(), std::back_inserter(result));
            return result;
        }

        // TEMPORARY: check reglist against bitset for sanity
        // static inline bool
        // agree_(const OperandArray &arr, uint64_t bits, const std::string &context, const
        // std::string &form_name)
        //{
        //    uint64_t arr_bits = 0;
        //    for (const auto &reg : arr) {
        //        arr_bits |= (0x1ull << reg);
        //    }
        //    if (arr_bits != bits) {
        //        std::cerr << "EXTRACTION MISMATCH: Form '" << form_name << "'"
        //                  << ", context '" << context << "'"
        //                  << ", array [";
        //        for (const auto &reg : arr) {
        //            std::cerr << std::dec << reg << ",";
        //        }
        //        std::cerr << "], (0x" << std::hex << arr_bits << ")"
        //                  << ", bitset=0x" << bits << " [";
        //        OperandArray bits_arr = getVectorOfBitIndices_(bits);
        //        for (const auto &reg : bits_arr) {
        //            std::cerr << std::dec << reg << ",";
        //        }
        //        std::cerr << "]" << std::dec << std::endl;
        //        return false;
        //    }
        //    return true;
        //}

        // TEMPORARY: Check opinfo lists for agreement with reg lists and bit sets
        bool
        agree_opinfo_(const OperandInfo::ElementList & olist, uint64_t bits,
                      const std::string & context, const std::string & form_name,
                      InstMetaData::OperandTypes otype = InstMetaData::OperandTypes::NONE) const
        {
            if (olist.empty() && (bits != 0))
            {
                std::cerr << "EXTRACTION MISMATCH: Form '" << form_name << "'"
                          << ", context '" << context << "'"
                          << ", empty opinfo-list and non-zero bitset=0x" << std::hex << bits
                          << " [";
                OperandArray bits_arr = getVectorOfBitIndices_(bits);
                for (const auto & reg : bits_arr)
                {
                    std::cerr << std::dec << reg << ",";
                }
                std::cerr << "]" << std::dec << std::endl;
                return false;
            }

            for (const auto & opinfo : olist)
            {
                uint64_t oi_bit = (0x1ull << opinfo.field_value);

                if (((otype == InstMetaData::OperandTypes::NONE) || (opinfo.operand_type == otype))
                    && ((oi_bit & bits) == 0))
                {
                    std::cerr << "EXTRACTION MISMATCH: Form '" << form_name << "'"
                              << ", context '" << context << "'"
                              << ", operand value '" << opinfo.field_value << "'"
                              << " not in bitset=0x" << std::hex << bits << " [";
                    OperandArray bits_arr = getVectorOfBitIndices_(bits);
                    for (const auto & reg : bits_arr)
                    {
                        std::cerr << std::dec << reg << ",";
                    }
                    std::cerr << "]" << std::dec << std::endl;
                    return false;
                }
            }
            return true;
        }

      public:
        // Architectural information
        const std::string mnemonic;
        const InstructionUniqueID unique_id;

        const OperandInfo source_opinfo;
        const OperandInfo dest_opinfo;

        const OperandInfo::ElementList source_opinfo_list;
        const OperandInfo::ElementList dest_opinfo_list;

        const uint64_t sources;
        // const OperandArray source_vals;
        const uint32_t n_sources;

        const uint64_t dests;
        // const OperandArray dest_vals;
        const uint32_t n_dests;

        const uint64_t word_sources;
        const uint64_t word_dests;
        const uint64_t word_operands;
        // const OperandArray word_oper_vals;
        // const OperandArray word_source_vals;
        // const OperandArray word_dest_vals;
        const uint32_t word_n_sources;
        const uint32_t word_n_dests;
        const uint32_t word_n_operands;

        const uint64_t long_sources;
        const uint64_t long_dests;
        const uint64_t long_operands;
        // const OperandArray long_oper_vals;
        // const OperandArray long_source_vals;
        // const OperandArray long_dest_vals;
        const uint32_t long_n_sources;
        const uint32_t long_n_dests;
        const uint32_t long_n_operands;

        const uint64_t int_sources;
        const uint64_t int_dests;
        const uint64_t int_operands;
        // const OperandArray int_oper_vals;
        // const OperandArray int_source_vals;
        // const OperandArray int_dest_vals;
        const uint32_t int_n_sources;
        const uint32_t int_n_dests;
        const uint32_t int_n_operands;

        const uint64_t half_sources;
        const uint64_t half_dests;
        const uint64_t half_operands;
        // const OperandArray half_oper_vals;
        // const OperandArray half_source_vals;
        // const OperandArray half_dest_vals;
        const uint32_t half_n_sources;
        const uint32_t half_n_dests;
        const uint32_t half_n_operands;

        const uint64_t single_sources;
        const uint64_t single_dests;
        const uint64_t single_operands;
        // const OperandArray single_oper_vals;
        // const OperandArray single_source_vals;
        // const OperandArray single_dest_vals;
        const uint32_t single_n_sources;
        const uint32_t single_n_dests;
        const uint32_t single_n_operands;

        const uint64_t double_sources;
        const uint64_t double_dests;
        const uint64_t double_operands;
        // const OperandArray double_oper_vals;
        // const OperandArray double_source_vals;
        // const OperandArray double_dest_vals;
        const uint32_t double_n_sources;
        const uint32_t double_n_dests;
        const uint32_t double_n_operands;

        const uint64_t float_sources;
        const uint64_t float_dests;
        const uint64_t float_operands;
        // const OperandArray float_oper_vals;
        // const OperandArray float_source_vals;
        // const OperandArray float_dest_vals;
        const uint32_t float_n_sources;
        const uint32_t float_n_dests;
        const uint32_t float_n_operands;

        const uint64_t quad_sources;
        const uint64_t quad_dests;
        const uint64_t quad_operands;
        // const OperandArray quad_oper_vals;
        // const OperandArray quad_source_vals;
        // const OperandArray quad_dest_vals;
        const uint32_t quad_n_sources;
        const uint32_t quad_n_dests;
        const uint32_t quad_n_operands;

        const uint64_t vector_sources;
        const uint64_t vector_dests;
        const uint64_t vector_operands;
        const uint32_t vector_n_sources;
        const uint32_t vector_n_dests;
        const uint32_t vector_n_operands;

        const uint64_t addr_sources;
        // const OperandArray addr_source_vals;
        // const uint32_t addr_n_sources;

        const uint64_t data_sources;
        // const OperandArray data_source_vals;
        // const uint32_t data_n_sources;

        const bool is_hint;
        const ImmediateType immediate_type;
        const bool has_immediate;
        const uint64_t immediate;
        const int64_t signed_offset;
        const uint32_t data_size;
        // const uint64_t function;
        uint64_t ext_itype =
            static_cast<std::underlying_type_t<ExtractedInstTypes>>(ExtractedInstTypes::NONE);

        // special fields
        ExtractorIF::SpecialFields special_fields;

        DecodedInstructionInfo(const std::string & iname, const InstructionUniqueID uid,
                               const ExtractorIF::PtrType & extractor,
                               const InstMetaData::PtrType & meta, const Opcode icode) :
            mnemonic(iname),
            unique_id(uid),

            source_opinfo(extractor->getSourceOperandInfo(icode, meta)),
            dest_opinfo(extractor->getDestOperandInfo(icode, meta)),

            source_opinfo_list(source_opinfo.getElements()),
            dest_opinfo_list(dest_opinfo.getElements()),

            sources(extractor->getSourceRegs(icode)),
            // source_vals(extractor->getSourceList(icode)),
            n_sources(source_opinfo.getNOpers()),

            dests(extractor->getDestRegs(icode)),
            // dest_vals(extractor->getDestList(icode)),
            n_dests(dest_opinfo.getNOpers()),

            word_sources(
                extractor->getSourceOperTypeRegs(icode, meta, InstMetaData::OperandTypes::WORD)),
            word_dests(
                extractor->getDestOperTypeRegs(icode, meta, InstMetaData::OperandTypes::WORD)),
            word_operands(word_sources | word_dests),
            // word_oper_vals(extractor->getOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::WORD)),
            // word_source_vals(extractor->getSourceOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::WORD)),
            // word_dest_vals(extractor->getDestOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::WORD)),
            word_n_sources(source_opinfo.getNTypes(InstMetaData::OperandTypes::WORD)),
            word_n_dests(dest_opinfo.getNTypes(InstMetaData::OperandTypes::WORD)),
            word_n_operands(word_n_sources + word_n_dests),

            long_sources(
                extractor->getSourceOperTypeRegs(icode, meta, InstMetaData::OperandTypes::LONG)),
            long_dests(
                extractor->getDestOperTypeRegs(icode, meta, InstMetaData::OperandTypes::LONG)),
            long_operands(long_sources | long_dests),
            // long_oper_vals(extractor->getOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::LONG)),
            // long_source_vals(extractor->getSourceOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::LONG)),
            // long_dest_vals(extractor->getDestOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::LONG)),
            long_n_sources(source_opinfo.getNTypes(InstMetaData::OperandTypes::LONG)),
            long_n_dests(dest_opinfo.getNTypes(InstMetaData::OperandTypes::LONG)),
            long_n_operands(long_n_sources + long_n_dests),

            int_sources(word_sources | long_sources),
            int_dests(word_dests | long_dests),
            int_operands(int_sources | int_dests),
            // int_oper_vals(combineVals_(word_oper_vals, long_oper_vals)),
            // int_source_vals(combineVals_(word_source_vals, long_source_vals)),
            // int_dest_vals(combineVals_(word_dest_vals, long_dest_vals)),
            int_n_sources(word_n_sources + long_n_sources),
            int_n_dests(word_n_dests + long_n_dests),
            int_n_operands(int_n_sources + int_n_dests),

            half_sources(
                extractor->getSourceOperTypeRegs(icode, meta, InstMetaData::OperandTypes::HALF)),
            half_dests(
                extractor->getDestOperTypeRegs(icode, meta, InstMetaData::OperandTypes::HALF)),
            half_operands(half_sources | half_dests),
            // half_oper_vals(extractor->getOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::HALF)),
            // half_source_vals(extractor->getSourceOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::HALF)),
            // half_dest_vals(extractor->getDestOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::HALF)),
            half_n_sources(source_opinfo.getNTypes(InstMetaData::OperandTypes::HALF)),
            half_n_dests(dest_opinfo.getNTypes(InstMetaData::OperandTypes::HALF)),
            half_n_operands(half_n_sources + half_n_dests),

            single_sources(
                extractor->getSourceOperTypeRegs(icode, meta, InstMetaData::OperandTypes::SINGLE)),
            single_dests(
                extractor->getDestOperTypeRegs(icode, meta, InstMetaData::OperandTypes::SINGLE)),
            single_operands(single_sources | single_dests),
            // single_oper_vals(extractor->getOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::SINGLE)),
            // single_source_vals(extractor->getSourceOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::SINGLE)),
            // single_dest_vals(extractor->getDestOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::SINGLE)),
            single_n_sources(source_opinfo.getNTypes(InstMetaData::OperandTypes::SINGLE)),
            single_n_dests(dest_opinfo.getNTypes(InstMetaData::OperandTypes::SINGLE)),
            single_n_operands(single_n_sources + single_n_dests),

            double_sources(
                extractor->getSourceOperTypeRegs(icode, meta, InstMetaData::OperandTypes::DOUBLE)),
            double_dests(
                extractor->getDestOperTypeRegs(icode, meta, InstMetaData::OperandTypes::DOUBLE)),
            double_operands(double_sources | double_dests),
            // double_oper_vals(extractor->getOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::DOUBLE)),
            // double_source_vals(extractor->getSourceOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::DOUBLE)),
            // double_dest_vals(extractor->getDestOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::DOUBLE)),
            double_n_sources(source_opinfo.getNTypes(InstMetaData::OperandTypes::DOUBLE)),
            double_n_dests(dest_opinfo.getNTypes(InstMetaData::OperandTypes::DOUBLE)),
            double_n_operands(double_n_sources + double_n_dests),

            float_sources(half_sources | single_sources | double_sources),
            float_dests(half_dests | single_dests | double_dests),
            float_operands(float_sources | float_dests),
            // float_oper_vals(combineVals_(single_oper_vals, double_oper_vals)),
            // float_source_vals(combineVals_(single_source_vals, double_source_vals)),
            // float_dest_vals(combineVals_(single_dest_vals, double_dest_vals)),
            float_n_sources(half_n_sources + single_n_sources + double_n_sources),
            float_n_dests(half_n_dests + single_n_dests + double_n_dests),
            float_n_operands(float_n_sources + float_n_dests),

            quad_sources(
                extractor->getSourceOperTypeRegs(icode, meta, InstMetaData::OperandTypes::QUAD)),
            quad_dests(
                extractor->getDestOperTypeRegs(icode, meta, InstMetaData::OperandTypes::QUAD)),
            quad_operands(quad_sources | quad_dests),
            // quad_oper_vals(extractor->getOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::QUAD)),
            // quad_source_vals(extractor->getSourceOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::QUAD)),
            // quad_dest_vals(extractor->getDestOperTypeList(icode, meta,
            // InstMetaData::OperandTypes::QUAD)),
            quad_n_sources(source_opinfo.getNTypes(InstMetaData::OperandTypes::QUAD)),
            quad_n_dests(dest_opinfo.getNTypes(InstMetaData::OperandTypes::QUAD)),
            quad_n_operands(quad_n_sources + quad_n_dests),

            vector_sources(
                extractor->getSourceOperTypeRegs(icode, meta, InstMetaData::OperandTypes::VECTOR)),
            vector_dests(
                extractor->getDestOperTypeRegs(icode, meta, InstMetaData::OperandTypes::VECTOR)),
            vector_operands(vector_sources | vector_dests),
            vector_n_sources(source_opinfo.getNTypes(InstMetaData::OperandTypes::VECTOR)),
            vector_n_dests(dest_opinfo.getNTypes(InstMetaData::OperandTypes::VECTOR)),
            vector_n_operands(vector_n_sources + vector_n_dests),

            addr_sources(extractor->getSourceAddressRegs(icode)),
            // addr_source_vals(extractor->getSourceAddressList(icode)),
            // addr_n_sources(addr_source_vals.size()),

            data_sources(extractor->getSourceDataRegs(icode)),
            // data_source_vals(extractor->getSourceDataList(icode)),
            // data_n_sources(data_source_vals.size()),

            is_hint(extractor->isHint(icode)),
            immediate_type(extractor->getImmediateType()),
            has_immediate(extractor->hasImmediate()),
            immediate(extractor->getImmediate(icode)),
            signed_offset(extractor->getSignedOffset(icode)),
            data_size(meta->getDataSize())
        // function(extractor->getFunction(icode))
        {
#if 1
            // TODO: Move this into a sanity checking method
            // FOR NOW: We bypass these checks if we're given an pseudo op (opcode = 0, e.g. from
            // makeInstDirectly() et al)
            if (icode != 0)
            {
                const std::string & form_name = extractor->getName();
                (void)form_name;

                // OperandArray    x_arr = extractor->getSourceList(icode);
                // assert(agree_(x_arr, sources, "sources", form_name));

                // x_arr = extractor->getSourceAddressList(icode);
                // assert(agree_(x_arr, addr_sources, "addr_sources", form_name));

                // x_arr = extractor->getSourceDataList(icode);
                // assert(agree_(x_arr, data_sources, "data_sources", form_name));

                // x_arr = extractor->getDestList(icode);
                // assert(agree_(x_arr, dests, "dests", form_name));

                // for (uint32_t i = 0;
                //      i <
                //      static_cast<std::underlying_type_t<InstMetaData::OperandTypes>>(InstMetaData::OperandTypes::__N);
                //      ++i) {
                //     auto kind = static_cast<InstMetaData::OperandTypes>(i);
                //
                //     x_arr = extractor->getSourceOperTypeList(icode, meta, kind);
                //     uint64_t bits = extractor->getSourceOperTypeRegs(icode, meta, kind);
                //     assert(agree_(x_arr, bits, "source_opertypes", form_name));
                //
                //     x_arr = extractor->getDestOperTypeList(icode, meta, kind);
                //     bits = extractor->getDestOperTypeRegs(icode, meta, kind);
                //     assert(agree_(x_arr, bits, "dest_opertypes", form_name));
                // }

                // assert(agree_(word_oper_vals, word_operands, "word_operands", form_name));
                // assert(agree_(word_source_vals, word_sources, "word_sources", form_name));
                // assert(agree_(word_dest_vals, word_dests, "word_dests", form_name));

                // assert(agree_(long_oper_vals, long_operands, "long_operands", form_name));
                // assert(agree_(long_source_vals, long_sources, "long_sources", form_name));
                // assert(agree_(long_dest_vals, long_dests, "long_dests", form_name));

                // assert(agree_(int_oper_vals, int_operands, "int_operands", form_name));
                // assert(agree_(int_source_vals, int_sources, "int_sources", form_name));
                // assert(agree_(int_dest_vals, int_dests, "int_dests", form_name));

                // assert(agree_(single_oper_vals, single_operands, "single_operands", form_name));
                // assert(agree_(single_source_vals, single_sources, "single_sources", form_name));
                // assert(agree_(single_dest_vals, single_dests, "single_dests", form_name));

                // assert(agree_(double_oper_vals, double_operands, "double_operands", form_name));
                // assert(agree_(double_source_vals, double_sources, "double_sources", form_name));
                // assert(agree_(double_dest_vals, double_dests, "double_dests", form_name));

                // assert(agree_(float_oper_vals, float_operands, "float_operands", form_name));
                // assert(agree_(float_source_vals, float_sources, "float_sources", form_name));
                // assert(agree_(float_dest_vals, float_dests, "float_dests", form_name));

                // assert(agree_(quad_oper_vals, quad_operands, "quad_operands", form_name));
                // assert(agree_(quad_source_vals, quad_sources, "quad_sources", form_name));
                // assert(agree_(quad_dest_vals, quad_dests, "quad_dests", form_name));

                // assert(agree_(addr_source_vals, addr_sources, "addr_sources", form_name));
                // assert(agree_(data_source_vals, data_sources, "data_sources", form_name));

                assert(agree_opinfo_(source_opinfo_list, sources, "sources", form_name));
                assert(agree_opinfo_(source_opinfo_list, word_sources, "word_sources", form_name,
                                     InstMetaData::OperandTypes::WORD));
                assert(agree_opinfo_(source_opinfo_list, long_sources, "long_sources", form_name,
                                     InstMetaData::OperandTypes::LONG));
                assert(agree_opinfo_(source_opinfo_list, half_sources, "half_sources",
                                     form_name, InstMetaData::OperandTypes::HALF));
                assert(agree_opinfo_(source_opinfo_list, single_sources, "single_sources",
                                     form_name, InstMetaData::OperandTypes::SINGLE));
                assert(agree_opinfo_(source_opinfo_list, double_sources, "double_sources",
                                     form_name, InstMetaData::OperandTypes::DOUBLE));
                assert(agree_opinfo_(source_opinfo_list, quad_sources, "quad_sources", form_name,
                                     InstMetaData::OperandTypes::QUAD));
                assert(agree_opinfo_(source_opinfo_list, vector_sources, "vector_sources",
                                     form_name, InstMetaData::OperandTypes::VECTOR));

                assert(agree_opinfo_(dest_opinfo_list, dests, "dests", form_name));
                assert(agree_opinfo_(dest_opinfo_list, word_dests, "word_dests", form_name,
                                     InstMetaData::OperandTypes::WORD));
                assert(agree_opinfo_(dest_opinfo_list, long_dests, "long_dests", form_name,
                                     InstMetaData::OperandTypes::LONG));
                assert(agree_opinfo_(dest_opinfo_list, half_dests, "half_dests", form_name,
                                     InstMetaData::OperandTypes::HALF));
                assert(agree_opinfo_(dest_opinfo_list, single_dests, "single_dests", form_name,
                                     InstMetaData::OperandTypes::SINGLE));
                assert(agree_opinfo_(dest_opinfo_list, double_dests, "double_dests", form_name,
                                     InstMetaData::OperandTypes::DOUBLE));
                assert(agree_opinfo_(dest_opinfo_list, quad_dests, "quad_dests", form_name,
                                     InstMetaData::OperandTypes::QUAD));
                assert(agree_opinfo_(dest_opinfo_list, vector_dests, "vector_dests", form_name,
                                     InstMetaData::OperandTypes::VECTOR));
            }
#endif

            // Certain instruction type information is most readily available from extracted
            // data. The extracted type information is used to future qualify extraction-independent
            // information provided in the meta-data (meta) that is coded in the JSON ISA file
            // (i.e. via meta->getInstType/isInstType)

            // Here, we qualify JAL and JALR instructions with call/return/indirect type
            // information, based on the source/dest register values
            const uint64_t link_regs = (1ull << REGISTER_LINK) | (1ull << REGISTER_ALT_LINK);
            if (meta->isInstType(InstMetaData::InstructionTypes::JAL)
                || meta->isInstType(InstMetaData::InstructionTypes::JALR))
            {
                // If the dest reg is one of the link registers, we include the CALL kind
                if ((dests & link_regs) != 0)
                {
                    ext_itype |= static_cast<std::underlying_type_t<ExtractedInstTypes>>(
                        ExtractedInstTypes::CALL);
                }

                // If the source reg is one of the link registers, we include the RETURN kind
                if ((sources & link_regs) != 0)
                {
                    // ...unless the source and dest link registers are the same
                    // The below test is safer than checking sources == dests
                    if ((sources & dests & link_regs) == 0)
                    {
                        ext_itype |= static_cast<std::underlying_type_t<ExtractedInstTypes>>(
                            ExtractedInstTypes::RETURN);
                    }
                }
                else if (hasSourceReg_(sources))
                {
                    ext_itype |= static_cast<std::underlying_type_t<ExtractedInstTypes>>(
                        ExtractedInstTypes::INDIRECT);
                }
            }

            // Cache all special field values
            for (const auto& [sf_name, sf_id] : ExtractorIF::SpecialFieldMap)
            {
                try
                {
                    (void)sf_name;
                    const uint64_t sf_val = extractor->getSpecialField(sf_id, icode, meta);
                    special_fields.emplace(sf_id, sf_val);
                }
                catch (const InvalidExtractorSpecialFieldID & ex)
                {
                    // Instruction does not have any special fields
                    break;
                }
                catch (const UnsupportedExtractorSpecialFieldID & ex)
                {
                    // Instruction has special fields, but not this one
                    continue;
                }
            }
        }

        bool isExtInstType(ExtractedInstTypes itype) const
        {
            return (static_cast<std::underlying_type_t<ExtractedInstTypes>>(itype) & ext_itype)
                   == static_cast<std::underlying_type_t<ExtractedInstTypes>>(itype);
        }

        bool isExtInstTypeAll(std::underlying_type_t<ExtractedInstTypes> itype) const
        {
            return (itype & ext_itype) == itype;
        }

        bool isExtInstTypeAny(std::underlying_type_t<ExtractedInstTypes> itype) const
        {
            return (itype & ext_itype) != 0;
        }
    };

} // namespace mavis
