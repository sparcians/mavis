#include "mavis/RISCVExtensionManager.hpp"

int main(int argc, char* argv[])
{
    auto rv32g_man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISA("rv32gcb", "json/riscv_isa_spec.json");

    assert(rv32g_man.isEnabled("i"));

    assert(rv32g_man.isEnabled("zba"));
    assert(!rv32g_man.isEnabled("v"));

    auto rv64g_man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISA("rv64gcbv", "json/riscv_isa_spec.json");

    assert(rv64g_man.isEnabled("i"));

    assert(rv64g_man.isEnabled("zba"));
    assert(rv64g_man.isEnabled("v"));

    const auto elf_man = mavis::extension_manager::riscv::RISCVExtensionManager::fromELF("hello", "json/riscv_isa_spec.json");

    auto rv_generic_man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISASpecJSON("json/riscv_isa_spec.json");
    rv_generic_man.setISA("rv64gcbv");

    assert(rv_generic_man.isEnabled("i"));

    assert(rv_generic_man.isEnabled("zba"));
    assert(rv_generic_man.isEnabled("v"));

    rv_generic_man.setISA("rv64gcb");

    assert(rv_generic_man.isEnabled("i"));

    assert(rv_generic_man.isEnabled("zba"));
    assert(!rv_generic_man.isEnabled("v"));

    return 0;
}
