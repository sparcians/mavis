#include "mavis/extension_managers/RISCVExtensionManager.hpp"

template <typename ExpectedExceptionType, typename Callback>
void testException(Callback && callback)
{
    bool saw_exception = false;
    try
    {
        callback();
    }
    catch (const ExpectedExceptionType &)
    {
        saw_exception = true;
    }
    assert(saw_exception);
}

template <bool is_elf>
mavis::extension_manager::riscv::RISCVExtensionManager testISA(const std::string & isa)
{
    if constexpr (is_elf)
    {
        return mavis::extension_manager::riscv::RISCVExtensionManager::fromELF(
            isa, "json/riscv_isa_spec.json", "json");
    }
    else
    {
        return mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            isa, "json/riscv_isa_spec.json", "json");
    }
}

template <typename ExpectedExceptionType, bool is_elf = false>
void testFailingISAString(const std::string & isa)
{
    testException<ExpectedExceptionType>([&isa]() { testISA<is_elf>(isa); });
}

int main(int argc, char* argv[])
{
    {
        const auto rv32g_man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            "rv32gcb", "json/riscv_isa_spec.json", "json");

        assert(rv32g_man.isEnabled("i"));

        assert(rv32g_man.isEnabled("zba"));
        assert(!rv32g_man.isEnabled("v"));
    }

    {
        const auto rv64g_man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            "rv64gcbv", "json/riscv_isa_spec.json", "json");

        assert(rv64g_man.isEnabled("i"));

        assert(rv64g_man.isEnabled("zba"));
        assert(rv64g_man.isEnabled("v"));
    }

    {
        // Test getting a string from an ELF
        const auto elf_man = mavis::extension_manager::riscv::RISCVExtensionManager::fromELF(
            "hello", "json/riscv_isa_spec.json", "json");
    }

    {
        // Test reusing the same object with a new ISA string
        auto rv_generic_man =
            mavis::extension_manager::riscv::RISCVExtensionManager::fromISASpecJSON(
                "json/riscv_isa_spec.json", "json");
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
    testFailingISAString<mavis::extension_manager::ConflictingExtensionException>(
        "rv32gc_zca_zcf_zclsd");
    testFailingISAString<mavis::extension_manager::riscv::UnknownRISCVExtensionException>(
        "rv64gc_zca_zclsd");
    testFailingISAString<mavis::extension_manager::riscv::InvalidBaseExtensionException>("rv32x");
    testFailingISAString<mavis::extension_manager::riscv::ISANotFoundInELFException, true>(
        "hello_stripped");

    {
        // Test blocking an extension
        auto man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISASpecJSON(
            "json/riscv_isa_spec.json", "json");
        man.blockExtension("i");
        testException<mavis::extension_manager::ExtensionNotAllowedException>(
            [&man]() { man.setISA("rv64gcbv"); });
    }

    {
        // Test blocking a meta extension
        auto man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISASpecJSON(
            "json/riscv_isa_spec.json", "json");
        man.blockExtension("g");
        testException<mavis::extension_manager::ExtensionNotAllowedException>(
            [&man]() { man.setISA("rv64i"); });
    }

    {
        // Test extension allowlist
        auto man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISASpecJSON(
            "json/riscv_isa_spec.json", "json");
        man.allowExtension("g");
        man.allowExtension("c");
        man.allowExtension("b");
        man.setISA("rv64gcb");
        testException<mavis::extension_manager::ExtensionNotAllowedException>(
            [&man]() { man.setISA("rv64imafdcbv"); });
    }

    {
        // Test multiple ISA strings with an allowlist
        auto man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISASpecJSON(
            "json/riscv_isa_spec.json", "json");
        man.allowExtensions({"g", "c", "zicsr", "zifencei"});

        const std::string isa_string = "rv64gc_zicsr_zifencei";
        man.setISA(isa_string);
        man.setISA(isa_string.substr(0, 6));
        testException<mavis::extension_manager::ExtensionNotAllowedException>(
            [&man]() { man.setISA("rv64gcb"); });
        man.clearAllowedExtensions();
        man.setISA("rv64gcb");
    }

    {
        // Test disabling extension
        auto man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            "rv64gc_zicsr_zifencei", "json/riscv_isa_spec.json", "json");
        const auto orig_jsons = man.getJSONs();
        man.disableExtension("d");
        auto new_jsons = man.getJSONs();
        assert(orig_jsons.size() == new_jsons.size() + 1);
        man.enableExtension("d");
        new_jsons = man.getJSONs();
        assert(orig_jsons == new_jsons);
        man.disableExtensions({"zicsr", "zifencei"});
        new_jsons = man.getJSONs();
        assert(orig_jsons.size() == new_jsons.size() + 2);
    }

    return 0;
}
