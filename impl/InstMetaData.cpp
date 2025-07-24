
#include <string>
#include <iostream>
#include <boost/bimap.hpp>

#include "mavis/InstMetaData.h"

namespace mavis
{

    template <class BmType>
    BmType make_bimap(std::initializer_list<typename BmType::value_type> list)
    {
        return BmType(list.begin(), list.end());
    }

    using BmInstMapType = boost::bimap<std::string, InstMetaData::InstructionTypes>;
    static const BmInstMapType tmap = make_bimap<BmInstMapType>({
        {"int",               InstMetaData::InstructionTypes::INT              },
        {"float",             InstMetaData::InstructionTypes::FLOAT            },
        {"arith",             InstMetaData::InstructionTypes::ARITH            },
        {"mul",               InstMetaData::InstructionTypes::MULTIPLY         },
        {"div",               InstMetaData::InstructionTypes::DIVIDE           },
        {"branch",            InstMetaData::InstructionTypes::BRANCH           },
        {"pc",                InstMetaData::InstructionTypes::PC               },
        {"cond",              InstMetaData::InstructionTypes::CONDITIONAL      },
        {"jal",               InstMetaData::InstructionTypes::JAL              },
        {"jalr",              InstMetaData::InstructionTypes::JALR             },
        {"load",              InstMetaData::InstructionTypes::LOAD             },
        {"store",             InstMetaData::InstructionTypes::STORE            },
        {"mac",               InstMetaData::InstructionTypes::MAC              },
        {"sqrt",              InstMetaData::InstructionTypes::SQRT             },
        {"convert",           InstMetaData::InstructionTypes::CONVERT          },
        {"compare",           InstMetaData::InstructionTypes::COMPARE          },
        {"move",              InstMetaData::InstructionTypes::MOVE             },
        {"classify",          InstMetaData::InstructionTypes::CLASSIFY         },
        {"vector",            InstMetaData::InstructionTypes::VECTOR           },
        {"maskable",          InstMetaData::InstructionTypes::MASKABLE         },
        {"unit_stride",       InstMetaData::InstructionTypes::UNIT_STRIDE      },
        {"stride",            InstMetaData::InstructionTypes::STRIDE           },
        {"ordered_indexed",   InstMetaData::InstructionTypes::ORDERED_INDEXED  },
        {"unordered_indexed", InstMetaData::InstructionTypes::UNORDERED_INDEXED},
        {"segment",           InstMetaData::InstructionTypes::SEGMENT          },
        {"faultfirst",        InstMetaData::InstructionTypes::FAULTFIRST       },
        {"whole",             InstMetaData::InstructionTypes::WHOLE            },
        {"mask",              InstMetaData::InstructionTypes::MASK             },
        {"widening",          InstMetaData::InstructionTypes::WIDENING         },
        {"hypervisor",        InstMetaData::InstructionTypes::HYPERVISOR       },
        {"crypto",            InstMetaData::InstructionTypes::CRYPTO           },
        {"prefetch",          InstMetaData::InstructionTypes::PREFETCH         },
        {"ntl",               InstMetaData::InstructionTypes::NTL              },
        {"hint",              InstMetaData::InstructionTypes::HINT             },
        {"cache",             InstMetaData::InstructionTypes::CACHE            },
        {"atomic",            InstMetaData::InstructionTypes::ATOMIC           },
        {"fence",             InstMetaData::InstructionTypes::FENCE            },
        {"system",            InstMetaData::InstructionTypes::SYSTEM           },
        {"csr",               InstMetaData::InstructionTypes::CSR              }
    });

    using ISAExtenMapType = boost::bimap<std::string, InstMetaData::ISAExtensionIndex>;
    static const ISAExtenMapType isamap = make_bimap<ISAExtenMapType>({
        {"A", InstMetaData::ISAExtensionIndex::A},
        {"B", InstMetaData::ISAExtensionIndex::B},
        {"C", InstMetaData::ISAExtensionIndex::C},
        {"D", InstMetaData::ISAExtensionIndex::D},
        {"F", InstMetaData::ISAExtensionIndex::F},
        {"G", InstMetaData::ISAExtensionIndex::G},
        {"H", InstMetaData::ISAExtensionIndex::H},
        {"I", InstMetaData::ISAExtensionIndex::I},
        {"M", InstMetaData::ISAExtensionIndex::M},
        {"Q", InstMetaData::ISAExtensionIndex::Q},
        {"V", InstMetaData::ISAExtensionIndex::V}
    });

    using OpFieldIDMapType = boost::bimap<std::string, InstMetaData::OperandFieldID>;
    static const OpFieldIDMapType ofimap = make_bimap<OpFieldIDMapType>({
        {"rs1",        InstMetaData::OperandFieldID::RS1       },
        {"rs2",        InstMetaData::OperandFieldID::RS2       },
        {"rs3",        InstMetaData::OperandFieldID::RS3       },
        {"rs4",        InstMetaData::OperandFieldID::RS4       },
        {"rs_max",     InstMetaData::OperandFieldID::RS_MAX    },
        {"fused_sd_0", InstMetaData::OperandFieldID::FUSED_SD_0},
        {"fused_sd_1", InstMetaData::OperandFieldID::FUSED_SD_1},
        {"temp_rs1",   InstMetaData::OperandFieldID::TEMP_RS1  },
        {"temp_rs2",   InstMetaData::OperandFieldID::TEMP_RS2  },
        {"temp_rs3",   InstMetaData::OperandFieldID::TEMP_RS3  },
        {"push_rs1",   InstMetaData::OperandFieldID::PUSH_RS1  },
        {"push_rs2",   InstMetaData::OperandFieldID::PUSH_RS2  },
        {"push_rs3",   InstMetaData::OperandFieldID::PUSH_RS3  },
        {"push_rs4",   InstMetaData::OperandFieldID::PUSH_RS4  },
        {"push_rs5",   InstMetaData::OperandFieldID::PUSH_RS5  },
        {"push_rs6",   InstMetaData::OperandFieldID::PUSH_RS6  },
        {"push_rs7",   InstMetaData::OperandFieldID::PUSH_RS7  },
        {"push_rs8",   InstMetaData::OperandFieldID::PUSH_RS8  },
        {"push_rs9",   InstMetaData::OperandFieldID::PUSH_RS9  },
        {"push_rs10",  InstMetaData::OperandFieldID::PUSH_RS10 },
        {"push_rs11",  InstMetaData::OperandFieldID::PUSH_RS11 },
        {"push_rs12",  InstMetaData::OperandFieldID::PUSH_RS12 },
        {"push_rs13",  InstMetaData::OperandFieldID::PUSH_RS13 },
        {"rd",         InstMetaData::OperandFieldID::RD        },
        {"rd1",        InstMetaData::OperandFieldID::RD1       },
        {"rd2",        InstMetaData::OperandFieldID::RD2       },
        {"rd_max",     InstMetaData::OperandFieldID::RD_MAX    },
        {"fused_rd_0", InstMetaData::OperandFieldID::FUSED_RD_0},
        {"fused_rd_1", InstMetaData::OperandFieldID::FUSED_RD_1},
        {"temp_rd",    InstMetaData::OperandFieldID::TEMP_RD   },
        {"temp_rd2",   InstMetaData::OperandFieldID::TEMP_RD2  },
        {"pop_rd1",    InstMetaData::OperandFieldID::POP_RD1   },
        {"pop_rd2",    InstMetaData::OperandFieldID::POP_RD2   },
        {"pop_rd3",    InstMetaData::OperandFieldID::POP_RD3   },
        {"pop_rd4",    InstMetaData::OperandFieldID::POP_RD4   },
        {"pop_rd5",    InstMetaData::OperandFieldID::POP_RD5   },
        {"pop_rd6",    InstMetaData::OperandFieldID::POP_RD6   },
        {"pop_rd7",    InstMetaData::OperandFieldID::POP_RD7   },
        {"pop_rd8",    InstMetaData::OperandFieldID::POP_RD8   },
        {"pop_rd9",    InstMetaData::OperandFieldID::POP_RD9   },
        {"pop_rd10",   InstMetaData::OperandFieldID::POP_RD10  },
        {"pop_rd11",   InstMetaData::OperandFieldID::POP_RD11  },
        {"pop_rd12",   InstMetaData::OperandFieldID::POP_RD12  },
        {"pop_rd13",   InstMetaData::OperandFieldID::POP_RD13  },
    });

    /**
     * Construct from JSON information pertaining to extraction
     */
    InstMetaData::InstMetaData(const json & inst, bool compressed, const MatchSet<Tag> & tags) :
        compressed_(compressed),
        tags_(tags)
    {

        if (ofimap.size() != size_t(OperandFieldID::__N))
        {
            std::cerr << "MAVIS: ofimap size is not the same as the OperandFieldID enum class"
                      << "\n\tUpdate the map in " << __FILE__ << std::endl;
            exit(-1);
        }

        if (isamap.size() != size_t(ISAExtensionIndex::__N))
        {
            std::cerr << "MAVIS: isamap size is not the same as the ISAExtensionIndex enum class"
                      << "\n\tUpdate the map in " << __FILE__ << std::endl;
            exit(-1);
        }

        oper_type_.fill(InstMetaData::OperandTypes::NONE);

        // Type
        parseTypeStanza_(inst);

        // Data size
        parseDataSizeStanza_(inst);

        // Word operand types
        if (const auto it = inst.find("w-oper"); it != inst.end())
        {
            const auto & it_value = it->value();
            if (it_value == "all")
            {
                setAllOperandsType_(OperandTypes::WORD);
            }
            else
            {
                FieldNameListType flist;
                flist = boost::json::value_to<FieldNameListType>(it_value);
                setOperandsType_(flist, OperandTypes::WORD);
            }
        }

        // Long operand types
        if (const auto it = inst.find("l-oper"); it != inst.end())
        {
            const auto & it_value = it->value();
            if (it_value == "all")
            {
                setAllOperandsType_(OperandTypes::LONG);
            }
            else
            {
                FieldNameListType flist;
                flist = boost::json::value_to<FieldNameListType>(it_value);
                setOperandsType_(flist, OperandTypes::LONG);
            }
        }

        // Single operand types
        if (const auto it = inst.find("s-oper"); it != inst.end())
        {
            const auto & it_value = it->value();
            if (it_value == "all")
            {
                setAllOperandsType_(OperandTypes::SINGLE);
            }
            else
            {
                FieldNameListType flist;
                flist = boost::json::value_to<FieldNameListType>(it_value);
                setOperandsType_(flist, OperandTypes::SINGLE);
            }
        }

        // Double operand types
        if (const auto it = inst.find("d-oper"); it != inst.end())
        {
            const auto & it_value = it->value();
            if (it_value == "all")
            {
                setAllOperandsType_(OperandTypes::DOUBLE);
            }
            else
            {
                FieldNameListType flist;
                flist = boost::json::value_to<FieldNameListType>(it_value);
                setOperandsType_(flist, OperandTypes::DOUBLE);
            }
        }

        // Quad operand types
        if (const auto it = inst.find("q-oper"); it != inst.end())
        {
            const auto & it_value = it->value();
            if (it_value == "all")
            {
                setAllOperandsType_(OperandTypes::QUAD);
            }
            else
            {
                FieldNameListType flist;
                flist = boost::json::value_to<FieldNameListType>(it_value);
                setOperandsType_(flist, OperandTypes::QUAD);
            }
        }

        // Vector operand types
        if (const auto it = inst.find("v-oper"); it != inst.end())
        {
            const auto & it_value = it->value();
            if (it_value == "all")
            {
                setAllOperandsType_(OperandTypes::VECTOR);
            }
            else
            {
                FieldNameListType flist;
                flist = boost::json::value_to<FieldNameListType>(it_value);
                setOperandsType_(flist, OperandTypes::VECTOR);
            }
        }

        // Try to find ISA extensions of the form 'wX' where 'w' is a an optional
        // "width" (e.g. 32, 64, etc.) and 'X' is an extension letter
        if (const auto it = inst.find("isa"); it != inst.end())
        {
            ISAExtListType ilist = boost::json::value_to<ISAExtListType>(it->value());
            if (!ilist.empty())
            {
                std::smatch matches;
                for (const auto & s : ilist)
                {
                    // matches[0] is the entire string matched,
                    // matches[1] is the capture group for the optional width (emtpy if not
                    // provided) matches[2] is the capture group for the ISA extension letter
                    if (std::regex_search(s, matches, isa_ext_pattern_))
                    {
                        const auto itr = isamap.left.find(matches[2].str());
                        if (itr != isamap.left.end())
                        {
                            isa_ext_ |=
                                (1ull << static_cast<std::underlying_type_t<ISAExtensionIndex>>(
                                     itr->second));
                        }
                        else
                        {
                            // Invalid ISA extension letter
                            throw BuildErrorInvalidISAExtension(
                                boost::json::value_to<std::string>(inst.at("mnemonic")), s,
                                matches[2].str());
                        }
                        if (matches[1].length() != 0)
                        {
                            const uint32_t n = std::strtoull(matches[1].str().c_str(), nullptr, 0);
                            if ((n == 0) || ((n & (n - 1)) != 0))
                            {
                                // Not a non-zero power of 2
                                throw BuildErrorInvalidISAWidth(
                                    boost::json::value_to<std::string>(inst.at("mnemonic")), s, n);
                            }
                            setISAWidth(itr->second, n);
                        }
                    }
                    else
                    {
                        // Malformed ISA extension string
                        throw BuildErrorMalformedISAExtension(
                            boost::json::value_to<std::string>(inst.at("mnemonic")), s);
                    }
                }
            }
        }
    }

    void InstMetaData::parseTypeStanza_(const json & inst)
    {
        // Merge type information from the overlay instruction and the base
        if (const auto it = inst.find("type"); it != inst.end())
        {
            const FieldNameListType tlist = boost::json::value_to<FieldNameListType>(it->value());
            for (const auto & t : tlist)
            {
                const auto itr = tmap.left.find(t);
                if (itr == tmap.left.end())
                {
                    throw BuildErrorUnknownType(
                        boost::json::value_to<std::string>(inst.at("mnemonic")), t);
                }
                inst_types_ |= static_cast<std::underlying_type_t<InstructionTypes>>(itr->second);
            }
        }
    }

    void InstMetaData::parseDataSizeStanza_(const json & inst)
    {
        // Merge data size information from the overlay instruction and the base
        if (const auto it = inst.find("data"); it != inst.end())
        {
            data_size_ = boost::json::value_to<uint32_t>(it->value());
            // Check positive, power-of-2 or zero
            if ((data_size_ & (data_size_ - 1)) || (int32_t(data_size_) < 0))
            {
                throw BuildErrorInvalidDataSize(
                    boost::json::value_to<std::string>(inst.at("mnemonic")), data_size_);
            }
        }
    }

    std::underlying_type_t<InstMetaData::OperandFieldID>
    InstMetaData::getFieldIndex_(const std::string & fname)
    {
        const auto itr = ofimap.left.find(fname);
        if (itr == ofimap.left.end())
        {
            throw BuildErrorUnknownFormField("InstMetaData", fname);
        }
        return static_cast<std::underlying_type_t<OperandFieldID>>(itr->second);
    }

    const std::string & InstMetaData::getInstructionTypeName(const InstructionTypes & inst_type)
    {
        return tmap.right.at(inst_type);
    }

    const std::string & InstMetaData::getFieldIDName(OperandFieldID fid)
    {
        return ofimap.right.at(fid);
    }

    InstMetaData::OperandFieldID InstMetaData::getFieldID(const std::string & fname)
    {
        const auto itr = ofimap.left.find(fname);
        if (itr == ofimap.left.end())
        {
            return OperandFieldID::NONE;
        }
        return itr->second;
    }

    // InstMetaData types
    const std::regex InstMetaData::isa_ext_pattern_ =
        std::regex("([1-9][0-9]*?)?([A-Z])", std::regex::optimize);

} // namespace mavis
