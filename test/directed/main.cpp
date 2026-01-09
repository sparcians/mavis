#include <boost/program_options.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <iomanip>
#include <bitset>

#include "mavis/Mavis.h"
#include "mavis/extension_managers/RISCVExtensionManager.hpp"

#include "Inst.h"
#include "uArchInfo.h"

using MavisType = Mavis<Instruction<uArchInfo>, uArchInfo>;

#ifndef HEX8
#define HEX8(val)                                                                                  \
    "0x" << std::setw(8) << std::setfill('0') << std::hex << std::right << val                     \
         << std::setfill(' ') << std::dec
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

int main(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description desc(
        "mavis_decode -- a program that details how mavis sees a given instruction");
    desc.add_options()("help,h", "Command line options")(
        "opc,o", po::value<std::vector<std::string>>(), "32-bit or 16-bit hex opcode")(
        "isa,a", po::value<std::string>(), "ISA string (example: rv32g_zce, rv64imadf_zicond)");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 0;
    }

    if (vm.count("opc") == 0 && vm.count("mnemonic") == 0)
    {
        std::cerr << "ERROR: Nothing given to decode!" << std::endl;
        std::cout << desc << "\n";
        return 255;
    }

    std::string rv_isa = "rv64";
    if (vm.count("isa"))
    {
        rv_isa = vm["isa"].as<std::string>();
    }

    mavis::extension_manager::riscv::RISCVExtensionManager extension_manager =
        mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            rv_isa, "json/riscv_isa_spec.json", "json");

    std::string uarch_file = "uarch/uarch_rv32g.json";
    if (rv_isa.starts_with("rv64"))
    {
        uarch_file = "uarch/uarch_rv64g.json";
    }

    try {
        std::unique_ptr<MavisType> mavis_facade
            = std::make_unique<MavisType>(
                extension_manager.constructMavis<Instruction<uArchInfo>, uArchInfo>({uarch_file}));

        assert(nullptr != mavis_facade);

        if (vm.count("opc"))
        {
            for (auto opcode : vm["opc"].as<std::vector<std::string>>())
            {
                auto inst = mavis_facade->makeInst(std::stol(opcode, 0, 16), 0);
                if (nullptr != inst)
                {
                    std::cout << "Dasm (" << HEX8(opcode) << "): " << inst->dasmString() << std::endl;
                    std::cout << "  Addr-Sources : "
                              << printBitSet(std::bitset<64>(inst->getSourceAddressRegs())) << ": "
                              << std::bitset<64>(inst->getSourceAddressRegs()) << std::endl;
                    std::cout << "  Data-Sources : "
                              << printBitSet(std::bitset<64>(inst->getSourceDataRegs())) << ": "
                              << std::bitset<64>(inst->getSourceDataRegs()) << std::endl;
                    std::cout << "  Int-Sources  : "
                              << printBitSet(std::bitset<64>(inst->getIntSourceRegs())) << ": "
                              << std::bitset<64>(inst->getIntSourceRegs()) << std::endl;
                    std::cout << "  Float-Sources: "
                              << printBitSet(std::bitset<64>(inst->getFloatSourceRegs())) << ": "
                              << std::bitset<64>(inst->getFloatSourceRegs()) << std::endl;
                    std::cout << "  Int-Dests    : "
                              << printBitSet(std::bitset<64>(inst->getIntDestRegs())) << ": "
                              << std::bitset<64>(inst->getIntDestRegs()) << std::endl;
                    std::cout << "  Float-Dests  : "
                              << printBitSet(std::bitset<64>(inst->getFloatDestRegs())) << ": "
                              << std::bitset<64>(inst->getFloatDestRegs()) << std::endl;
                    if (inst->hasImmediate())
                    {
                        std::cout << "  immediate    : " << HEX8(inst->getImmediate()) << std::endl;
                    }
                    std::cout << "Inst type      : ";
                    std::string comma;
                    for (uint64_t inst_type = 0;
                         inst_type < sizeof(mavis::InstMetaData::InstructionTypes) * 8;
                         ++inst_type)
                    {
                        const auto itype = static_cast<mavis::InstMetaData::InstructionTypes>((0x1ull << inst_type));
                        if (inst->isInstType(itype)) {
                            std::cout << comma << mavis::InstMetaData::getInstructionTypeName(itype);
                            comma = ", ";
                        }
                    }
                    if (comma.empty()) {
                        std::cout << "None";
                    }
                    std::cout << std::endl;
                    std::cout << "Inst tags      : ";
                    comma.clear();
                    const auto tagv = inst->getTags().getV();
                    for (auto tag : tagv)
                    {
                        std::cout << comma << tag.getV();
                        comma = ", ";
                    }
                    if (comma.empty()) {
                        std::cout << "None";
                    }
                    std::cout << std::endl;

                    std::cout << "Src List: " << std::endl;
                    for (const auto & op : inst->getSourceOpInfoList())
                    {
                        std::cout << "  Operand fid  : "
                                  << mavis::InstMetaData::getFieldIDName(op.field_id) << std::endl;
                        std::cout << "  Operand val  : " << op.field_value << std::endl;
                    }
                    std::cout << "Dest List: " << std::endl;
                    for (const auto & op : inst->getDestOpInfoList())
                    {
                        std::cout << "  Operand fid  : "
                                  << mavis::InstMetaData::getFieldIDName(op.field_id) << std::endl;
                        std::cout << "  Operand val  : " << op.field_value << std::endl;
                    }
                }
                else
                {
                    std::cerr << "ERROR: " << HEX8(opcode) << " is not decodable" << std::endl;
                }
            }
        }
    }
    catch (std::exception & ex) {
        std::cerr << "Somthing went wrong: " << ex.what() << std::endl;
        throw;
    }
    return 0;
}
