#include "mavis/extension_managers/RISCVExtensionManager.hpp"

template<typename ExpectedExceptionType, bool is_elf = false>
void testFailingISAString(const std::string& isa)
{
    bool was_bad_isa_str = false;
    try
    {
        if constexpr(is_elf)
        {
            const auto rv_bad_isa_str = mavis::extension_manager::riscv::RISCVExtensionManager::fromELF(isa, "json/riscv_isa_spec.json");
        }
        else
        {
            const auto rv_bad_isa_str = mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(isa, "json/riscv_isa_spec.json");
        }
    }
    catch(const ExpectedExceptionType&)
    {
        was_bad_isa_str = true;
    }
    assert(was_bad_isa_str);
}

int main(int argc, char* argv[])
{
    {
        const auto rv32g_man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISA("rv32gcb", "json/riscv_isa_spec.json");

        assert(rv32g_man.isEnabled("i"));

        assert(rv32g_man.isEnabled("zba"));
        assert(!rv32g_man.isEnabled("v"));
    }

    {
        const auto rv64g_man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISA("rv64gcbv", "json/riscv_isa_spec.json");

        assert(rv64g_man.isEnabled("i"));

        assert(rv64g_man.isEnabled("zba"));
        assert(rv64g_man.isEnabled("v"));
    }

    {
        // Test getting a string from an ELF
        const auto elf_man = mavis::extension_manager::riscv::RISCVExtensionManager::fromELF("hello", "json/riscv_isa_spec.json");
    }

    {
        // Test reusing the same object with a new ISA string
        auto rv_generic_man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISASpecJSON("json/riscv_isa_spec.json");
        rv_generic_man.setISA("rv64gcbv");

        assert(rv_generic_man.getXLEN() == 64);
        assert(rv_generic_man.isEnabled("i"));

        assert(rv_generic_man.isEnabled("zba"));
        assert(rv_generic_man.isEnabled("v"));

        rv_generic_man.setISA("rv32gcb");

        assert(rv_generic_man.getXLEN() == 32);
        assert(rv_generic_man.isEnabled("i"));

        assert(rv_generic_man.isEnabled("zba"));
        assert(!rv_generic_man.isEnabled("v"));
    }

    testFailingISAString<mavis::extension_manager::InvalidISAStringException>("rv16gc");
    testFailingISAString<mavis::extension_manager::MissingRequiredExtensionException>("rv64i_zcd");
    testFailingISAString<mavis::extension_manager::ConflictingExtensionException>("rv32gc_zca_zcf_zclsd");
    testFailingISAString<mavis::extension_manager::riscv::UnknownRISCVExtensionException>("rv64gc_zca_zclsd");
    testFailingISAString<mavis::extension_manager::riscv::InvalidBaseExtensionException>("rv32x");
    testFailingISAString<mavis::extension_manager::riscv::ISANotFoundInELFException, true>("hello_stripped");

    return 0;
}
