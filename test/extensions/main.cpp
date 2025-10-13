// Defining this enables additional circular dependency sanity checking via boost::graph
#define ENABLE_GRAPH_SANITY_CHECKER
#include "mavis/extension_managers/RISCVExtensionManager.hpp"

#include "Inst.h"
#include "uArchInfo.h"

struct ExampleTraceInfo
{
    std::string mnemonic;
    uint64_t opcode;

    const std::string & getMnemonic() const { return mnemonic; }

    uint64_t getOpcode() const { return opcode; }

    uint64_t getFunction() const { return 0; }

    uint64_t getSourceRegs() const { return 0; }

    uint64_t getDestRegs() const { return 0; }

    uint64_t getImmediate() const { return 0; }
};

template <typename ExpectedExceptionType, typename Callback>
void testException(Callback && callback)
{
    bool saw_exception = false;
    (void)saw_exception; // Needed for new compilers
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

        // Test enabled_by behavior - c + d (implied by g) should enable zcd
        assert(rv_generic_man.isEnabled("zcd"));

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
        // Disabling d will disable both d and zcd
        assert(orig_jsons.size() == new_jsons.size() + 2);
        man.enableExtension("d");
        new_jsons = man.getJSONs();
        assert(std::set(orig_jsons.begin(), orig_jsons.end())
               == std::set(new_jsons.begin(), new_jsons.end()));
        man.disableExtensions({"zicsr", "zifencei"});
        new_jsons = man.getJSONs();
        assert(orig_jsons.size() == new_jsons.size() + 2);
    }

    {
        // Test dependent extension behavior when enabling/disabling
        auto man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            "rv32imafdcv_zicsr", "json/riscv_isa_spec.json", "json");
        assert(man.isEnabled("f"));
        assert(man.isEnabled("d"));
        assert(man.isEnabled("zcf"));
        assert(man.isEnabled("zcd"));
        assert(man.isEnabled("zve32f"));
        assert(man.isEnabled("zve64d"));

        man.disableExtension("f");
        assert(!man.isEnabled("f"));
        assert(!man.isEnabled("d"));
        assert(!man.isEnabled("zcf"));
        assert(!man.isEnabled("zcd"));
        assert(!man.isEnabled("zve32f"));
        assert(!man.isEnabled("zve64d"));

        man.enableExtension("f");
        assert(man.isEnabled("f"));
        assert(man.isEnabled("d"));
        assert(man.isEnabled("zcf"));
        assert(man.isEnabled("zcd"));
        assert(man.isEnabled("zve32f"));
        assert(man.isEnabled("zve64d"));

        man.disableExtension("d");
        assert(man.isEnabled("f"));
        assert(!man.isEnabled("d"));
        assert(man.isEnabled("zcf"));
        assert(!man.isEnabled("zcd"));
        assert(man.isEnabled("zve32f"));
        assert(!man.isEnabled("zve64d"));

        man.disableExtension("f");
        assert(!man.isEnabled("f"));
        assert(!man.isEnabled("d"));
        assert(!man.isEnabled("zcf"));
        assert(!man.isEnabled("zcd"));
        assert(!man.isEnabled("zve32f"));
        assert(!man.isEnabled("zve64d"));

        man.enableExtension("f");
        assert(man.isEnabled("f"));
        assert(!man.isEnabled("d"));
        assert(man.isEnabled("zcf"));
        assert(!man.isEnabled("zcd"));
        assert(man.isEnabled("zve32f"));
        assert(!man.isEnabled("zve64d"));
    }

    {
        const std::vector<std::string> uarch{"uarch/uarch_rv64g.json"};

        // Test creating a Mavis object
        auto man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            "rv64gc_zicsr_zifencei", "json/riscv_isa_spec.json", "json");

        auto mavis = man.constructMavis<Instruction<uArchInfo>, uArchInfo>(uarch);
        const ExampleTraceInfo addi{"addi", 0xf0008013};
        const ExampleTraceInfo fadd{"fadd.s", 0x53};

        Instruction<uArchInfo>::PtrType iptr = mavis.makeInstFromTrace(addi, 0);
        iptr = mavis.makeInstFromTrace(fadd, 0);

        man.disableExtension("f");
        man.switchMavisContext(mavis);

        iptr = mavis.makeInstFromTrace(addi, 0);
        testException<mavis::UnknownOpcode>([&mavis, &iptr, &fadd]()
                                            { iptr = mavis.makeInstFromTrace(fadd, 0); });

        man.enableExtension("f");
        man.switchMavisContext(mavis);

        iptr = mavis.makeInstFromTrace(addi, 0);
        iptr = mavis.makeInstFromTrace(fadd, 0);
    }

    {
        // Test isExtensionSupported
        auto man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            "rv64gc_zicsr_zifencei", "json/riscv_isa_spec.json", "json");
        assert(man.isExtensionSupported("i"));
        assert(man.isExtensionSupported("c"));
        assert(!man.isExtensionSupported("znotanextension"));
    }

    {
        // Test internal-only flag functionality
        auto man = mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            "rv32gc_zbb_zbkb", "json/riscv_isa_spec.json", "json");

        assert(man.isEnabled("zbb"));
        assert(man.isEnabled("zbkb"));
        // This method asks without checking if an extension is internal
        assert(man.isEnabled("zbb_zbkb_common"));

        // This view will filter out internal extensions
        const auto & enabled_extensions = man.getEnabledExtensions();
        assert(enabled_extensions.isEnabled("zbb"));
        assert(enabled_extensions.isEnabled("zbkb"));
        assert(!enabled_extensions.isEnabled("zbb_zbkb_common"));
    }
    return 0;
}
