#pragma once

#include <vector>
#include <array>
#include <cinttypes>

#include "InstMetaData.h"

namespace mavis {

class OperandInfo
{
public:
    typedef uint32_t OpcodeFieldValueType;

    struct Element {
        InstMetaData::OperandFieldID field_id = InstMetaData::OperandFieldID::NONE;      // Field ID (e.g. RS1, RS2, RD, ...)
        InstMetaData::OperandTypes   operand_type = InstMetaData::OperandTypes::NONE;    // Type (e.g. WORD, LONG, DOUBLE, ...)
        OpcodeFieldValueType         field_value = 0;                                    // Extracted field value from the opcode
        bool                         is_store_data = false;                              // Whether this is a store data operand
        bool                         is_implied = false;                                 // Whether this operand is implied

        Element(InstMetaData::OperandFieldID fid,
                InstMetaData::OperandTypes otype,
                OpcodeFieldValueType val,
                bool is_sdata = false,
                bool is_impl = false):
            field_id(fid),
            operand_type(otype),
            field_value(val),
            is_store_data(is_sdata),
            is_implied(is_impl)
        {}
        Element() = default;
        Element(const Element&) = default;
        Element& operator=(const Element&) = default;
    };

    typedef std::vector<Element> ElementList;

private:
    ElementList     elems_;
    uint32_t        n_opers_ = 0;
    bool            has_implied_operand_ = false;
    using OperandTypesUnderLyingType = std::underlying_type_t<InstMetaData::OperandTypes>;
    std::array<uint32_t, static_cast<OperandTypesUnderLyingType>(InstMetaData::OperandTypes::__N)>  n_types_ = {0};

    struct FieldIDInfo {
        bool                       valid = false;
        OpcodeFieldValueType       value = 0;
        InstMetaData::OperandTypes type = InstMetaData::OperandTypes::NONE;
        bool                       sdata = false;
        bool                       implied = false;

        bool operator==(const Element& elm) const {
            return valid && (value == elm.field_value) && (type == elm.operand_type) && (sdata == elm.is_store_data) &&
                   (implied == elm.is_implied);
        }
    };

    using OperandFieldIDUnderlyingType = std::underlying_type_t<InstMetaData::OperandFieldID>;
    std::array<FieldIDInfo, static_cast<OperandFieldIDUnderlyingType>(InstMetaData::OperandFieldID::__N)> field_id_list_;

public:
    OperandInfo() = default;

    OperandInfo(const ElementList & elems)
    {
        for(const auto & elem : elems) {
            addElement(elem);
        }
    }

    OperandInfo(const OperandInfo&) = default;
    OperandInfo& operator=(const OperandInfo& other) = default;

    template<class ...ElementArgs>
    void addElement(ElementArgs&& ... args)
    {
        const auto elem = elems_.emplace_back(std::forward<ElementArgs>(args)...);
        ++n_opers_;
        has_implied_operand_ = has_implied_operand_ || elem.is_implied;
        bool duplicate_operand = false;
        if (elem.field_id != InstMetaData::OperandFieldID::NONE)
        {
            // allow multiple copies of the same exact operand...
            if (hasFieldID(elem.field_id))
            {
                const FieldIDInfo& finfo = field_id_list_[static_cast<OperandFieldIDUnderlyingType>(elem.field_id)];
                duplicate_operand = (finfo == elem);
                if (!duplicate_operand) [[unlikely]]
                {
                    std::ostringstream ss;
                    ss << "Attempted to add a near-duplicate operand: fid=" << InstMetaData::getFieldIDName(elem.field_id)
                       << " optype=" << InstMetaData::getOperandTypeName(elem.operand_type)
                       << " val=" << elem.field_value
                       << " sd=" << std::boolalpha << elem.is_store_data
                       << " implied=" << elem.is_implied;
                    throw std::runtime_error(ss.str());
                }
            }
            else
            {
                field_id_list_[static_cast<OperandFieldIDUnderlyingType>(elem.field_id)] =
                    {true, elem.field_value, elem.operand_type, elem.is_store_data, elem.is_implied};
            }
        }
        if ((elem.operand_type != InstMetaData::OperandTypes::NONE) && (!duplicate_operand))
        {
            ++n_types_[static_cast<OperandTypesUnderLyingType>(elem.operand_type)];
        }
    }

    const ElementList& getElements() const
    {
        return elems_;
    }

    uint32_t getNOpers() const
    {
        return n_opers_;
    }

    uint32_t getNTypes(InstMetaData::OperandTypes otype) const
    {
        if (otype == InstMetaData::OperandTypes::NONE)
        {
            throw std::invalid_argument("otype must not be NONE");
        }
        return n_types_[static_cast<OperandTypesUnderLyingType>(otype)];
    }

    bool hasImpliedOperand() const
    {
        return has_implied_operand_;
    }

    bool hasFieldID(InstMetaData::OperandFieldID fid) const
    {
        if (fid == InstMetaData::OperandFieldID::NONE) {
            return false;
        }
        return field_id_list_[static_cast<OperandFieldIDUnderlyingType>(fid)].valid;
    }

    OpcodeFieldValueType getFieldValue(InstMetaData::OperandFieldID fid) const
    {
        if (hasFieldID(fid)) {
            return field_id_list_[static_cast<OperandFieldIDUnderlyingType>(fid)].value;
        } else {
            throw OperandInfoInvalidFieldID(InstMetaData::getFieldIDName(fid));
        }
    }

    InstMetaData::OperandTypes getFieldType(InstMetaData::OperandFieldID fid) const
    {
        if (hasFieldID(fid)) {
            return field_id_list_[static_cast<OperandFieldIDUnderlyingType>(fid)].type;
        } else {
            throw OperandInfoInvalidFieldID(InstMetaData::getFieldIDName(fid));
        }
    }

    bool isStoreData(InstMetaData::OperandFieldID fid) const
    {
        if (hasFieldID(fid)) {
            return field_id_list_[static_cast<OperandFieldIDUnderlyingType>(fid)].sdata;
        } else {
            throw OperandInfoInvalidFieldID(InstMetaData::getFieldIDName(fid));
        }
    }

    bool isImplied(InstMetaData::OperandFieldID fid) const
    {
        if (hasFieldID(fid)) {
            return field_id_list_[static_cast<OperandFieldIDUnderlyingType>(fid)].implied;
        } else {
            throw OperandInfoInvalidFieldID(InstMetaData::getFieldIDName(fid));
        }
    }
};

} // namespace mavis
