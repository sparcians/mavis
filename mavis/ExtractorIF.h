#pragma once

#include "InstMetaData.h"
#include "OperandInfo.hpp"
#include <map>
#include <algorithm>

namespace mavis {
/**
 * ExtractorIF: Interface to Extractors
 */
class ExtractorIF
{
    friend class ExtractorWrap;

public:
    using OpcodeFieldValueType = OperandInfo::OpcodeFieldValueType;

    typedef std::shared_ptr<ExtractorIF>        PtrType;
    typedef std::vector<OpcodeFieldValueType>   RegListType;
    typedef std::vector<uint32_t>               ValueListType;

    // TODO: Maybe make SpecialField into its own class (for string name <--> enum conversions)
    enum class SpecialField : uint32_t
    {
        AQ = 0,     // AQ "acquire" bit in lr.w, sc.w, and atomics
        AVL,        // AVL "immediate" field in vsetivli
        CSR,        // CSR field in csr* instructions
        FM,         // FENCE mode bits
        NF,         // NF field in vector memory instructions
        PRED,       // FENCE predecessor bits
        RL,         // RL "release" bit in lr.w, sc.w, and atomics
        RM,         // RM "rounding mode" bit in FP instructions
        SUCC,       // FENCE successor bits
        VM,         // VM bit in vector insts
        WD,         // WD in vector atomic insts
        __N
    };

    static inline const std::map<const std::string, SpecialField> SpecialFieldMap {
        { "aq",    SpecialField::AQ},
        { "avl",   SpecialField::AVL},
        { "csr",   SpecialField::CSR},
        { "fm",    SpecialField::FM},
        { "nf",    SpecialField::NF},
        { "pred",  SpecialField::PRED},
        { "rl",    SpecialField::RL},
        { "rm",    SpecialField::RM},
        { "succ",  SpecialField::SUCC},
        { "vm",    SpecialField::VM},
        { "wd",    SpecialField::WD}
    };

private:
    static inline const std::array<const std::string, static_cast<std::underlying_type_t<SpecialField>>(SpecialField::__N)> spec_field_name_ {
        "aq",
        "avl",
        "csr",
        "fm",
        "nf",
        "pred",
        "rl",
        "rm",
        "succ",
        "vm",
        "wd"
    };

public:
    static inline const std::string& getSpecialFieldName(SpecialField sid) {
        assert(sid != SpecialField::__N);
        return spec_field_name_[static_cast<std::underlying_type_t<SpecialField>>(sid)];
    }

public:
    virtual ~ExtractorIF() = default;

    virtual ExtractorIF::PtrType specialCaseClone(uint64_t ffmask, uint64_t fset) const = 0;

    virtual std::string getName() const = 0;

    virtual bool isIllop(Opcode icode) const = 0;

    virtual bool isHint(Opcode icode) const = 0;

    virtual uint64_t getSourceRegs(Opcode icode) const = 0;

    virtual uint64_t getSourceAddressRegs(Opcode icode) const = 0;

    virtual uint64_t getSourceDataRegs(Opcode icode) const = 0;

    virtual uint64_t getDestRegs(Opcode icode) const = 0;

    virtual uint64_t getSourceOperTypeRegs(Opcode icode,
                                           const InstMetaData::PtrType &meta,
                                           InstMetaData::OperandTypes kind) const = 0;

    virtual uint64_t getDestOperTypeRegs(Opcode icode,
                                         const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const = 0;

    // TODO: Remove this, unless used outside of DecodedInstInfo
    virtual uint64_t getOperTypeRegs(Opcode icode,
                                     const InstMetaData::PtrType &meta, InstMetaData::OperandTypes kind) const
    {
        return getSourceOperTypeRegs(icode, meta, kind) | getDestOperTypeRegs(icode, meta, kind);
    }

    virtual OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                             bool suppress_x0 = false) const = 0;

    virtual OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                           bool suppress_x0 = false) const = 0;

    virtual ImmediateType getImmediateType() const = 0;

    bool hasImmediate() const
    {
        return getImmediateType() != ImmediateType::NONE;
    }

    virtual uint64_t getImmediate(Opcode icode) const = 0;

    // Just an alias to getImmediate() for symmetry with getSignedOffset()
    uint64_t getUnsignedOffset(Opcode icode) const
    {
        return getImmediate(icode);
    }

    // TODO: Consider a default implementation for this, by
    // moving the ExtractorBase version here, and getting rid
    // of the identical versions for ExtractorDirectInfo and
    // ExtractorTraceInfo. Do we really need separate versions anymore?
    virtual int64_t getSignedOffset(Opcode icode) const = 0;

    virtual uint64_t getSpecialField(SpecialField sfid, Opcode icode) const = 0;

    virtual std::string dasmString(const std::string &mnemonic, Opcode icode) const = 0;

    // This version of dasmString can be overridden in the derived classes to take advantage
    // of operand type information in the InstMetaData object (e.g. for providing the correct
    // register prefix characters such as "x" for 64-bit int, "f" for float, and "v" for
    // vector). If unimplemented, we revert to the standard call to dasmString()
    virtual std::string dasmString(const std::string &mnemonic, Opcode icode, const InstMetaData::PtrType&) const
    {
        return dasmString(mnemonic, icode);
    }

    virtual void dasmAnnotate(const std::string& txt) = 0;
    virtual const std::string& getDasmAnnotation() const = 0;

    virtual void print(std::ostream &os) const = 0;

protected:
    virtual uint64_t getSpecialFieldByIndex_(uint32_t) const {
        assert(false);
        return 0;
    }
};

} // namespace mavis
