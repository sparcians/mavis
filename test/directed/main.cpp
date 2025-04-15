#include <boost/program_options.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <iomanip>
#include <bitset>

#include "mavis/Mavis.h"

#include "Inst.h"
#include "uArchInfo.h"

using MavisType = Mavis<Instruction<uArchInfo>, uArchInfo>;

#ifndef HEX8
#define HEX8(val)                                         \
    "0x" << std::setw(8) << std::setfill('0') << std::hex   \
    << std::right << val << std::setfill(' ') << std::dec
#endif

std::string printBitSet(const std::bitset<64> & bits)
{
    std::string ret;
    ret += '[';
    bool in_range = false;
    bool print_comma = false;
    int32_t v = 0;
    int32_t range_start_v = -1;
    while (v < int32_t(bits.size()))
    {
        if (bits.test(v))
        {
            // Determine if this bit is in the current range
            if (!in_range)
            {
                if (print_comma)
                {
                    ret += ',';
                }
                ret += std::to_string(v);
                print_comma = true;
                // Assume within a range
                in_range = true;
                range_start_v = v;
            }
        }
        else
        {
            if (in_range)
            {
                // close the range if more than one bit
                if (range_start_v + 1 != v)
                {
                    ret += '-';
                    ret += std::to_string(v - 1);
                }
                in_range = false;
            }
        }
        ++v;
    }
    ret += ']';
    return ret;
}

int main(int argc, char **argv)
{
    namespace po = boost::program_options;
    po::options_description desc("mavis_decode -- a program that details how mavis sees a given instruction");
    desc.add_options()
        ("help,h",  "Command line options")
        ("opc,o",   po::value<std::vector<std::string>>(), "32-bit or 16-bit hex opcode")
        ("isa,a",   po::value<std::string>(), "rv32 or rv64 (all inclusive)")
        ("zclsd,z", "use rv32_zclsd extension or not (default is not)");
    //("mnemonic,m", po::value<std::vector<std::string>>(), "Mnemonic to look up");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(vm.count("help"))
    {
        std::cout << desc << "\n";
        return 0;
    }

    if(vm.count("opc") == 0 && vm.count("mnemonic") == 0)
    {
        std::cerr << "ERROR: Nothing given to decode!" << std::endl;
        std::cout << desc << "\n";
        return 255;
    }

    std::string rv_isa = "rv64";
    if(vm.count("isa"))
    {
        rv_isa = vm["isa"].as<std::string>();
    }

    std::unique_ptr<MavisType> mavis_facade;
    if(rv_isa == "rv32")
    {
        mavis::FileNameListType isa_files = {
                    "json/isa_rv32i.json",        // included in "g" spec
                    "json/isa_rv32f.json",        // included in "g" spec
                    "json/isa_rv32m.json",        // included in "g" spec
                    "json/isa_rv32zmmul.json",    // included in "g" spec
                    "json/isa_rv32zaamo.json",    // included in "g" spec
                    "json/isa_rv32zalrsc.json",   // included in "g" spec
                    "json/isa_rv32d.json",        // included in "g" spec
                    "json/isa_rv32zicsr.json",    // included in "g" spec
                    "json/isa_rv32zifencei.json", // included in "g" spec
                    "json/isa_rv32q.json",
                    "json/isa_rv32zfa.json",
                    "json/isa_rv32zfa_d.json",
                    "json/isa_rv32zfa_d_addons.json",
                    "json/isa_rv32zfa_q.json",
                    "json/isa_rv32zfa_h.json",
                    "json/isa_rv32zca.json",
                    "json/isa_rv32zcd.json",
                    "json/isa_rv32zfh.json",
                    "json/isa_rv32zfhmin.json",
                    "json/isa_rv32zfhmin_d.json",
                    "json/isa_rv32zawrs.json",
                    "json/isa_rv32zilsd.json",
                    "json/isa_rv32zacas.json",
                    "json/isa_rv32zabha.json"};

        // Zclsd has overlapping encodings with Zcf, so only one can be used at a time
        if (vm.count("zclsd"))
        {
            isa_files.emplace_back("json/isa_rv32zclsd.json");
        }
        else
        {
            isa_files.emplace_back("json/isa_rv32zcf.json");
        }
        mavis_facade.reset(new MavisType(isa_files, {"uarch/uarch_rv32g.json"}));
    }
    else if(rv_isa == "rv64")
    {
        mavis_facade.reset(new MavisType({
                    "json/isa_rv64i.json",        // included in "g" spec
                    "json/isa_rv64f.json",        // included in "g" spec
                    "json/isa_rv64m.json",        // included in "g" spec
                    "json/isa_rv64zmmul.json",    // included in "g" spec
                    "json/isa_rv64zaamo.json",    // included in "g" spec
                    "json/isa_rv64zalrsc.json",   // included in "g" spec
                    "json/isa_rv64d.json",        // included in "g" spec
                    "json/isa_rv64zicsr.json",    // included in "g" spec
                    "json/isa_rv64zifencei.json", // included in "g" spec
                    "json/isa_rv64zca.json",
                    "json/isa_rv64zcd.json",
                    "json/isa_rv64q.json",
                    "json/isa_rv64q_addons.json",
                    "json/isa_rv64h.json",
                    "json/isa_rv64zfa.json",
                    "json/isa_rv64zfa_d.json",
                    "json/isa_rv64zfa_q.json",
                    "json/isa_rv64zfa_q_addons.json",
                    "json/isa_rv64zfa_h.json",
                    "json/isa_rv64v.json",
                    "json/isa_rv64vf.json",
                    "json/isa_rv64zvbb.json",
                    "json/isa_rv64zvbc.json",
                    "json/isa_rv64zvkned.json",
                    "json/isa_rv64zvkg.json",
                    "json/isa_rv64zvknh.json",
                    "json/isa_rv64zvksed.json",
                    "json/isa_rv64zvksh.json",
                    "json/isa_rv64zfh.json",
                    "json/isa_rv64zfh_addons.json",
                    "json/isa_rv64zfhmin.json",
                    "json/isa_rv64zfhmin_d.json",
                    "json/isa_rv64zicbom.json",
                    "json/isa_rv64zicbop.json",
                    "json/isa_rv64zicboz.json",
                    "json/isa_rv64zihintntl.json",
                    "json/isa_rv64zihintpause.json",
                    "json/isa_rv64zicond.json",
                    "json/isa_rv64zawrs.json",
                    "json/isa_rv64zfbfmin.json",
                    "json/isa_rv64zvfbfwma.json"},
                {"uarch/uarch_rv64g.json"}));
    }
    else
    {
        std::cerr << "ERROR: rv_isa expected to be either rv32 or rv64" << std::endl;
        std::cerr << desc << std::endl;
        return 255;
    }
    assert(nullptr != mavis_facade);

    if(vm.count("opc"))
    {
        for(auto opcode : vm["opc"].as<std::vector<std::string>>())
        {
            auto inst = mavis_facade->makeInst(std::stol(opcode, 0, 16), 0);
            if(nullptr != inst)
            {
                std::cout << "Dasm (" << HEX8(opcode) << "): " << inst->dasmString() << std::endl;
                std::cout << "  Addr-Sources : "
                          << printBitSet(std::bitset<64>(inst->getSourceAddressRegs())) << ": "
                          << std::bitset<64>(inst->getSourceAddressRegs()) << std::endl;
                std::cout << "  Data-Sources : "
                          << printBitSet(std::bitset<64>(inst->getSourceDataRegs()))    << ": "
                          << std::bitset<64>(inst->getSourceDataRegs())    << std::endl;
                std::cout << "  Int-Sources  : "
                          << printBitSet(std::bitset<64>(inst->getIntSourceRegs()))     << ": "
                          << std::bitset<64>(inst->getIntSourceRegs())     << std::endl;
                std::cout << "  Float-Sources: "
                          << printBitSet(std::bitset<64>(inst->getFloatSourceRegs()))   << ": "
                          << std::bitset<64>(inst->getFloatSourceRegs())   << std::endl;
                std::cout << "  Int-Dests    : "
                          << printBitSet(std::bitset<64>(inst->getIntDestRegs()))       << ": "
                          << std::bitset<64>(inst->getIntDestRegs())       << std::endl;
                std::cout << "  Float-Dests  : "
                          << printBitSet(std::bitset<64>(inst->getFloatDestRegs()))     << ": "
                          << std::bitset<64>(inst->getFloatDestRegs())     << std::endl;
                if(inst->hasImmediate())
                {
                    std::cout << "  immediate    : " << HEX8(inst->getImmediate()) << std::endl;
                }
                std::cout << "Src List: " << std::endl;
                for(const auto & op : inst->getSourceOpInfoList())
                {
                    std::cout << "  Operand fid  : " << mavis::InstMetaData::getFieldIDName(op.field_id) << std::endl;
                    std::cout << "  Operand val  : " << op.field_value << std::endl;
                }
                std::cout << "Dest List: " << std::endl;
                for(const auto & op : inst->getDestOpInfoList())
                {
                    std::cout << "  Operand fid  : " << mavis::InstMetaData::getFieldIDName(op.field_id) << std::endl;
                    std::cout << "  Operand val  : " << op.field_value << std::endl;
                }
            }
            else
            {
                std::cerr << "ERROR: " << HEX8(opcode) << " is not decodable" << std::endl;
            }
        }
    }

    // Not really handy...
    // if(vm.count("mnemonic")) {
    //     for(auto mnemonic : vm["mnemonic"].as<std::vector<std::string>>())
    //     {
    //         mavis::ExtractorDirectInfo ex_data(mnemonic,
    //                                            MavisType::RegListType(),
    //                                            MavisType::RegListType());
    //         auto inst = mavis_facade->makeInstDirectly(ex_data, 0);
    //         if(nullptr != inst) {
    //             std::cout << inst->dasmString() << std::endl;
    //         }
    //         else {
    //             std::cerr << "ERROR: " << mnemonic << " is not decodable" << std::endl;
    //         }
    //     }
    // }

    return 0;
}
