#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <iomanip>

#include "mavis/Mavis.h"
#include "mavis/extension_managers/RISCVExtensionManager.hpp"

#include "Inst.h"
#include "uArchInfo.h"

using MavisType = Mavis<Instruction<uArchInfo>, uArchInfo>;

int main()
{
    mavis::extension_manager::riscv::RISCVExtensionManager extension_manager =
        mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            "rv64gcbvfdq_zicsr_zicbom", "json/riscv_isa_spec.json", "json");

    std::unique_ptr<MavisType> mavis_facade
        = std::make_unique<MavisType>(
            extension_manager.constructMavis<Instruction<uArchInfo>,
            uArchInfo>({"uarch/uarch_rv64g.json"}));

    // Open the rv64.tset file and load into a vector
    std::ifstream rv64_test("rv64.tset");
    std::vector<uint32_t> opcodes;
    std::string mnemonic, opcode;

    while(rv64_test.good())
    {
        rv64_test >> mnemonic >> opcode;
        opcodes.emplace_back(std::stoul(opcode, 0, 16));
    }

    const uint32_t num_opcodes = opcodes.size();
    assert(num_opcodes > 0);
    const uint32_t num_trials = num_opcodes * 100000;
    uint32_t curr_opcode = 0;
    auto start = std::chrono::system_clock::system_clock::now();
    for (uint32_t trial = 0; trial < num_trials; ++trial)
    {
        try {
            mavis_facade->makeInst(opcodes[curr_opcode++], 0);
        }
        catch(std::exception & ex) {
            std::cerr << "Not decodable: 0x" << std::hex << opcodes[curr_opcode-1]
                      << " What:" << ex.what() << std::endl;
            return 1;
        }
        if (curr_opcode == num_opcodes) {
            curr_opcode = 0;
        }
    }
    auto end = std::chrono::system_clock::system_clock::now();
    auto decode_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    std::cout.precision(12);

    std::cout << "Instructions Decoded: " << std::dec << num_trials << std::endl;
    std::cout << "Raw time (seconds): " << std::dec << (decode_time / 1000000.0) << std::endl;
    std::cout << "Inst Decoded/Second: " << std::dec << (num_trials / (decode_time / 1000000.0)) << std::endl;

    return 0;
}
