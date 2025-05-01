#pragma once

#include <boost/json.hpp>
#include "DecoderExceptions.h"
#include <vector>
#include "InstMetaData.h"
#include "Extractor.h"

// FormGeneric class (SPARTA-954)
//
// we will need to implement the operand and special field "declarations" for pseudo-insts
// in their JSON ISA stanzas. E.g.
//
// { pseudo: "mnemonic",
//   ...
//   sources: ["rs1", "rs2", ...],   // NOTE: address sources, then data sources for stores
//   dests: ["rd", ...],
//   specials: ["vm", ...],
//   ...
// }
//
// The operands and specials stanzas will provide the operand and special field "form"
// information needed by the meta data registry to associate operand types
// (i.e. the "l-oper, s-oper, etc." stanzas).
//
// The order of the id's given in the stanzas is important, and ExtractorDirectInfo
// register and field value list elements will be provided in that same order

namespace mavis
{

class FormGeneric
{
public:
    typedef std::shared_ptr<FormGeneric> PtrType;
    static constexpr uint32_t INVALID_LIST_POS = -1;

private:
    using json = boost::json::object;
    typedef std::vector<std::string> OperandNameListType;
    typedef std::vector<std::string> SpecialFieldNameListType;

    struct OperandFormElement {
        InstMetaData::OperandFieldID    oid;
        InstMetaData::OperandTypes      otype;
    };
    typedef std::vector<OperandFormElement>     OperandFormList;

    class UnknownFieldID : public BaseException
    {
    public:
        UnknownFieldID(const std::string &otype, const std::string &field_name) :
            BaseException()
        {
            std::stringstream ss;
            ss << otype << " operand ID '" << field_name << "': "
               << "is not known to the decoder (InstMetaData ID lookup in FormGeneric object)";
            why_ = ss.str();
        }
    };

    class UnknownSpecialFieldID : public BaseException
    {
    public:
        UnknownSpecialFieldID(const std::string &field_name) :
            BaseException()
        {
            std::stringstream ss;
            ss << "Special field ID '" << field_name << "': "
               << "is not known to the decoder (ExtractorIF::SpecialFieldMap ID lookup in FormGeneric object)";
            why_ = ss.str();
        }
    };

    class UnsupportedOperandInfoID : public BaseException
    {
    public:
        UnsupportedOperandInfoID(const std::string &id_name, uint32_t pos) :
            BaseException()
        {
            std::stringstream ss;
            ss << "Unsupported OperandInfo element ID '" << id_name << "' at list position " << pos << ": "
               << "FormGeneric object expects OperandTypes::NONE";
            why_ = ss.str();
        }
    };

public:
    FormGeneric(const json& inst, const InstMetaData::PtrType& meta)
    {
        for (auto& i : spec_indices_) {
            i = INVALID_LIST_POS;
        }

        // NOTE: For stores, the list of ID's should put address sources
        // first, then data sources. This is to match the ordering of address
        // then data sources in ExtractorDirectInfo_Stores::getSourceOperandInfo
        if (const auto it = inst.find("sources"); it != inst.end()) {
            OperandNameListType olist;
            olist = boost::json::value_to<OperandNameListType>(it->value());

            for (const auto& oname : olist) {
                InstMetaData::OperandFieldID fid = meta->getFieldID(oname);
                if (fid == InstMetaData::OperandFieldID::NONE) {
                    throw UnknownFieldID("Source", oname);
                }

                InstMetaData::OperandTypes otype = meta->getOperandType(fid);
                src_oper_list_.push_back({fid, otype});
            }
        }

        if (const auto it = inst.find("dests"); it != inst.end()) {
            OperandNameListType olist;
            olist = boost::json::value_to<OperandNameListType>(it->value());

            for (const auto& oname : olist) {
                InstMetaData::OperandFieldID fid = meta->getFieldID(oname);
                if (fid == InstMetaData::OperandFieldID::NONE) {
                    throw UnknownFieldID("Destination", oname);
                }

                InstMetaData::OperandTypes otype = meta->getOperandType(fid);
                dest_oper_list_.push_back({fid, otype});
            }
        }

        // Parse special fields list
        if (const auto it = inst.find("specials"); it != inst.end()) {
            OperandNameListType slist;
            slist = boost::json::value_to<SpecialFieldNameListType>(it->value());

            uint32_t pos = 0;
            for (const auto& sname : slist) {
                const auto itr = ExtractorIF::SpecialFieldMap.find(sname);
                if (itr == ExtractorIF::SpecialFieldMap.end()) {
                    throw UnknownSpecialFieldID(sname);
                } else {
                    spec_indices_[static_cast<std::underlying_type_t<ExtractorIF::SpecialField>>(itr->second)] = pos;
                }
                ++pos;
            }
        }
    }

    FormGeneric(const FormGeneric&) = default;

    OperandInfo fixupOISources(const OperandInfo& oi) const
    {
        // Tempted to use std::move() here, but RVO is required in
        // c++17, and shouldn't be needed
        return fixupOIList_(oi, src_oper_list_);
    }

    OperandInfo fixupOIDests(const OperandInfo& oi) const
    {
        // Tempted to use std::move() here, but RVO is required in
        // c++17, and shouldn't be needed
        return fixupOIList_(oi, dest_oper_list_);
    }

    uint32_t getSpecialFieldIndex(ExtractorIF::SpecialField sid) const
    {
        return spec_indices_[static_cast<std::underlying_type_t<ExtractorIF::SpecialField>>(sid)];
    }

private:
    OperandFormList    src_oper_list_;       // (also address sources for stores)
    OperandFormList    dest_oper_list_;
    std::array<uint32_t, static_cast<std::underlying_type_t<ExtractorIF::SpecialField>>(ExtractorIF::SpecialField::__N)> spec_indices_;

private:
    /**
     * @brief fixupOIList_: assign operand ID and type information from this form
     * to the elements of a given OperandInfo list. The approach we take is intended
     * for the lists produced by ExtractorDirectInfoIF::get*OperandInfo() -- where
     * the list provides the operand value, operand ID of NONE, and a "default"
     * operand type from InstMetaData. Use with lists from other types of extractors
     * may need some additional coding.
     *
     * The information we supply from the form here is provided by the user's
     * JSON "sources", "sdatas", and "dests" stanzas. If there are not enough
     * user-supplied ID's for the given list, we just stop processing (leaving the
     * un-assigned ID's as OperandTypes::NONE with default type)
     *
     * @param oi
     * @param flist
     * @return
     */
    OperandInfo fixupOIList_(const OperandInfo& oi, const OperandFormList& flist) const
    {
        OperandInfo::ElementList oil = oi.getElements();
        auto flist_itr = flist.begin();
        for (auto& elem : oil) {
            if (flist_itr == flist.end()) {
                break;
            }
            if (elem.field_id == InstMetaData::OperandFieldID::NONE) {
                elem.field_id = flist_itr->oid;
                elem.operand_type = flist_itr->otype;
            }
            ++flist_itr;
        }
        return oil;
    }
};

} // namespace mavis
