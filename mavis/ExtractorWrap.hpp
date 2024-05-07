#pragma once

#include "Extractor.h"
#include "FormGeneric.hpp"
#include <sstream>

namespace mavis {

/**
 * @brief ExtractorWrap: This is a wrapper for ExtractorIF which augments the getOperandInfo*()
 * methods with information from the generic form (part of PseudoInst extraction, where the
 * pseudo-instructions aren't associated with any standard Form in the ISA
 *
 * See IFactoryPseudo.hpp for how this is used
 */
class ExtractorWrap : public ExtractorIF
{
public:
    ExtractorWrap(const ExtractorIF::PtrType& edi, const FormGeneric::PtrType& form) :
        obj_(edi), form_(form)
    {
        std::stringstream ss;
        ss << "wrapped(" << obj_->getName() << ")";
        name_ = ss.str();
    }

    ExtractorWrap(const ExtractorWrap &other) = delete;

    std::string getName() const override
    {
        return name_;
    }

    bool isIllop(Opcode icode) const override
    {
        return obj_->isIllop(icode);
    }

    bool isHint(Opcode icode) const override
    {
        return obj_->isHint(icode);
    }

    uint64_t getSourceRegs(Opcode icode) const override
    {
        return obj_->getSourceRegs(icode);
    }

    uint64_t getSourceAddressRegs(Opcode icode) const override
    {
        return obj_->getSourceAddressRegs(icode);
    }

    uint64_t getSourceDataRegs(Opcode icode) const override
    {
        return obj_->getSourceDataRegs(icode);
    }

    uint64_t getDestRegs(Opcode icode) const override
    {
        return obj_->getDestRegs(icode);
    }

    uint64_t getSourceOperTypeRegs(Opcode icode,
                                   const InstMetaData::PtrType &meta,
                                   InstMetaData::OperandTypes kind) const override
    {
        const OperandInfo::ElementList oilist = getSourceOperandInfo(icode, meta).getElements();
        uint64_t result = 0;
        for (const auto& oper : oilist) {
            if (oper.operand_type == kind) {
                result |= (0x1ull << oper.field_value);
            }
        }
        return result;
    }

    uint64_t getDestOperTypeRegs(Opcode icode,
                                 const InstMetaData::PtrType &meta,
                                 InstMetaData::OperandTypes kind) const override
    {
        const OperandInfo::ElementList oilist = getDestOperandInfo(icode, meta).getElements();
        uint64_t result = 0;
        for (const auto& oper : oilist) {
            if (oper.operand_type == kind) {
                result |= (0x1ull << oper.field_value);
            }
        }
        return result;
    }

    OperandInfo getSourceOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                     bool suppress_x0 = false) const override
    {
        if (icode == 0) {
            if (op0_src_oi_ == nullptr) {
                OperandInfo olist = obj_->getSourceOperandInfo(icode, meta, suppress_x0);
                op0_src_oi_ = std::make_unique<OperandInfo>(form_->fixupOISources(olist));
            }
            return *op0_src_oi_;
        } else {
            OperandInfo olist = obj_->getSourceOperandInfo(icode, meta, suppress_x0);
            // RVO should cover moving the OperandInfo temporary here
            return form_->fixupOISources(olist);
        }
    }

    OperandInfo getDestOperandInfo(Opcode icode, const InstMetaData::PtrType& meta,
                                   bool suppress_x0 = false) const override
    {
        if (icode == 0) {
            if (op0_dest_oi_ == nullptr) {
                OperandInfo olist = obj_->getDestOperandInfo(icode, meta, suppress_x0);
                op0_dest_oi_ = std::make_unique<OperandInfo>(form_->fixupOIDests(olist));
            }
            return *op0_dest_oi_;
        } else {
            OperandInfo olist = obj_->getDestOperandInfo(icode, meta, suppress_x0);
            // RVO should cover moving the OperandInfo temporary here
            return form_->fixupOIDests(olist);
        }
    }

    ImmediateType getImmediateType() const override
    {
        return obj_->getImmediateType();
    }

    uint64_t getImmediate(Opcode icode) const override
    {
        return obj_->getImmediate(icode);
    }

    int64_t getSignedOffset(Opcode icode) const override
    {
        return obj_->getSignedOffset(icode);
    }

    uint64_t getSpecialField(SpecialField sfid, Opcode icode) const override
    {
        uint32_t index = form_->getSpecialFieldIndex(sfid);
        if (index == FormGeneric::INVALID_LIST_POS) {
            throw UnsupportedExtractorSpecialFieldID(getSpecialFieldName(sfid), icode);
        } else {
            return obj_->getSpecialFieldByIndex_(index);
        }
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode) const override
    {
        return obj_->dasmString(mnemonic, icode);
    }

    std::string dasmString(const std::string &mnemonic, const Opcode icode, const InstMetaData::PtrType& meta) const override
    {
        return obj_->dasmString(mnemonic, icode, meta);
    }

    void dasmAnnotate(const std::string& txt) override
    {
        obj_->dasmAnnotate(txt);
    }

    const std::string& getDasmAnnotation() const override
    {
        return obj_->getDasmAnnotation();
    }

    void print(std::ostream &os) const override
    {
        obj_->print(os);
    }

private:
    const ExtractorIF::PtrType      obj_;
    const FormGeneric::PtrType      form_;
    std::string                     name_;
    mutable std::unique_ptr<OperandInfo>    op0_src_oi_;        // "Cached" OperandInfo for 0 opcodes
    mutable std::unique_ptr<OperandInfo>    op0_dest_oi_;       // "Cached" OperandInfo for 0 opcodes

private:
    ExtractorIF::PtrType specialCaseClone(const uint64_t, const uint64_t) const override
    {
        assert(false);
        return nullptr;
    }
};

} // namespace mavis
