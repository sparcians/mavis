#include <iostream>
#include <bit>

#include "mavis/Mavis.h"
#include "mavis/MatchSet.hpp"
#include "mavis/Tag.hpp"
#include "mavis/Pattern.hpp"
#include "mavis/ExtractorDirectImplementations.hpp"

#include "Inst.h"
#include "uArchInfo.h"

using namespace std;

#define ASSERT_ALWAYS(condition) \
    if (!(condition)) { \
        std::cerr << "Assertion failed: " << #condition << ", file " << __FILE__ \
                  << ", line " << __LINE__ << std::endl; \
        std::abort(); \
    }

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

using MavisType = Mavis<Instruction<uArchInfo>, uArchInfo>;

void runTSet(MavisType & mavis_facade, const std::string & tfile,
             const std::vector<mavis::OpcodeInfo::ISAExtension> isa_list = {})
{
    // Run a test file
    ifstream tin(tfile);

    while (tin)
    {
        string line;
        tin >> line;
        if (tin)
        {
            char mnestr[100];
            long long icode;
            // Dave and I are in a silent fight here.  llx or lx?  gcc
            // says lx for uint64_t types.  clang doesn't seem to
            // care.  I say lx, Dave says llx.  If we force icode to
            // be a long long, everyone is happy as long as the sizes
            // are the same.
            static_assert(sizeof(long long) == sizeof(uint64_t));
            sscanf(line.c_str(), "%[^,],%llx", mnestr, &icode);

            // uint64_t icode = stoll(hexstr, nullptr, 16);
            // Instruction*     iptr = dt.makeInst(icode);
            ExampleTraceInfo tinfo;
            tinfo.mnemonic = std::string(mnestr);
            tinfo.opcode = icode;

            Instruction<uArchInfo>::PtrType iptr = nullptr;
            try
            {
                iptr = mavis_facade.makeInstFromTrace(tinfo, 0);
            }
            catch (const mavis::IllegalOpcode & ex)
            {
                cout << ex.what() << endl;
            }
            catch (const mavis::UnknownOpcode & ex)
            {
                cout << ex.what() << endl;
            }

            if (iptr != nullptr)
            {
                cout << "HEX: 0x" << hex << icode << ", INST: " << *iptr
                     << ", DASM: " << iptr->dasmString() << endl;
                if (iptr->getMnemonic() != std::string(mnestr))
                {
                    cout << "WARNING: expected '" << mnestr << "'"
                         << ", decoded '" << iptr->getMnemonic() << "'" << endl;
                }

                // Check getInfo...
                MavisType::DecodeInfoType optr = mavis_facade.getInfo(tinfo.getOpcode());
                ASSERT_ALWAYS(optr != nullptr);
                if (optr->opinfo->getMnemonic() != iptr->getMnemonic())
                {
                    cout << "OPCODE INFO WARNING: expected '" << iptr->getMnemonic() << "'"
                         << ", retrieved '" << optr->opinfo->getMnemonic() << "'" << endl;
                }

                // Check UID...
                const mavis::InstructionUniqueID expected_id =
                    mavis_facade.lookupInstructionUniqueID(iptr->getMnemonic());
                if (optr->opinfo->getInstructionUniqueID() != expected_id)
                {
                    cout << "INSTRUCTION ID WARNING: expected ID=" << std::dec << expected_id
                         << ", got ID=" << optr->opinfo->getInstructionUniqueID() << endl;
                }

                // Check ISA list if present
                if (!isa_list.empty())
                {
                    for (const auto & i : isa_list)
                    {
                        if (!(optr->opinfo->isISA(i) && optr->opinfo->isISAAnyOf(i)
                              && optr->opinfo->isISAAllOf(i)
                              && optr->opinfo->isISASameAs(optr->opinfo->getISA(), i)))
                        {
                            cout << "INSTRUCTION '" << optr->opinfo->getMnemonic() << "' "
                                 << "failed the expected ISA check against enum '"
                                 << static_cast<
                                        std::underlying_type_t<mavis::OpcodeInfo::ISAExtension>>(i)
                                 << "'" << endl;
                        }
                    }
                }
            }

            // Check direct info extraction
            try
            {
                mavis::ExtractorDirectInfo ex_info_A(tinfo.mnemonic, {1, 2}, {3});
                Instruction<uArchInfo>::PtrType iptr_A =
                    mavis_facade.makeInstDirectly(ex_info_A, 0);
                ASSERT_ALWAYS(iptr_A != nullptr);

                const mavis::InstructionUniqueID uid =
                    mavis_facade.lookupInstructionUniqueID(tinfo.mnemonic);
                mavis::ExtractorDirectInfo ex_info_B(uid, {1, 2}, {3});
                Instruction<uArchInfo>::PtrType iptr_B =
                    mavis_facade.makeInstDirectly(ex_info_B, 0);
                ASSERT_ALWAYS(iptr_B != nullptr);

                ASSERT_ALWAYS(iptr_A->getUID() == iptr_B->getUID());
                ASSERT_ALWAYS(iptr_A->getMnemonic() == iptr_B->getMnemonic());
            }
            catch (const mavis::UnknownMnemonic & ex)
            {
                // We currently don't support overlays (mv) and compressed instructions
                // in ExtractorDirectInfo...
                cout << ex.what() << endl;
            }
        }
    }
    tin.close();
}

int main()
{
    constexpr mavis::InstructionUniqueID NOP_UID = 1;

    // UID initialization list for testing NOP and C.NOP UID's against NOP_UID
    mavis::InstUIDList uid_init{
        {"nop",  NOP_UID},
        {"cmov", 2      }
    };

    mavis::AnnotationOverrides anno_overrides = {
        std::make_pair("andn", "pipelined:false"),
        std::make_pair("srai", "rob_group:[\"begin\"]"),
    };

    MavisType mavis_facade({"json/isa_rv64i.json",        // included in "g" spec
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
                            "json/isa_rv64zve32x.json",
                            "json/isa_rv64zve32f.json",
                            "json/isa_rv64zve64x.json",
                            "json/isa_rv64zve64d.json",
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
                            "json/isa_rv64zvfbfwma.json",
                            "json/isa_rv64svinval.json",
                            "json/isa_rv64zbkx.json",
                            "json/isa_rv64zknd.json"
    },
                           {"uarch/uarch_rv64g.json"}, uid_init, anno_overrides);
    cout << mavis_facade;

    Instruction<uArchInfo>::PtrType inst = nullptr;
    try
    {
        inst = mavis_facade.makeInst(0x0, 0);
    }
    catch (const mavis::IllegalOpcode & ex)
    {
        cout << ex.what() << endl;
    }

    if (inst != nullptr)
    {
        cout << "DASM: 0x0 = " << inst->dasmString() << endl;
    }

    runTSet(mavis_facade, "rv64.tset");

    // Test HV decoding
    runTSet(mavis_facade, "rv64h.tset");

    // Test the Vector Crypto decoding
    runTSet(mavis_facade, "rv64zvk.tset");

    // Test BF16 extensions
    runTSet(mavis_facade, "rv64_bf16.tset");

    // Exercise the cache
    runTSet(mavis_facade, "rv64.tset");
    mavis_facade.flushCaches();
    runTSet(mavis_facade, "rv64.tset");

    // "NEW" context should haven't been existed
    ASSERT_ALWAYS(mavis_facade.hasContext("NEW") == false);

    // Try creating a new context
    mavis_facade.makeContext("NEW",
                             {"json/isa_rv64i.json",
                              "json/isa_rv64m.json",
                              "json/isa_rv64zmmul.json",
                              "json/isa_rv64zaamo.json",
                              "json/isa_rv64zalrsc.json",
                              "json/isa_rv64f.json",
                              "json/isa_rv64d.json",
                              "json/isa_rv64q.json",
                              "json/isa_rv64q_addons.json",
                              "json/isa_rv64zfa.json",
                              "json/isa_rv64zfa_d.json",
                              "json/isa_rv64zfa_q.json",
                              "json/isa_rv64zfa_q_addons.json",
                              "json/isa_rv64zfa_h.json",
                              "json/isa_rv64zca.json",
                              "json/isa_rv64zcd.json",
                              "json/isa_rv64zicsr.json",
                              "json/isa_rv64zifencei.json",
                              "json/isa_rv64zve32x.json",
                              "json/isa_rv64zve32f.json",
                              "json/isa_rv64zve64x.json",
                              "json/isa_rv64zve64d.json",
                              "json/isa_rv64zfh.json",
                              "json/isa_rv64zfh_addons.json",
                              "json/isa_rv64zfhmin.json",
                              "json/isa_rv64zfhmin_d.json",
                              "json/isa_rv64zba.json",
                              "json/isa_rv64zbb.json",
                              "json/isa_rv64zbc.json",
                              "json/isa_rv64zbs.json",
                              "json/isa_rv64zicbom.json",
                              "json/isa_rv64zicbop.json",
                              "json/isa_rv64zicboz.json",
                              "json/isa_rv64zcb.json",
                              "json/isa_rv64zihintntl.json",
                              "json/isa_rv64zihintpause.json",
                              "json/isa_rv64zicond.json"},
                             {});
    mavis_facade.switchContext("NEW");
    cout << mavis_facade;
    runTSet(mavis_facade, "rv64.tset");
    runTSet(mavis_facade, "rv64_bits.tset", {mavis::InstMetaData::ISAExtension::B});
    runTSet(mavis_facade, "rv64_zcb.tset");
    runTSet(mavis_facade, "rv64_zicond.tset");
    runTSet(mavis_facade, "rv64_zbkx.tset");
    runTSet(mavis_facade, "rv64_zknd.tset");

    // Check implied extraction fields for zcb c.zext.[bhw] instructions
    // c.zext.b should have 0xFF implied immediate
    inst = mavis_facade.makeInst(0x9c61, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x9c61 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->hasImmediate());
    ASSERT_ALWAYS(inst->getImmediate() == 0xFF);

    // c.zext.h should have RS2 implied source (X0) and implied immediate 0
    inst = mavis_facade.makeInst(0x9c69, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x9c69 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->hasImmediate());
    ASSERT_ALWAYS(inst->getImmediate() == 0);
    ASSERT_ALWAYS(inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RS2));
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RS2) == 0);

    // c.zext.w should have RS2 implied source (X0) and implied immediate 0
    inst = mavis_facade.makeInst(0x9c71, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x9c71 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->hasImmediate());
    ASSERT_ALWAYS(inst->getImmediate() == 0);
    ASSERT_ALWAYS(inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RS2));
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RS2) == 0);

    // sext.b
    inst = mavis_facade.makeInst(0x60401013, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x0x60401013 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "sext.b");

    // Switch back to BASE context
    mavis_facade.switchContext("BASE");

    // 0x003100b3 = add	1,2,3
    inst = mavis_facade.makeInst(0x003100b3, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x003100b3 = " << inst->dasmString()
         << endl;

    // 0x02028593 = addi	11,5, IMM=0x20
    inst = mavis_facade.makeInst(0x02028593, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x02028593 = " << inst->dasmString()
         << endl;

    // 0x02028593 = mv	11,5
    inst = mavis_facade.makeInst(0x00028593, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x00028593 = " << inst->dasmString()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Has Immediate? "
         << (inst->hasImmediate() ? "YES" : "no") << endl;

    // 0x4081 = c.li	1, IMM=0x0
    inst = mavis_facade.makeInst(0x4081, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x4081 = " << inst->dasmString() << endl;

    // 0x000280e7 = jalr	1,5, IMM=0x0
    inst = mavis_facade.makeInst(0x000280e7, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x000280e7 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::BRANCH));
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::JALR));
    ASSERT_ALWAYS(inst->isExtInstType(mavis::DecodedInstructionInfo::ExtractedInstTypes::CALL));
    ASSERT_ALWAYS(inst->isExtInstType(mavis::DecodedInstructionInfo::ExtractedInstTypes::RETURN));

    // 0xe152 = c.sdsp	20, SP, IMM=0x0
    inst = mavis_facade.makeInst(0xe152, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xe152 = " << inst->dasmString() << endl;

    // 0xfcd6 = c.sdsp	21, SP, IMM=0x0
    inst = mavis_facade.makeInst(0xfcd6, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xfcd6 = " << inst->dasmString() << endl;

    // 0xf1402573 = csrrs	10,0, CSR=0xf14
    inst = mavis_facade.makeInst(0xf1402573, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xf1402573 = " << inst->dasmString()
         << endl;

    mavis::ExtractorDirectInfo ex_info("add", {1, 2}, {3});
    inst = mavis_facade.makeInstDirectly(ex_info, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DIRECT: 'add' = " << inst->dasmString() << endl;

    mavis::ExtractorDirectInfoBitMask exbm_info("add", 0x6, 0x8);
    inst = mavis_facade.makeInstDirectly(exbm_info, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DIRECT_BM: 'add' = " << inst->dasmString()
         << endl;

    mavis::ExtractorDirectInfo_Stores sx_info("sw", {1}, {2});
    inst = mavis_facade.makeInstDirectly(sx_info, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DIRECT: 'sw' = " << inst->dasmString() << endl;

    mavis::ExtractorDirectInfoBitMask_Stores sxbm_info("sw", 0x2, 0x4);
    inst = mavis_facade.makeInstDirectly(sxbm_info, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DIRECT_BM: 'sw' = " << inst->dasmString()
         << endl;

    // 0x907405e3 = beq	8,7 +0xfffffffffffff90a
    inst = mavis_facade.makeInst(0x907405e3, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x907405e3 = " << inst->dasmString()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex
         << inst->getSignedOffset() << std::dec << endl;

    // 0x107405e3 = beq	8,7 +0x90a
    inst = mavis_facade.makeInst(0x107405e3, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x107405e3 = " << inst->dasmString()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex
         << inst->getSignedOffset() << std::dec << endl;

    // 0xd3ad = c.beqz	15, +0xffffffffffffff62
    inst = mavis_facade.makeInst(0xd3ad, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xd3ad = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex
         << inst->getSignedOffset() << std::dec << endl;

    // 0xc3ad = c.beqz	15, +0x62
    inst = mavis_facade.makeInst(0xc3ad, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xc3ad = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex
         << inst->getSignedOffset() << std::dec << endl;

    // 0x8f16c3ef = jal	7, +0xfffffffffff6c8f0
    inst = mavis_facade.makeInst(0x8f16c3ef, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x8f16c3ef = " << inst->dasmString()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex
         << inst->getSignedOffset() << std::dec << endl;

    // 0x0f16c3ef = jal	7, +0x6c8f0
    inst = mavis_facade.makeInst(0x0f16c3ef, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x0f16c3ef = " << inst->dasmString()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex
         << inst->getSignedOffset() << std::dec << endl;

    // 0xb555 = c.j	 +0xfffffffffffffea4
    inst = mavis_facade.makeInst(0xb555, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xb555 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex
         << inst->getSignedOffset() << std::dec << endl;

    // 0xa555 = c.j	 +0x6a4
    inst = mavis_facade.makeInst(0xa555, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xa555 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex
         << inst->getSignedOffset() << std::dec << endl;

    // 0xa9cc0767 = jalr	14,24, IMM=0xfffffffffffffa9c
    inst = mavis_facade.makeInst(0xa9cc0767, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xa9cc0767 = " << inst->dasmString()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex
         << inst->getSignedOffset() << std::dec << endl;

    // 0xbe10afa3 = sw	1,1
    inst = mavis_facade.makeInst(0xbe10afa3, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xbe10afa3 = " << inst->dasmString()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs()
         << endl;

    mavis::Opcode jalr_stencil = mavis_facade.getOpcode("jalr");
    cout << "line " << dec << __LINE__ << ": " << "Stencil for 'jalr' = 0x" << hex << jalr_stencil
         << dec << endl;
    inst = mavis_facade.makeInst(jalr_stencil, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x" << hex << jalr_stencil << dec << " = "
         << inst->dasmString() << endl;

    // 0x60e2 = c.ldsp	1, SP, IMM=0x0
    inst = mavis_facade.makeInst(0x60e2, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x60e2 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Has Immediate? "
         << (inst->hasImmediate() ? "YES" : "no") << endl;

    // fld       f0,0(x10)
    inst = mavis_facade.makeInst(0x53007, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x53007 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs()
         << endl;

    // c.fld     f8,64(x10)
    inst = mavis_facade.makeInst(0x2120, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x2120 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs()
         << endl;

    // mret	0,0,0
    inst = mavis_facade.makeInst(0x30200073, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x30200073 = " << inst->dasmString()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs()
         << endl;

    // lr.w 0,0
    inst = mavis_facade.makeInst(0x1000202f, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x1000202f = " << inst->dasmString()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs()
         << endl;

    // c.fld     f10,80(x10)
    inst = mavis_facade.makeInst(0x2928, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x2928 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Int-Sources: " << inst->getIntSourceRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Float-Sources: " << inst->getFloatSourceRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Int-Dests: " << inst->getIntDestRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Float-Dests: " << inst->getFloatDestRegs()
         << endl;

    // c.fld     f11,88(x10)
    inst = mavis_facade.makeInst(0x2d2c, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x2d2c = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Int-Sources: " << inst->getIntSourceRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Float-Sources: " << inst->getFloatSourceRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Int-Dests: " << inst->getIntDestRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Float-Dests: " << inst->getFloatDestRegs()
         << endl;

    // 0x72a7f543 fmadd.d  f10 = f15, f10, f14
    inst = mavis_facade.makeInst(0x72a7f543, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x72a7f543 = " << inst->dasmString()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "fmadd.d RM field = 0x" << hex
         << inst->getSpecialField(mavis::OpcodeInfo::SpecialField::RM) << dec << endl;

    // Operand types for fcvt.l.d
    mavis::ExtractorDirectInfo ex_info_fcvt("fcvt.l.d", {1}, {4});
    inst = mavis_facade.makeInstDirectly(ex_info_fcvt, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DIRECT: 'fcvt.l.d' = " << inst->dasmString()
         << endl;

    // Try Mavis::isOpcodeInfo and Mavis::isOpcodeExtractedInfo
    // 0x000280e7 = jalr	1,5, IMM=0x0
    ASSERT_ALWAYS(mavis_facade.isOpcodeInstType(0x000280e7, MavisType::InstructionType::BRANCH));
    ASSERT_ALWAYS(mavis_facade.isOpcodeExtractedInstType(0x000280e7, MavisType::ExtractedInstType::CALL));

    //  0x8006 c.mv
    inst = mavis_facade.makeInst(0x8006, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x8006 = " << inst->dasmString() << endl;

    mavis::ExtractorDirectInfo cmov_info("cmov", {1, 2, 3}, {4});
    mavis_facade.morphInst(inst, cmov_info);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "MORPH DASM: = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::INT));
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::MOVE));
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::CONDITIONAL));

    // NOP (addi x0, x0, 0)
    inst = mavis_facade.makeInst(mavis::CANONICAL_NOP, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM (CANONICAL_NOP): = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getUID() == NOP_UID);

    // C.NOP (c.addi x0, x0, 0)
    inst = mavis_facade.makeInst(mavis::CANONICAL_CNOP, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": "
         << "DASM (C.NOP/CANONICAL_CNOP): = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getUID() == NOP_UID);

    // Operand types for floating point compares
    mavis::ExtractorDirectInfo ex_info_feq("feq.s", {1, 2}, {4});
    inst = mavis_facade.makeInstDirectly(ex_info_feq, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DIRECT: 'feq.s' = " << inst->dasmString()
         << endl;

    //  0x710d c.addi16sp 2, 0x2A0  (0xF...FEA0 when sign extended)
    inst = mavis_facade.makeInst(0x710d, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x710d = " << inst->dasmString() << endl;

    //  0x5769 c.li x14, 0x3A  (0xF...FA when sign extended)
    inst = mavis_facade.makeInst(0x5769, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x5769 = " << inst->dasmString() << endl;

    //  0x7769 c.lui x14, 0x3A000  (0xF...FA000 when sign extended)
    inst = mavis_facade.makeInst(0x7769, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x7769 = " << inst->dasmString() << endl;

    //  0x177c c.addi4spn x15, 0x3AC
    inst = mavis_facade.makeInst(0x177c, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x177c = " << inst->dasmString() << endl;

    //  0x0063b2af amoadd.d 5,7,6, aq=0, rl=0
    inst = mavis_facade.makeInst(0x0063b2af, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x0063b2af = " << inst->dasmString()
         << endl;

    //  0x8516 c.mv rd, 5 --> add rd, rs1 = x0, rs2 = 5 (must preserve x0)
    inst = mavis_facade.makeInst(0x8516, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x8516 = " << inst->dasmString() << endl;

    //  0xe3c1 c.bnez 15, 0, +0x80 (must preserve x0)
    inst = mavis_facade.makeInst(0xe3c1, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xe3c1 = " << inst->dasmString() << endl;

    //  0x9696 c.add rD, rS --> add rD, rD, rS
    inst = mavis_facade.makeInst(0x9696, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x9696 = " << inst->dasmString() << endl;

    //  0x5877857 vsetvli a6, a4, e64, m1, ta, mu
    inst = mavis_facade.makeInst(0x5877857, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x5877857 = " << inst->dasmString()
         << endl;

    // 0x803170D7 vsetvl, x1, x2, x3
    inst = mavis_facade.makeInst(0x803170D7, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x803170d7 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getVectorSourceRegs() == 0ull);
    ASSERT_ALWAYS(inst->getVectorDestRegs() == 0ull);
    ASSERT_ALWAYS(inst->getIntSourceRegs() == 12ull);
    ASSERT_ALWAYS(inst->getIntDestRegs() == 2ull);

    // 0x2f007, vle64, v0, x5
    inst = mavis_facade.makeInst(0x2f007, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x2f007 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) == 0ull);
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 0ull);
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);

    // 0x102f007, vle64ff, v0, x5
    inst = mavis_facade.makeInst(0x102f007, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x102f007 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) == 0ull);
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 0ull);
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::FAULTFIRST) == 1);

    // 0x202f007, vle64, v0, x5, v0.t
    inst = mavis_facade.makeInst(0x202f007, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x202f007 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) == 1ull);
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 0ull);
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);

    // 0x2206e007, vlseg2e32.v v0, x13
    inst = mavis_facade.makeInst(0x2206e007, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x2206e007 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) == 1ull);
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 1ull);
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);

    // 0xa606e007, vluxseg6e32.v v0, x13
    inst = mavis_facade.makeInst(0xa606e007, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xa606e007 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) == 1ull);
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 5ull);
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);

    // 0xea06e007, vlsseg8e32.v v0, x13
    inst = mavis_facade.makeInst(0xea06e007, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xea06e007 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) == 1ull);
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 7ull);
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);

    // vloxei8.v,0xc000007
    inst = mavis_facade.makeInst(0xc000007, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xc000007 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 0ull);
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);

    // vsuxei8.v,0x4000027
    inst = mavis_facade.makeInst(0x4000027, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x4000027 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 0ull);

    // vadd.vi v31, v31, 0x1F (-1)
    inst = mavis_facade.makeInst(0x03ffbfd7, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x03ffbfd7 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) == 1ull);
    ASSERT_ALWAYS(inst->getSignedOffset() == -1);

    // vadd.vv v1, v2, v3, 0x3100D7
    inst = mavis_facade.makeInst(0x3100D7, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x3100D7 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getVectorSourceRegs() == 0b1100);
    ASSERT_ALWAYS(inst->getVectorDestRegs() == 0b10);
    ASSERT_ALWAYS(inst->getDestOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RD));
    ASSERT_ALWAYS(inst->getDestOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RD) == 1);
    ASSERT_ALWAYS(inst->getDestOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RD)
           == mavis::OpcodeInfo::OperandTypes::VECTOR);
    ASSERT_ALWAYS(inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RS1));
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RS1) == 2);
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS1)
           == mavis::OpcodeInfo::OperandTypes::VECTOR);
    ASSERT_ALWAYS(inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RS2));
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RS2) == 3);
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2)
           == mavis::OpcodeInfo::OperandTypes::VECTOR);
    try
    {
        inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RS3);
    }
    catch (const mavis::OperandInfoInvalidFieldID & ex)
    {
        cout << "line " << dec << __LINE__ << ": " << ex.what() << endl;
    }

    // vadc.vvm v1, v2, v3, 0x403100D7
    inst = mavis_facade.makeInst(0x403100D7, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x403100D7 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getVectorSourceRegs() == 0b1100);
    ASSERT_ALWAYS(inst->getVectorDestRegs() == 0b10);
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::MASK) == 0);

    // vmv.v.v v2, v1, 0x5E008157
    inst = mavis_facade.makeInst(0x5E008157, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x5E008157 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getVectorSourceRegs() == 0b10);
    ASSERT_ALWAYS(inst->getVectorDestRegs() == 0b100);

    // vadd.vx v1, v3, x2, 0x3140D7
    inst = mavis_facade.makeInst(0x3140D7, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x3140D7 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getIntSourceRegs() == 0b100);
    ASSERT_ALWAYS(inst->getVectorSourceRegs() == 0b1000);
    ASSERT_ALWAYS(inst->getVectorDestRegs() == 0b10);

    // vadc.vxm v1, v3, x2, 0x403140D7
    inst = mavis_facade.makeInst(0x403140D7, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x403140D7 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getIntSourceRegs() == 0b100);
    ASSERT_ALWAYS(inst->getVectorSourceRegs() == 0b1000);
    ASSERT_ALWAYS(inst->getVectorDestRegs() == 0b10);

    // vmv.v.x v2, x1, 0x5E00C157
    inst = mavis_facade.makeInst(0x5E00C157, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x5E00C157 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getIntSourceRegs() == 0b10);
    ASSERT_ALWAYS(inst->getVectorDestRegs() == 0b100);

    // vmv1r.v v1, v2, 0x9E2030D7
    inst = mavis_facade.makeInst(0x9E2030D7, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 9E2030D7 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getVectorSourceRegs() == 0b100);
    ASSERT_ALWAYS(inst->getVectorDestRegs() == 0b10);

    // vid.v v1, 0x5008A0D7
    inst = mavis_facade.makeInst(0x5008A0D7, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 5008A0D7 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getVectorSourceRegs() == 0);
    ASSERT_ALWAYS(inst->getVectorDestRegs() == 0b10);

    // vse8.v v1, x2 0x100a7
    inst = mavis_facade.makeInst(0x100a7, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 100a7 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getIntSourceRegs() == 0b100);
    ASSERT_ALWAYS(inst->getVectorSourceRegs() == 0b10);
    ASSERT_ALWAYS(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);

    // 0xc6880857, vwredsum.vs v16,v8,v16
    inst = mavis_facade.makeInst(0xc6880857, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xc6880857 = " << inst->dasmString()
         << endl;

    // 0x52a1b657, vror.vi v12,v10,3
    inst = mavis_facade.makeInst(0x52a1b657, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x52a1b657 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getImmediate() == 3);
    ASSERT_ALWAYS(inst->hasImmediate() == true);

    // 0x56b43757, vror.vi v12,v10,40
    inst = mavis_facade.makeInst(0x56b43757, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x56b43757 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getImmediate() == 40);
    ASSERT_ALWAYS(inst->hasImmediate() == true);

    // 0xb662a257, vmacc.vv	v4,v5,v6
    inst = mavis_facade.makeInst(0xb662a257, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x56b43757 = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getDestOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RD));
    ASSERT_ALWAYS(inst->getDestOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RD) == 4);
    ASSERT_ALWAYS(inst->getDestOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RD)
           == mavis::OpcodeInfo::OperandTypes::VECTOR);
    ASSERT_ALWAYS(inst->getSourceOpInfo().hasImpliedOperand());
    ASSERT_ALWAYS(inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RS1));
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RS1) == 5);
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS1)
           == mavis::OpcodeInfo::OperandTypes::VECTOR);
    ASSERT_ALWAYS(inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RS2));
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RS2) == 6);
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2)
           == mavis::OpcodeInfo::OperandTypes::VECTOR);
    ASSERT_ALWAYS(inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RD));
    ASSERT_ALWAYS(inst->getSourceOpInfo().isImplied(mavis::InstMetaData::OperandFieldID::RD));
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RD) == 4);
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RD)
           == mavis::OpcodeInfo::OperandTypes::VECTOR);

    // Issue #87
    // mavis::ExtractorDirectBase::RegListType dsts_ {8};
    // mavis::ExtractorDirectBase::RegListType srcs_ {30};

    mavis::ExtractorDirectInfo vector_info("vsext.vf2", {30}, {8});
    inst = mavis_facade.makeInstDirectly(vector_info, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM(Direct): = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getDestOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RD));
    ASSERT_ALWAYS(inst->getDestOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RD) == 8);
    ASSERT_ALWAYS(inst->getDestOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RD)
           == mavis::OpcodeInfo::OperandTypes::VECTOR);
    ASSERT_ALWAYS(!inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RS1));
    ASSERT_ALWAYS(inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RS2));
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RS2) == 30);
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2)
           == mavis::OpcodeInfo::OperandTypes::VECTOR);

    mavis::ExtractorDirectOpInfoList vector_opinfo(
        "vsext.vf2",
        {{{mavis::InstMetaData::OperandFieldID::RS2, mavis::InstMetaData::OperandTypes::VECTOR, 30,
           false}}},
        {{{mavis::InstMetaData::OperandFieldID::RD, mavis::InstMetaData::OperandTypes::VECTOR,
           8}}});
    inst = mavis_facade.makeInstDirectly(vector_opinfo, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM(DirectOpInfo): = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getDestOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RD));
    ASSERT_ALWAYS(inst->getDestOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RD) == 8);
    ASSERT_ALWAYS(inst->getDestOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RD)
           == mavis::OpcodeInfo::OperandTypes::VECTOR);
    ASSERT_ALWAYS(!inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RS1));
    ASSERT_ALWAYS(inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RS2));
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RS2) == 30);
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2)
           == mavis::OpcodeInfo::OperandTypes::VECTOR);

    // c.fld     f8,64(x10)
    inst = mavis_facade.makeInst(0x2120, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x53007 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs()
         << endl;

    // 0x40e2 = c.lwsp	1, SP, IMM=0x18
    inst = mavis_facade.makeInst(0x40e2, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x40e2 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Has Immediate? "
         << (inst->hasImmediate() ? "YES" : "no") << endl;

    // 0x60e2 = c.flwsp	1, SP, IMM=0x18
    inst = mavis_facade.makeInst(0x60e2, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x60e2 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs()
         << endl;
    cout << "line " << dec << __LINE__ << ": " << "Has Immediate? "
         << (inst->hasImmediate() ? "YES" : "no") << endl;

    // 0x650d = c.lui	10, x3
    inst = mavis_facade.makeInst(0x650d, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x650d = " << inst->dasmString() << endl;

    // 0x12000073 = sfence
    inst = mavis_facade.makeInst(0x12000073, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x12000073 = " << inst->dasmString()
         << endl;

    // 0xa422 = c.fsdsp	8, SP, IMM=0x8
    inst = mavis_facade.makeInst(0xa422, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xa422 = " << inst->dasmString() << endl;

    // Issue #89 - half-precision operands
    // 0x4310253 = fadd.h  f4,f2,f3, RM=0
    inst = mavis_facade.makeInst(0x4310253, 0);
    ASSERT_ALWAYS(inst != nullptr);
    ASSERT_ALWAYS(inst->getOpInfo()->numSourceRegsByType(mavis::InstMetaData::OperandTypes::HALF) == 2);
    ASSERT_ALWAYS(inst->getOpInfo()->getDestRegsByType(mavis::InstMetaData::OperandTypes::HALF) != 0);
    // 0x44010253 = fcvt.h.s   f4,f2, RM=0
    inst = mavis_facade.makeInst(0x44010253, 0);
    ASSERT_ALWAYS(inst != nullptr);
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS1)
           == mavis::OpcodeInfo::OperandTypes::SINGLE);
    ASSERT_ALWAYS(inst->getDestOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RD)
           == mavis::OpcodeInfo::OperandTypes::HALF);

    // Create a new context for testing pseudo instructions
    mavis_facade.makeContext("PSEUDO", {"json/isa_rv64i.json", "uarch/isa_pseudo.json"},
                             {"uarch/uarch_rv64g.json", "uarch/uarch_pseudo.json"});
    mavis_facade.switchContext("PSEUDO");

    // clang-format off
    mavis::ExtractorPseudoInfo pseudo_op(
        "P0",
        {{mavis::InstMetaData::OperandFieldID::RS1, mavis::InstMetaData::OperandTypes::WORD, 1},
         {mavis::InstMetaData::OperandFieldID::RS2, mavis::InstMetaData::OperandTypes::DOUBLE, 2}},
        {{mavis::InstMetaData::OperandFieldID::RD, mavis::InstMetaData::OperandTypes::WORD, 3}});
    // clang-format on
    inst = mavis_facade.makePseudoInst(pseudo_op, 0);
    cout << "line " << dec << __LINE__ << ": " << "PSEUDO = " << inst->dasmString() << endl;

    mavis::ExtractorDirectInfo di_pseudo_op("P0", {1, 2}, {3});
    inst = mavis_facade.makePseudoInst(di_pseudo_op, 0);
    cout << "line " << dec << __LINE__ << ": " << "PSEUDO = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS1)
           == mavis::OpcodeInfo::OperandTypes::WORD);
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2)
           == mavis::OpcodeInfo::OperandTypes::DOUBLE);
    ASSERT_ALWAYS(inst->getDestOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RD)
           == mavis::OpcodeInfo::OperandTypes::WORD);

    mavis::ExtractorDirectInfoBitMask dibm_pseudo_op("P0", (0x1 << 1) | (0x1 << 2), 0x1 << 3,
                                                     0xDEAD);
    inst = mavis_facade.makePseudoInst(dibm_pseudo_op, 0);
    cout << "line " << dec << __LINE__ << ": " << "PSEUDO = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS1)
           == mavis::OpcodeInfo::OperandTypes::WORD);
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2)
           == mavis::OpcodeInfo::OperandTypes::DOUBLE);
    ASSERT_ALWAYS(inst->getDestOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RD)
           == mavis::OpcodeInfo::OperandTypes::WORD);

    mavis::ExtractorDirectInfo_Stores di_pseudo_op_p1("P1", {1}, {2},
                                                      mavis::OpcodeInfo::SpecialFieldsMap({{mavis::OpcodeInfo::SpecialField::VM, 3}}));
    inst = mavis_facade.makePseudoInst(di_pseudo_op_p1, 0);
    cout << "line " << dec << __LINE__ << ": " << "PSEUDO = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": "
         << "VM = " << inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) << endl;
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS1)
           == mavis::OpcodeInfo::OperandTypes::LONG);
    ASSERT_ALWAYS(inst->getSourceOpInfo().isStoreData(mavis::InstMetaData::OperandFieldID::RS1) == false);
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2)
           == mavis::OpcodeInfo::OperandTypes::LONG);
    ASSERT_ALWAYS(inst->getSourceOpInfo().isStoreData(mavis::InstMetaData::OperandFieldID::RS2) == true);
    try
    {
        inst->getSpecialField(mavis::OpcodeInfo::SpecialField::AVL);
        ASSERT_ALWAYS(false); // should not get here
    }
    catch (const mavis::UnsupportedExtractorSpecialFieldID & ex)
    {
        // OK
    }

    mavis::ExtractorDirectInfoBitMask_Stores dibm_pseudo_op_p1("P1", 0x1 << 1, 0x1 << 2, 0xBEEF);
    inst = mavis_facade.makePseudoInst(dibm_pseudo_op_p1, 0);
    cout << "line " << dec << __LINE__ << ": " << "PSEUDO = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS1)
           == mavis::OpcodeInfo::OperandTypes::LONG);
    ASSERT_ALWAYS(inst->getSourceOpInfo().isStoreData(mavis::InstMetaData::OperandFieldID::RS1) == false);
    ASSERT_ALWAYS(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2)
           == mavis::OpcodeInfo::OperandTypes::LONG);
    ASSERT_ALWAYS(inst->getSourceOpInfo().isStoreData(mavis::InstMetaData::OperandFieldID::RS2) == true);

    mavis::InstructionUniqueID p0_uid = mavis_facade.lookupPseudoInstUniqueID("P0");
    // clang-format off
    mavis::ExtractorPseudoInfo pseudo_op_by_uid(
        p0_uid,
        {{mavis::InstMetaData::OperandFieldID::RS1, mavis::InstMetaData::OperandTypes::WORD, 1},
         {mavis::InstMetaData::OperandFieldID::RS2, mavis::InstMetaData::OperandTypes::DOUBLE, 2}},
        {{mavis::InstMetaData::OperandFieldID::RD, mavis::InstMetaData::OperandTypes::WORD, 3}});
    // clang-format on
    inst = mavis_facade.makePseudoInst(pseudo_op_by_uid, 0);
    cout << "line " << dec << __LINE__ << ": " << "PSEUDO = " << inst->dasmString() << endl;

    // Test out some meta data queries needed for fusion... (using the P0 pseudo inst)
    ASSERT_ALWAYS(inst->getOpInfo()->isInstTypeAnyOf(mavis::InstMetaData::InstructionTypes::FLOAT,
                                              mavis::InstMetaData::InstructionTypes::BRANCH));
    ASSERT_ALWAYS(inst->getOpInfo()->isInstTypeAllOf(mavis::InstMetaData::InstructionTypes::BRANCH,
                                              mavis::InstMetaData::InstructionTypes::CONDITIONAL));
    ASSERT_ALWAYS(inst->getOpInfo()->isInstTypeSameAs(inst->getOpInfo()->getInstType(),
                                               mavis::InstMetaData::InstructionTypes::BRANCH));
    ASSERT_ALWAYS(inst->getOpInfo()->isInstTypeSameAs(inst->getOpInfo()->getInstType(),
                                               mavis::InstMetaData::InstructionTypes::FLOAT));
    ASSERT_ALWAYS(inst->getOpInfo()->isInstTypeSameAs(inst->getOpInfo()->getInstType()));

    // Issue #69
    mavis_facade.switchContext("BASE");
    inst = mavis_facade.makeInst(0x6f8c, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x6f8c = " << inst->dasmString() << endl;

    inst = mavis_facade.makeInst(0x01043823, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x01043823 = " << inst->dasmString()
         << endl;

    mavis::ExtractorDirectInfo ex_info_srai("srai", {1, 2}, {4});
    inst = mavis_facade.makeInstDirectly(ex_info_srai, 0);
    ASSERT_ALWAYS(inst->getuArchInfo()->isROBGrpStart()
           == true); // was false in the uarch files; overridden with annotations
    ASSERT_ALWAYS(inst->getuArchInfo()->isROBGrpEnd() == false);

    // LI x12, 0x0 (ADDI x12, x0, 0x0)
    inst = mavis_facade.makeInst(0x613, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x613 = " << inst->dasmString() << endl;

    // LI x12, 0x800 (ADDI, x12, x0, 0x800)
    inst = mavis_facade.makeInst(0x80000613, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x80000613 = " << inst->dasmString()
         << endl;

    // NOP (ADDI x0, x0, 0x0)
    inst = mavis_facade.makeInst(0x13, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x13 = " << inst->dasmString() << endl;

    // MV x12, x1 (ADDI x12, x1, 0x0)
    inst = mavis_facade.makeInst(0x8613, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x8613 = " << inst->dasmString() << endl;

    // ADDI x12, x1, 0x800
    inst = mavis_facade.makeInst(0x80008613, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x80008613 = " << inst->dasmString()
         << endl;

    // prefetch.i
    inst = mavis_facade.makeInst(0x6013, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x6013 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getImmediate() == 0);
    ASSERT_ALWAYS(inst->hasImmediate() == true);

    // ori x0, x0, 0x2
    inst = mavis_facade.makeInst(0x206013, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x6013 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getImmediate() == 2);
    ASSERT_ALWAYS(inst->hasImmediate() == true);

    // prefetch.r
    inst = mavis_facade.makeInst(0x106013, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x106013 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getImmediate() == 0);
    ASSERT_ALWAYS(inst->hasImmediate() == true);

    // prefetch.w x0, 0x1
    inst = mavis_facade.makeInst(0x2306013, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x306013 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getImmediate() == 1);
    ASSERT_ALWAYS(inst->hasImmediate() == true);

    // pause
    inst = mavis_facade.makeInst(0x0100000f, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x0100000f = " << inst->dasmString()
         << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "pause");

    // TAG testing
    mavis::MatchSet<mavis::Pattern> pset(std::vector<std::string>{"a", "a+", "[abc]"});
    mavis::MatchSet<mavis::Tag> tset(std::vector<std::string>{"aaa", "c"});
    ASSERT_ALWAYS(tset.matchAnyAny(pset));
    ASSERT_ALWAYS(!tset.matchAnyAll(pset));
    ASSERT_ALWAYS(tset.matchAllAny(pset));
    ASSERT_ALWAYS(!tset.matchAllAll(pset));

    // T0: Create a new context for testing pseudo instructions
    mavis_facade.makeContext("T0", {"uarch/isa_tagged.json"}, {}, {}, {}, {},
                             mavis::MatchSet<mavis::Pattern>(std::vector<std::string>{"pf"}));
    mavis_facade.switchContext("T0");
    cout << "====== TAG 'pf' EXCLUDED =========" << endl;
    // NOTE: prefetch instructions will NOT show in the trie dump (they are overlays of ORI)
    cout << mavis_facade;

    // prefetch.i should map to ori here (since pf tag excluded)
    inst = mavis_facade.makeInst(0x6013, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x6013 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "ori");
    ASSERT_ALWAYS(inst->getImmediate() == 0);
    ASSERT_ALWAYS(inst->hasImmediate() == true);
    ASSERT_ALWAYS(!inst->getTags().isMember("pf"));

    // T1: Create a new context for testing pseudo instructions
    mavis_facade.makeContext("T1", {"uarch/isa_tagged.json"}, {}, {}, {}, {},
                             mavis::MatchSet<mavis::Pattern>(std::vector<std::string>{"c.*"}));
    mavis_facade.switchContext("T1");
    cout << "====== TAG 'ccf' EXCLUDED =========" << endl;
    // NOTE: prefetch instructions will NOT show in the trie dump (they are overlays of ORI)
    cout << mavis_facade;

    // prefetch.i should map to prefetch.i here (since pf tag NOT excluded)
    inst = mavis_facade.makeInst(0x6013, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x6013 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "prefetch.i");
    ASSERT_ALWAYS(inst->getImmediate() == 0);
    ASSERT_ALWAYS(inst->hasImmediate() == true);
    ASSERT_ALWAYS(inst->getTags().isMember("pf"));

    // T2: Create a new context for testing pseudo instructions
    mavis_facade.makeContext("T2", {"uarch/isa_tagged.json"}, {}, {}, {},
                             mavis::MatchSet<mavis::Pattern>(std::vector<std::string>{"c.*"}), {});
    mavis_facade.switchContext("T2");
    cout << "====== TAG 'ccf' INCLUDED (ONLY) =========" << endl;
    // NOTE: prefetch instructions will NOT show in the trie dump (they are overlays of ORI)
    cout << mavis_facade;

    // prefetch.i should fail to decode (PREFETCH and ORI have been filtered out)
    try
    {
        inst = mavis_facade.makeInst(0x6013, 0);
        ASSERT_ALWAYS(inst == nullptr);
    }
    catch (const mavis::UnknownOpcode & ex)
    {
        cout << "line " << dec << __LINE__ << ": "
             << "DASM: 0x6013 fails to decode. This is expected" << endl;
    }

    // T3: Create a new context for testing pseudo instructions
    try
    {
        mavis_facade.makeContext("T3", {"uarch/isa_tagged.json"}, {}, {}, {},
                                 mavis::MatchSet<mavis::Pattern>(std::vector<std::string>{"zic.*"}),
                                 mavis::MatchSet<mavis::Pattern>(std::vector<std::string>{"c.*"}));
        ASSERT_ALWAYS(false);
    }
    catch (const mavis::BuildErrorOverlayBaseNotFound & ex)
    {
        cout << "line " << dec << __LINE__ << ": "
             << "Missing ORI definition during build. This is expected" << endl;
    }

    mavis_facade.switchContext("BASE");

    // prefetch.i should map to prefetch.i here (back to BASE context)
    inst = mavis_facade.makeInst(0x6013, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x6013 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "prefetch.i");
    ASSERT_ALWAYS(inst->getImmediate() == 0);
    ASSERT_ALWAYS(inst->hasImmediate() == true);
    ASSERT_ALWAYS(inst->getImmediate() == 0);
    ASSERT_ALWAYS(inst->hasImmediate() == true);

    cout << "====== TESTING MOP, CMOP, and zicfiss =========" << endl;

    mavis_facade.makeContext("MOP",
                             {"json/isa_rv64i.json",
                              "json/isa_rv64m.json",
                              "json/isa_rv64zmmul.json",
                              "json/isa_rv64f.json",
                              "json/isa_rv64d.json",
                              "json/isa_rv64zimop.json"},
                             {});
    mavis_facade.switchContext("MOP");

    const uint32_t mopr = 0x81c04073;
    for (uint32_t i = 0; i < 32; ++i) {
        const uint32_t mopi = mopr | (((i & 0b10000) >> 4) << 30) |
            (((i & 0b1100) >> 2) << 26) | ((i & 0b11) << 20);
        inst = mavis_facade.makeInst(mopi, 0);
        cout << "line " << dec << __LINE__ << ": " << "DASM: 0x"
             << hex << mopi << " = " << inst->dasmString()
             << endl;
    }

    mavis_facade.makeContext("CMOP",
                             {"json/isa_rv64i.json",
                              "json/isa_rv64m.json",
                              "json/isa_rv64zmmul.json",
                              "json/isa_rv64f.json",
                              "json/isa_rv64d.json",
                              "json/isa_rv64zimop.json",
                              "json/isa_rv64zcmop.json"},
                             {});
    mavis_facade.switchContext("CMOP");

    const uint32_t moprc = 0b0110000010000001;
    for (uint32_t i = 0; i < 8; ++i) {
        const uint32_t mopi = moprc | (i << 8);
        inst = mavis_facade.makeInst(mopi, 0);
        cout << "line " << dec << __LINE__ << ": " << "DASM: 0x"
             << hex << mopi << " = " << inst->dasmString()
             << endl;
        ASSERT_ALWAYS(inst->getMnemonic() == ("c.mop." + std::to_string((i << 1) + 1)));
    }

    mavis_facade.makeContext("MOP_zicfiss",
                             {"json/isa_rv64i.json",
                              "json/isa_rv64m.json",
                              "json/isa_rv64zmmul.json",
                              "json/isa_rv64f.json",
                              "json/isa_rv64d.json",
                              "json/isa_rv64zimop.json",
                              "json/isa_rv64zicfiss.json",
                              "json/isa_rv64zicfiss_common.json"},
                             {});
    mavis_facade.switchContext("MOP_zicfiss");

    // sspush x1
    inst = mavis_facade.makeInst(0xce104073, 0);
    ASSERT_ALWAYS(inst->getMnemonic() == "sspush");
    cout << "line " << dec << __LINE__ << ": "
         << "DASM: 0xce104073 = " << inst->dasmString() << endl;

    // sspush x5
    inst = mavis_facade.makeInst(0xce504073, 0);
    ASSERT_ALWAYS(inst->getMnemonic() == "sspush");
    cout << "line " << dec << __LINE__ << ": "
         << "DASM: 0xce504073 = " << inst->dasmString() << endl;

    // sspopchk x1
    inst = mavis_facade.makeInst(0xcdc0c073, 0);
    ASSERT_ALWAYS(inst->getMnemonic() == "sspopchk");
    cout << "line " << dec << __LINE__ << ": "
         << "DASM: 0xcdc0c073 = " << inst->dasmString() << endl;

    // sspopchk x5
    inst = mavis_facade.makeInst(0xcdc2c073, 0);
    ASSERT_ALWAYS(inst->getMnemonic() == "sspopchk");
    cout << "line " << dec << __LINE__ << ": "
         << "DASM: 0xcdc2c073 = " << inst->dasmString() << endl;

    // ssrdp
    inst = mavis_facade.makeInst(0xcdc54173, 0);
    ASSERT_ALWAYS(inst->getMnemonic() == "ssrdp");
    cout << "line " << dec << __LINE__ << ": "
         << "DASM: 0xcdc54173 = " << inst->dasmString() << endl;

    // ssamoswap.w
    inst = mavis_facade.makeInst(0x4800202f, 0);
    ASSERT_ALWAYS(inst->getMnemonic() == "ssamoswap.w");
    cout << "line " << dec << __LINE__ << ": "
         << "DASM: 0x4800202f = " << inst->dasmString() << endl;

    // ssamoswap.d
    inst = mavis_facade.makeInst(0x4800302f, 0);
    ASSERT_ALWAYS(inst->getMnemonic() == "ssamoswap.d");
    cout << "line " << dec << __LINE__ << ": "
         << "DASM: 0x4800302f = " << inst->dasmString() << endl;

    // c.sspush -- should fail
    try {
        inst = mavis_facade.makeInst(0x6081, 0);
        ASSERT_ALWAYS(!"c.sspush should have failed");
    }
    catch (...)
    {}

    mavis_facade.makeContext("MOP_zicfiss_c",
                             {"json/isa_rv64i.json",
                              "json/isa_rv64m.json",
                              "json/isa_rv64zmmul.json",
                              "json/isa_rv64f.json",
                              "json/isa_rv64d.json",
                              "json/isa_rv64zimop.json",
                              "json/isa_rv64zcmop.json",
                              "json/isa_rv64zca.json",
                              "json/isa_rv64zicfiss.json",
                              "json/isa_rv64zicfiss_c.json",
                              "json/isa_rv64zicfiss_common.json"},
                             {});
    mavis_facade.switchContext("MOP_zicfiss_c");

    // c.sspush
    inst = mavis_facade.makeInst(0x6081, 0);
    cout << "line " << dec << __LINE__ << ": "
         << "DASM: 0x6081 = " << inst->dasmString() << endl;

    // c.sspopchk
    inst = mavis_facade.makeInst(0x6281, 0);
    cout << "line " << dec << __LINE__ << ": "
         << "DASM: 0x6281 = " << inst->dasmString() << endl;

    cout << "====== TESTING RV32 =========" << endl;

    // RV32
    MavisType mavis_facade_rv32({"json/isa_rv32i.json",        // included in "g" spec
                                 "json/isa_rv32f.json",        // included in "g" spec
                                 "json/isa_rv32m.json",        // included in "g" spec
                                 "json/isa_rv32zmmul.json",    // included in "g" spec
                                 "json/isa_rv32zaamo.json",    // included in "g" spec
                                 "json/isa_rv32zalrsc.json",   // included in "g" spec
                                 "json/isa_rv32d.json",        // included in "g" spec
                                 "json/isa_rv32zicsr.json",    // included in "g" spec
                                 "json/isa_rv32zifencei.json", // included in "g" spec
                                 "json/isa_rv32q.json",           "json/isa_rv32zfa.json",
                                 "json/isa_rv32zfa_d.json",       "json/isa_rv32zfa_d_addons.json",
                                 "json/isa_rv32zfa_q.json",       "json/isa_rv32zfa_h.json",
                                 "json/isa_rv32zca.json",         "json/isa_rv32zcf.json",
                                 "json/isa_rv32zcd.json",         "json/isa_rv32zfh.json",
                                 "json/isa_rv32zfhmin.json",      "json/isa_rv32zfhmin_d.json",
                                 "json/isa_rv32zihintpause.json", "json/isa_rv32zawrs.json",
                                 "json/isa_rv32zilsd.json",       "json/isa_rv32zacas.json",
                                 "json/isa_rv32zabha.json",
                                 "json/isa_rv32zknd.json"
                                 },
                                {"uarch/uarch_rv32g.json"}, uid_init, anno_overrides);
    cout << mavis_facade_rv32;

    // add x1, x2, x3 (same in RV32 as RV64)
    inst = mavis_facade_rv32.makeInst(0x003100b3, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x003100b3 = " << inst->dasmString()
         << endl;

    //
    // Test 32-bit ld (pair) zilsd extension
    //
    inst = mavis_facade_rv32.makeInst(0x03103, 0);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x03103 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getIntDestRegs() == 0xCull); // 2 dests

    try
    {
        // Illegal form of load pair -- rd starts odd
        inst = mavis_facade_rv32.makeInst(0x03f83, 0);
        ASSERT_ALWAYS(inst == nullptr);
    }
    catch (...)
    {
    }

    //
    // Test 32-bit atomics zabha zacas extension
    //
    // Zabha testing
    // amoadd.b
    inst = mavis_facade_rv32.makeInst(0x006382af, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x006382af = " << inst->dasmString()
         << endl;

    // amoadd.h
    inst = mavis_facade_rv32.makeInst(0x006392af, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x006392af = " << inst->dasmString()
         << endl;

    // Zacas testing
    // amocas.w
    inst = mavis_facade_rv32.makeInst(0x2867322f, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x2867322f = " << inst->dasmString()
         << endl;

    // amocas.d
    inst = mavis_facade_rv32.makeInst(0x2863b22f, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x2863b22f = " << inst->dasmString()
         << endl;
    try
    {
        // Illegal form due to rd starts at an odd reg number
        inst = mavis_facade_rv32.makeInst(0x2863b2af, 0);
        ASSERT_ALWAYS(inst == nullptr);
    }
    catch (...)
    {
    }

    //
    // Test 32-bit st (pair) zilsd extension
    //
    inst = mavis_facade_rv32.makeInst(0x203023, 0);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x203023 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getIntSourceRegs() == 0xDull); // 3 sources, addr + rs1/rs2

    try
    {
        // Illegal form of store pair -- rs2 starts odd
        inst = mavis_facade_rv32.makeInst(0x503023, 0);
        ASSERT_ALWAYS(inst == nullptr);
    }
    catch (...)
    {
    }

    // c.jal is only available in RV32
    inst = mavis_facade_rv32.makeInst(0x2001, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x2001 = " << inst->dasmString() << endl;

    // srai x4, x3, 0x4 (RV64 shift amount is encoded in 6 bits, RV32 only uses 5)
    inst = mavis_facade_rv32.makeInst(0x4041d213, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x4041d213 = " << inst->dasmString()
         << endl;

    // When using zcf, 0x6008 should map to c.flw f10,x8, IMM=0
    inst = mavis_facade_rv32.makeInst(0x6008, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x6008 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "c.flw");

    // When using zcf, 0xe008 should map to c.fsw f10,x8, IMM=0
    inst = mavis_facade_rv32.makeInst(0xe008, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xe008 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "c.fsw");

    runTSet(mavis_facade_rv32, "rv32_zknd.tset");

    // "ZCLSD" context should exist yet
    ASSERT_ALWAYS(mavis_facade_rv32.hasContext("ZCLSD") == false);

    cout << "====== TESTING RV32 Zclsd =========" << endl;

    // Create new context to test Zclsd extension
    // Zclsd has overlapping encodings with Zcf, so they can't be used at the same time
    mavis_facade_rv32.makeContext("ZCLSD",
                                  {"json/isa_rv32i.json",        // included in "g" spec
                                   "json/isa_rv32f.json",        // included in "g" spec
                                   "json/isa_rv32m.json",        // included in "g" spec
                                   "json/isa_rv32zmmul.json",    // included in "g" spec
                                   "json/isa_rv32zaamo.json",    // included in "g" spec
                                   "json/isa_rv32zalrsc.json",   // included in "g" spec
                                   "json/isa_rv32d.json",        // included in "g" spec
                                   "json/isa_rv32zicsr.json",    // included in "g" spec
                                   "json/isa_rv32zifencei.json", // included in "g" spec
                                   "json/isa_rv32zfa.json", "json/isa_rv32zfa_d.json",
                                   "json/isa_rv32zfa_d_addons.json", "json/isa_rv32zca.json",
                                   "json/isa_rv32zcd.json", "json/isa_rv32zihintpause.json",
                                   "json/isa_rv32zilsd.json", "json/isa_rv32zclsd.json"},
                                  {"uarch/uarch_rv32g.json"});
    mavis_facade_rv32.switchContext("ZCLSD");
    cout << mavis_facade_rv32;

    // When using zclsd, 0x6008 should map to c.ld x10,x8, IMM=0
    inst = mavis_facade_rv32.makeInst(0x6008, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x6008 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "c.ld");
    ASSERT_ALWAYS(inst->getIntDestRegs() == 0xC00ull); // 2 dests

    try
    {
        // Illegal form of load pair -- rd starts odd
        inst = mavis_facade_rv32.makeInst(0x6082, 0);
        ASSERT_ALWAYS(inst == nullptr);
    }
    catch (...)
    {
    }

    // When using zclsd, 0xe008 should map to c.sd x10,x8, IMM=0
    inst = mavis_facade_rv32.makeInst(0xe008, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xe008 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "c.sd");
    ASSERT_ALWAYS(inst->getIntSourceRegs() == 0xD00ull); // 3 sources

    try
    {
        // Illegal form of store pair -- rs2 starts odd
        inst = mavis_facade_rv32.makeInst(0xe004, 0);
        ASSERT_ALWAYS(inst == nullptr);
    }
    catch (...)
    {
    }

    cout << "====== TESTING RV32 Zcmp/Zcmt =========" << endl;

    // RV32
    mavis_facade_rv32.makeContext("ZCMP_ZCMT",
                                  {"json/isa_rv32i.json",        // included in "g" spec
                                   "json/isa_rv32f.json",        // included in "g" spec
                                   "json/isa_rv32m.json",        // included in "g" spec
                                   "json/isa_rv32zmmul.json",    // included in "g" spec
                                   "json/isa_rv32zaamo.json",    // included in "g" spec
                                   "json/isa_rv32zalrsc.json",   // included in "g" spec
                                   "json/isa_rv32d.json",        // included in "g" spec
                                   "json/isa_rv32zicsr.json",    // included in "g" spec
                                   "json/isa_rv32zifencei.json", // included in "g" spec
                                   "json/isa_rv32q.json",           "json/isa_rv32zfa.json",
                                   "json/isa_rv32zfa_d.json",       "json/isa_rv32zfa_d_addons.json",
                                   "json/isa_rv32zfa_q.json",       "json/isa_rv32zfa_h.json",
                                   "json/isa_rv32zca.json",         "json/isa_rv32zcf.json",
                                   "json/isa_rv32zcmp.json",        "json/isa_rv32zcmt.json",
                                   "json/isa_rv32zfh.json",         "json/isa_rv32zfhmin.json",
                                   "json/isa_rv32zfhmin_d.json",    "json/isa_rv32zihintpause.json",
                                   "json/isa_rv32zawrs.json",       "json/isa_rv32zilsd.json",
                                   "json/isa_rv32zacas.json",       "json/isa_rv32zabha.json"},
                                  {"uarch/uarch_rv32g.json"}, uid_init, anno_overrides);
    mavis_facade_rv32.switchContext("ZCMP_ZCMT");
    cout << mavis_facade_rv32;

    // 0xb856 should map to cm.push  {x1, x8}, -32
    inst = mavis_facade_rv32.makeInst(0xb856, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xb856 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "cm.push");
    ASSERT_ALWAYS(inst->getIntDestRegs() == 0x4ull); // 1 dest
    ASSERT_ALWAYS(std::bit_cast<int64_t>(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::STACK_ADJ)) == -32); // Should have stack_adj SF

    try
    {
        // Illegal form of cm.push -- urlist is 0
        inst = mavis_facade_rv32.makeInst(0xb806, 0);
        ASSERT_ALWAYS(inst == nullptr);
    }
    catch (...)
    {
    }

    // 0xb856 should map to cm.popret        {x1, x8}, 16
    inst = mavis_facade_rv32.makeInst(0xbe52, 0);
    ASSERT_ALWAYS(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xbe52 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "cm.popret");
    ASSERT_ALWAYS(inst->getIntDestRegs() == 0x106ull); // 3 dests

    //
    // Test 32-bit Zcmt extension (cm.jt instruction)
    //
    inst = mavis_facade_rv32.makeInst(0xa002, 0);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xa002 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "cm.jt");
    ASSERT_ALWAYS(inst->getImmediate() == 0x0ull);

    //
    // Test 32-bit Zcmt extension (cm.jalt instruction)
    //
    inst = mavis_facade_rv32.makeInst(0xa082, 0);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xa082 = " << inst->dasmString() << endl;
    ASSERT_ALWAYS(inst->getMnemonic() == "cm.jt");
    ASSERT_ALWAYS(inst->getIntDestRegs() == 0x2ull);
    ASSERT_ALWAYS(inst->getImmediate() == 32ull);

    // These opcode variants have caused issues in the past
    try {
        inst = mavis_facade_rv32.makeInst(0xac22, 0);
        ASSERT_ALWAYS(inst == nullptr);
    }
    catch (...) {
        cout << "line " << dec << __LINE__ << ": DASM: 0xac22 is illegal " << endl;
    }

    // cm.mvsa01
    inst = mavis_facade_rv32.makeInst(0xad26, 0);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xad26 = " << inst->dasmString() << endl;
    inst = mavis_facade_rv32.makeInst(0xac62, 0);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xac62 = " << inst->dasmString() << endl;

    return 0;
}
