#pragma once

#include "json.hpp"
#include "Form.h"
#include "DecoderTypes.h"
#include "DecoderExceptions.h"
//#include "IFactoryBuilder.h"
#include "InstMetaData.h"
#include "Disassembler.hpp"
#include <memory>
#include <array>

namespace mavis {

template<typename InstType, typename AnnotationType>
class Overlay
{
private:
    using json = nlohmann::json;
    typedef typename std::vector<std::string> FieldNameListType;
    typedef typename std::vector<std::string> MatchMaskValType;

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

public:
    typedef std::shared_ptr<Overlay<InstType, AnnotationType>> PtrType;

public:
    Overlay(const std::string& mnemonic, const json& olay, const json& inst, const ExtractorIF::PtrType& xform_extractor) :
        mnemonic_(mnemonic), json_inst_(inst), xform_extractor_(xform_extractor)
    {
        // Parse the JSON overlay section to set up this object
        if (olay.find("base") != olay.end()) {
            base_mnemonic_ = static_cast<std::string>(olay["base"]);
        } else {
            throw BuildErrorOverlayMissingBase(mnemonic);
        }

        if (olay.find("match") != olay.end()) {
            MatchMaskValType mlist = olay["match"].get<MatchMaskValType>();
            if (mlist.size() != 2) {
                throw BuildErrorOverlayBadMatchSpec(mnemonic);
            }
            match_mask_ = std::strtoull(mlist[0].c_str(), nullptr, 0);
            match_value_ = std::strtoull(mlist[1].c_str(), nullptr, 0);
            n_match_mask_bits_ = count1Bits_(match_mask_);
        } else {
            throw BuildErrorOverlayMissingMatch(mnemonic);
        }

        // TODO: Handle any disassembler overrides here. For now, we just create an
        // empty disassembler
        dasm_ = std::make_shared<Disassembler>();

#if 0
        // Ask the builder for the base instruction's meta data information
        // We'll make a copy of this to use as our own meta data, and update it with
        // any changes from our JSON spec
        InstMetaData::PtrType base_meta = builder_.findMetaData(base_mnemonic_);
        meta_ = base_meta->clone();
        meta_->parseOverrides(inst);

        // Register this mnemonic with the builder's UID registry
        uid_ = builder_.registerInst(mnemonic);

        // Attempt to find the annotation for the overlay.
        // If not found, we use the annotation for the base
        anno_ = builder_.findAnnotation(mnemonic);
        if (anno_ == nullptr) {
            anno_ = builder_.findAnnotation(base_mnemonic_);
        }
        if (anno_ == nullptr) {
            // throw
        }
#endif
    }

    std::string getMnemonic() const
    {
        return mnemonic_;
    }

    std::string getBaseMnemonic() const
    {
        return base_mnemonic_;
    }

    void setBaseMetaData(const InstMetaData::PtrType& base_meta)
    {
        meta_ = base_meta->clone();
        meta_->parseOverrides(json_inst_);
    }

    // Return the override extractor (xform) is supplied in the
    // JSON, or nullptr if no override
    ExtractorIF::PtrType getExtractor() const
    {
        return xform_extractor_;
    }

    InstMetaData::PtrType getMetaData() const
    {
        return meta_;
    }

    DisassemblerIF::PtrType getDasm() const
    {
        return dasm_;
    }

    void setUID(InstructionUniqueID uid)
    {
        uid_ = uid;
    }

    InstructionUniqueID getUID() const
    {
        return uid_;
    }

    void setAnnotation(typename AnnotationType::PtrType& panno)
    {
        anno_ = panno;
    }

    typename AnnotationType::PtrType getAnnotation() const
    {
        return anno_;
    }

    bool isMatch(Opcode icode) const
    {
        return ((icode & match_mask_) == match_value_);
    }

    uint32_t getNumMaskBits() const
    {
        return n_match_mask_bits_;
    }

    void print(std::ostream& os) const
    {
        std::ios_base::fmtflags os_state(os.flags());
        os << "Overlay: '" << mnemonic_ << "'"
           << " -> base '" << base_mnemonic_ << "'"
           << ", match icode & 0x" << std::hex << match_mask_
           << " == 0x" << match_value_
           << " (" << std::dec << n_match_mask_bits_ << " bits)"
           << ", UID:" << std::dec << uid_
           << ", annotation: ";
        if (anno_ == nullptr) {
            os << "NA";
        } else {
            os << *anno_;
        }
        os << std::endl;
        os.flags(os_state);
    }

private:
    std::string mnemonic_;
    std::string base_mnemonic_;
    Opcode match_mask_ = 0;
    Opcode match_value_ = 0;
    uint32_t n_match_mask_bits_ = 0;
    const json& json_inst_;
    ExtractorIF::PtrType xform_extractor_;
    InstMetaData::PtrType meta_;
    DisassemblerIF::PtrType dasm_;
    InstructionUniqueID uid_;
    typename AnnotationType::PtrType anno_;
};

template<typename InstType, typename AnnotationType>
inline std::ostream& operator<<(std::ostream& os, const Overlay<InstType, AnnotationType>& olay)
{
    olay.print(os);
    return os;
}

} // namespace mavis
