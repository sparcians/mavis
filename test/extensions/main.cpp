#include "mavis/ExtensionManager.hpp"

int main(int argc, char* argv[])
{
    ExtensionManager rv32g_man("rv32gcb", "json/riscv_isa_spec.json");

    assert(rv32g_man.isEnabled("i"));

    assert(rv32g_man.isEnabled("zba"));
    assert(!rv32g_man.isEnabled("v"));

    ExtensionManager rv64g_man("rv64gcbv", "json/riscv_isa_spec.json");

    assert(rv64g_man.isEnabled("i"));

    assert(rv64g_man.isEnabled("zba"));
    assert(rv64g_man.isEnabled("v"));

    const auto elf_man = ExtensionManager::fromELF("hello", "json/riscv_isa_spec.json");

    return 0;
}
