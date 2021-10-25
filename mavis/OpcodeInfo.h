#pragma once

#include <bitset>
#include "DecoderTypes.h"
#include "DecodedInstInfo.h"
#include "InstMetaData.h"
#include "DisassemblerIF.hpp"

namespace mavis {

// This is really just to make coding accesses within Instruction easier
// It will cause a small hit to performance
// Need to decide whether this is worth it
// TODO: Add proxy interfaces for the operand type stuff
class OpcodeInfo
{
public:
    typedef std::shared_ptr<OpcodeInfo> PtrType;

    using InstructionTypes = InstMetaData::InstructionTypes;
    using ISAExtension = InstMetaData::ISAExtension;
    using OperandTypes = InstMetaData::OperandTypes;
    using ExtractedInstTypes = DecodedInstructionInfo::ExtractedInstTypes;
    using SpecialField = ExtractorIF::SpecialField;

public:
    OpcodeInfo(const Opcode icode, const DecodedInstructionInfo::PtrType& dii, const ExtractorIF::PtrType& extractor,
               const InstMetaData::PtrType& meta, const DisassemblerIF::PtrType& dasm) :
        icode_(icode), info_(dii), extractor_(extractor), meta_(meta), dasm_(dasm)
    {}

    const std::string& getMnemonic() const
    {
        return info_->mnemonic;
    }

    InstructionUniqueID getInstructionUniqueID() const
    {
        return info_->unique_id;
    }

    Opcode getOpcode() const
    {
        return icode_;
    }

    std::string dasmString() const
    {
        return dasm_->toString(getMnemonic(), icode_, meta_, extractor_);
        //return extractor_->dasmString(getMnemonic(), icode_, meta_);
    }

    bool isHint() const
    {
        return info_->is_hint;
    }

    bool hasImmediate() const
    {
        return info_->has_immediate;
    }

    uint64_t getImmediate() const
    {
        return info_->immediate;
    }

    uint64_t getUnsignedOffset() const
    {
        return info_->immediate;
    }

    int64_t getSignedOffset() const
    {
        return info_->signed_offset;
    }

    DecodedInstructionInfo::BitMask getSourceRegs() const
    {
        return info_->sources;
    }

    const OperandInfo& getSourceOpInfo() const
    {
        return info_->source_opinfo;
    }

    const OperandInfo::ElementList& getSourceOpInfoList() const
    {
        return info_->source_opinfo_list;
    }

    DecodedInstructionInfo::BitMask getIntSourceRegs() const
    {
        return info_->int_sources;
    }

    DecodedInstructionInfo::BitMask getFloatSourceRegs() const
    {
        return info_->float_sources;
    }

    DecodedInstructionInfo::BitMask getVectorSourceRegs() const
    {
        return info_->vector_sources;
    }

    DecodedInstructionInfo::BitMask getSourceRegsByType(OperandTypes otype) const
    {
        switch (otype) {
            case OperandTypes::WORD:
                return info_->word_sources;
            case OperandTypes::LONG:
                return info_->long_sources;
            case OperandTypes::SINGLE:
                return info_->single_sources;
            case OperandTypes::DOUBLE:
                return info_->double_sources;
            case OperandTypes::QUAD:
                return info_->quad_sources;
            case OperandTypes::VECTOR:
                return info_->vector_sources;
            case OperandTypes::__N:
                assert(false && "invalid operand type");
        }
        return DecodedInstructionInfo::BitMask();
    }

    uint32_t numSourceRegs() const
    {
        return info_->n_sources;
    }

    uint32_t numIntSourceRegs() const
    {
        return info_->int_n_sources;
    }

    uint32_t numFloatSourceRegs() const
    {
        return info_->float_n_sources;
    }

    uint32_t numVectorSourceRegs() const
    {
        return info_->vector_n_sources;
    }

    uint32_t numSourceRegsByType(OperandTypes otype) const
    {
        switch (otype) {
            case OperandTypes::WORD:
                return info_->word_n_sources;
            case OperandTypes::LONG:
                return info_->long_n_sources;
            case OperandTypes::SINGLE:
                return info_->single_n_sources;
            case OperandTypes::DOUBLE:
                return info_->double_n_sources;
            case OperandTypes::QUAD:
                return info_->quad_n_sources;
            case OperandTypes::VECTOR:
                return info_->vector_n_sources;
            case OperandTypes::__N:
                assert(false && "invalid operand type");
        }
        return std::numeric_limits<uint32_t>::max();
    }

    DecodedInstructionInfo::BitMask getDestRegs() const
    {
        return info_->dests;
    }

    const OperandInfo& getDestOpInfo() const
    {
        return info_->dest_opinfo;
    }

    const OperandInfo::ElementList& getDestOpInfoList() const
    {
        return info_->dest_opinfo_list;
    }

    DecodedInstructionInfo::BitMask getIntDestRegs() const
    {
        return info_->int_dests;
    }

    DecodedInstructionInfo::BitMask getFloatDestRegs() const
    {
        return info_->float_dests;
    }

    DecodedInstructionInfo::BitMask getVectorDestRegs() const
    {
        return info_->vector_dests;
    }

    DecodedInstructionInfo::BitMask getDestRegsByType(OperandTypes otype) const
    {
        switch (otype) {
            case OperandTypes::WORD:
                return info_->word_dests;
            case OperandTypes::LONG:
                return info_->long_dests;
            case OperandTypes::SINGLE:
                return info_->single_dests;
            case OperandTypes::DOUBLE:
                return info_->double_dests;
            case OperandTypes::QUAD:
                return info_->quad_dests;
            case OperandTypes::VECTOR:
                return info_->vector_dests;
            case OperandTypes::__N:
                assert(false && "invalid operand type");
        }
        return DecodedInstructionInfo::BitMask();
    }

    uint32_t numDestRegs() const
    {
        return info_->n_dests;
    }

    uint32_t numIntDestRegs() const
    {
        return info_->int_n_dests;
    }

    uint32_t numFloatDestRegs() const
    {
        return info_->float_n_dests;
    }

    uint32_t numVectorDestRegs() const
    {
        return info_->vector_n_dests;
    }

    uint32_t numDestRegsByType(OperandTypes otype) const
    {
        switch (otype) {
            case OperandTypes::WORD:
                return info_->word_n_dests;
            case OperandTypes::LONG:
                return info_->long_n_dests;
            case OperandTypes::SINGLE:
                return info_->single_n_dests;
            case OperandTypes::DOUBLE:
                return info_->double_n_dests;
            case OperandTypes::QUAD:
                return info_->quad_n_dests;
            case OperandTypes::VECTOR:
                return info_->vector_n_dests;
            case OperandTypes::__N:
                assert(false && "invalid operand type");
        }
        return std::numeric_limits<uint32_t>::max();
    }

    DecodedInstructionInfo::BitMask getSourceAddressRegs() const
    {
        return info_->addr_sources;
    }

    DecodedInstructionInfo::BitMask getSourceDataRegs() const
    {
        return info_->data_sources;
    }

    // TODO: Deprecate this completely... still used in Dabble::Instruction() for printing
    uint64_t getFunction() const
    {
        //return info_->function;
        return 0;
    }

    std::underlying_type_t<InstructionTypes> getInstType() const
    {
        return meta_->getInstType();
    }

    bool isInstType(InstructionTypes itype) const
    {
        return meta_->isInstType(itype);
    }

    template<typename ...ArgTypes>
    bool isInstTypeAnyOf(ArgTypes&& ... args) const
    {
        return meta_->isInstTypeAnyOf(std::forward<ArgTypes>(args)...);
    }

    template<typename ...ArgTypes>
    bool isInstTypeAllOf(ArgTypes&& ... args) const
    {
        return meta_->isInstTypeAllOf(std::forward<ArgTypes>(args)...);
    }

    template<typename ...ArgTypes>
    bool isInstTypeSameAs(std::underlying_type_t<InstructionTypes> other_types, ArgTypes&& ... args) const
    {
        return meta_->isInstTypeSameAs(other_types, std::forward<ArgTypes>(args)...);
    }

    bool isExtractedInstType(ExtractedInstTypes itype) const
    {
        return info_->isExtInstType(itype);
    }

    std::underlying_type_t<ISAExtension> getISA() const
    {
        return meta_->getISA();
    }

    bool isISA(ISAExtension isa) const
    {
        return meta_->isISA(isa);
    }

    template<typename ...ArgTypes>
    bool isISAAnyOf(ArgTypes&& ... args) const
    {
        return meta_->isISAAnyOf(std::forward<ArgTypes>(args)...);
    }

    template<typename ...ArgTypes>
    bool isISAAllOf(ArgTypes&& ... args) const
    {
        return meta_->isISAAllOf(std::forward<ArgTypes>(args)...);
    }

    template<typename ...ArgTypes>
    bool isISASameAs(std::underlying_type_t<ISAExtension> other_isa, ArgTypes&& ... args) const
    {
        return meta_->isISASameAs(other_isa, std::forward<ArgTypes>(args)...);
    }


    uint32_t getDataSize() const
    {
        return meta_->getDataSize();
    }

    // NOTE: We don't cache the special fields in the decoded
    // instruction object (i.e. info_). Athena wants exceptions to be thrown
    // for invalid special field requests, and supporting that makes the
    // Mavis extraction cacheing expensive.
    //
    // There's not much love lost here, since special field requests
    // should be relatively infrequent (compared to immediate and register
    // extractions which almost every instruction has).
    //
    // If we absolutely must cache the special fields, I would suggest
    // cacheing "SpecialFieldObjects" as part of the decoded instruction
    // info (info_). These objects would be provided by the extractors with
    // either the extracted field data, or constructed to cause an exception
    // when attempt is made to "get" the data value.
    //
    // This getSpecialField() method would then invoke the "SpecialFieldObject::get()"
    // method to return the value or throw the exception.
    uint64_t getSpecialField(SpecialField sfid)
    {
        try {
            return extractor_->getSpecialField(sfid, icode_);
        } catch (const UnsupportedExtractorSpecialFieldID& ex) {
            // We can provide the mnemonic here, so we "rethrow"
            // the original exception with more information
            throw UnsupportedExtractorSpecialFieldID(getMnemonic(), ex);
        }
    }

private:
    const Opcode icode_;
    const DecodedInstructionInfo::PtrType info_;
    const ExtractorIF::PtrType extractor_;
    const InstMetaData::PtrType meta_;
    const DisassemblerIF::PtrType dasm_;

    // Used to keep clang/gcc from complaining about no return functions.
    DecodedInstructionInfo::OperandArray __bad_array_;
};

} // namespace mavis
