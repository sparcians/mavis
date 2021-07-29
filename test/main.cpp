#include <iostream>
#include "Mavis.h"
#include "Inst.h"
#include "uArchInfo.h"

// For custom extraction
#include "ExtractorDirectImplementations.hpp"

using namespace std;
using json = nlohmann::json;

struct ExampleTraceInfo
{
    std::string     mnemonic;
    uint64_t        opcode;

    const std::string& getMnemonic() const
    {
        return mnemonic;
    }

    uint64_t getOpcode() const
    {
        return opcode;
    }

    uint64_t getFunction() const
    {
        return 0;
    }

    uint64_t getSourceRegs() const
    {
        return 0;
    }

    uint64_t getDestRegs() const
    {
        return 0;
    }

    uint64_t getImmediate() const
    {
        return 0;
    }
};

using MavisType = Mavis<Instruction<uArchInfo>, uArchInfo>;

void runTSet(MavisType& mavis_facade, const std::string& tfile) {
    // Run a test file
    ifstream    tin(tfile);

    while (tin) {
        string  line;
        tin >>  line;
        if (tin) {
            char        mnestr[100];
            long long   icode;
            // Dave and I are in a silent fight here.  llx or lx?  gcc
            // says lx for uint64_t types.  clang doesn't seem to
            // care.  I say lx, Dave says llx.  If we force icode to
            // be a long long, everyone is happy as long as the sizes
            // are the same.
            static_assert(sizeof(long long) == sizeof(uint64_t));
            sscanf(line.c_str(), "%[^,],%llx", mnestr, &icode);

            // uint64_t icode = stoll(hexstr, nullptr, 16);
            // Instruction*     iptr = dt.makeInst(icode);
            ExampleTraceInfo  tinfo;
            tinfo.mnemonic = std::string(mnestr);
            tinfo.opcode = icode;

            Instruction<uArchInfo>::PtrType     iptr = nullptr;
            try {
                iptr = mavis_facade.makeInstFromTrace(tinfo, 0);
            } catch (const mavis::IllegalOpcode& ex) {
                cout << ex.what() << endl;
            } catch (const mavis::UnknownOpcode& ex) {
                cout << ex.what() << endl;
            }

            if (iptr != nullptr) {
                cout << "HEX: 0x" << hex << icode
                     << ", INST: " << *iptr
                     << ", DASM: " << iptr->dasmString()
                     << endl;
                if (iptr->getMnemonic() != std::string(mnestr)) {
                    cout << "WARNING: expected '" << mnestr << "'"
                         << ", decoded '" << iptr->getMnemonic() << "'"
                         << endl;
                }

                // Check getInfo...
                MavisType::DecodeInfoType  optr = mavis_facade.getInfo(tinfo.getOpcode());
                assert(optr != nullptr);
                if (optr->opinfo->getMnemonic() != iptr->getMnemonic()) {
                    cout << "OPCODE INFO WARNING: expected '" << iptr->getMnemonic() << "'"
                         << ", retrieved '" << optr->opinfo->getMnemonic() << "'"
                         << endl;
                }

                // Check UID...
                const mavis::InstructionUniqueID expected_id =
                    mavis_facade.lookupInstructionUniqueID(iptr->getMnemonic());
                if (optr->opinfo->getInstructionUniqueID() != expected_id) {
                    cout << "INSTRUCTION ID WARNING: expected ID=" << std::dec << expected_id
                         << ", got ID=" << optr->opinfo->getInstructionUniqueID()
                         << endl;
                }
            }

            // Check direct info extraction
            try {
                mavis::ExtractorDirectInfo ex_info_A(tinfo.mnemonic, {1, 2}, {3});
                Instruction<uArchInfo>::PtrType iptr_A = mavis_facade.makeInstDirectly(ex_info_A, 0);
                assert(iptr_A != nullptr);

                const mavis::InstructionUniqueID uid = mavis_facade.lookupInstructionUniqueID(tinfo.mnemonic);
                mavis::ExtractorDirectInfo ex_info_B(uid, {1, 2}, {3});
                Instruction<uArchInfo>::PtrType iptr_B = mavis_facade.makeInstDirectly(ex_info_B, 0);
                assert(iptr_B != nullptr);

                assert(iptr_A->getUID() == iptr_B->getUID());
                assert(iptr_A->getMnemonic() == iptr_B->getMnemonic());
            } catch (const mavis::UnknownMnemonic& ex) {
                // We currently don't support overlays (mv) and compressed instructions
                // in ExtractorDirectInfo...
                cout << ex.what() << endl;
            }
        }
    }
    tin.close();
}

int main() {
    constexpr mavis::InstructionUniqueID NOP_UID = 1;

    // UID initialization list for testing NOP and C.NOP UID's against NOP_UID
    mavis::InstUIDList uid_init {
        { "nop", NOP_UID},
        { "cmov", 2}
    };

    mavis::AnnotationOverrides anno_overrides =
        {
            std::make_pair("andn", "pipelined:false"),
            std::make_pair("srai", "rob_group:[\"begin\"]"),
        };

    MavisType mavis_facade({"../json/isa_rv64g.json",
                                    "../json/isa_rv64c.json",
                                    "../json/isa_rv64cf.json",
                                    "../json/isa_rv64cd.json",
                                    "../json/isa_rv64q.json",
                                    "../json/isa_rv64v.json",
                                    "../json/isa_rv64vf.json",
                                    "../json/isa_rv64zvamo.json",
                                    "../json/isa_rv64xsfvqmaccqoq.json",
                                    "../json/isa_rv64xsfvqmaccdod.json",
                                    "../json/isa_rv64xsfvfhbfmin.json",
                                    "../json/isa_rv64zfh.json",
                                    "../json/isa_rv64zfh_d.json"
                                    },
            {"mallard_uarch_rv64g.json", "mallard_uarch_rv64g.json"
             // "mallard_uarch_rv64c.json",
             // "mallard_uarch_rv64c.json",
             // "mallard_uarch_rv64v.json"
            },
        uid_init,
        anno_overrides);
    cout << mavis_facade;

    Instruction<uArchInfo>::PtrType inst = nullptr;
    try {
        inst = mavis_facade.makeInst(0x0, 0);
    } catch (const mavis::IllegalOpcode& ex) {
        cout << ex.what() << endl;
    }

    if (inst != nullptr) {
        cout << "DASM: 0x0 = " << inst->dasmString() << endl;
    }

    runTSet(mavis_facade, "rv64.tset");

    // Exercise the cache
    runTSet(mavis_facade, "rv64.tset");
    mavis_facade.flushCaches();
    runTSet(mavis_facade, "rv64.tset");

    // "NEW" context should haven't been existed
    assert(mavis_facade.hasContext("NEW") == false);

    // Try creating a new context
    mavis_facade.makeContext("NEW", {"../json/isa_rv64i.json",
                                     "../json/isa_rv64m.json",
                                     "../json/isa_rv64a.json",
                                     "../json/isa_rv64b.json",
                                     "../json/isa_rv64f.json",
                                     "../json/isa_rv64d.json",
                                     "../json/isa_rv64c.json",
                                     "../json/isa_rv64cf.json",
                                     "../json/isa_rv64cd.json",
                                     "../json/isa_rv64v.json",
                                     "../json/isa_rv64vf.json",
                                     "../json/isa_rv64zvamo.json",
                                     "../json/isa_rv64xsfvqmaccqoq.json",
                                     "../json/isa_rv64xsfvqmaccdod.json",
                                     "../json/isa_rv64zfh.json",
                                     "../json/isa_rv64zfh_d.json"}, {});
    mavis_facade.switchContext("NEW");
    cout << mavis_facade;
    runTSet(mavis_facade, "rv64.tset");
    runTSet(mavis_facade, "rv64_bits.tset");

    // Switch back to BASE context
    mavis_facade.switchContext("BASE");

    // 0x003100b3 = add	1,2,3
    inst = mavis_facade.makeInst(0x003100b3, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x003100b3 = " << inst->dasmString() << endl;

    // 0x02028593 = addi	11,5, IMM=0x20
    inst = mavis_facade.makeInst(0x02028593, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x02028593 = " << inst->dasmString() << endl;

    // 0x02028593 = mv	11,5
    inst = mavis_facade.makeInst(0x00028593, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x00028593 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Has Immediate? " << (inst->hasImmediate() ? "YES" : "no") << endl;

    // 0x4081 = c.li	1, IMM=0x0
    inst = mavis_facade.makeInst(0x4081, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x4081 = " << inst->dasmString() << endl;

    // 0x000280e7 = jalr	1,5, IMM=0x0
    inst = mavis_facade.makeInst(0x000280e7, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x000280e7 = " << inst->dasmString() << endl;
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::BRANCH));
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::JALR));
    assert(inst->isExtInstType(mavis::DecodedInstructionInfo::ExtractedInstTypes::CALL));
    assert(inst->isExtInstType(mavis::DecodedInstructionInfo::ExtractedInstTypes::RETURN));

    // 0xe152 = c.sdsp	20, SP, IMM=0x0
    inst = mavis_facade.makeInst(0xe152, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xe152 = " << inst->dasmString() << endl;

    // 0xfcd6 = c.sdsp	21, SP, IMM=0x0
    inst = mavis_facade.makeInst(0xfcd6, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xfcd6 = " << inst->dasmString() << endl;

    // 0xf1402573 = csrrs	10,0, CSR=0xf14
    inst = mavis_facade.makeInst(0xf1402573, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xf1402573 = " << inst->dasmString() << endl;

    mavis::ExtractorDirectInfo ex_info ("add", {1, 2}, {3});
    inst = mavis_facade.makeInstDirectly(ex_info, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DIRECT: 'add' = " << inst->dasmString() << endl;

    mavis::ExtractorDirectInfoBitMask exbm_info("add", 0x6, 0x8);
    inst = mavis_facade.makeInstDirectly(exbm_info, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DIRECT_BM: 'add' = " << inst->dasmString() << endl;

    mavis::ExtractorDirectInfo_Stores sx_info("sw", {1}, {2});
    inst = mavis_facade.makeInstDirectly(sx_info, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DIRECT: 'sw' = " << inst->dasmString() << endl;

    mavis::ExtractorDirectInfoBitMask_Stores sxbm_info("sw", 0x2, 0x4);
    inst = mavis_facade.makeInstDirectly(sxbm_info, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DIRECT_BM: 'sw' = " << inst->dasmString() << endl;

    // 0x907405e3 = beq	8,7 +0xfffffffffffff90a
    inst = mavis_facade.makeInst(0x907405e3, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x907405e3 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex << inst->getSignedOffset() << std::dec << endl;

    // 0x107405e3 = beq	8,7 +0x90a
    inst = mavis_facade.makeInst(0x107405e3, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x107405e3 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex << inst->getSignedOffset() << std::dec << endl;

    // 0xd3ad = c.beqz	15, +0xffffffffffffff62
    inst = mavis_facade.makeInst(0xd3ad, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xd3ad = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex << inst->getSignedOffset() << std::dec << endl;

    // 0xc3ad = c.beqz	15, +0x62
    inst = mavis_facade.makeInst(0xc3ad, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xc3ad = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex << inst->getSignedOffset() << std::dec << endl;

    // 0x8f16c3ef = jal	7, +0xfffffffffff6c8f0
    inst = mavis_facade.makeInst(0x8f16c3ef, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x8f16c3ef = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex << inst->getSignedOffset() << std::dec << endl;

    // 0x0f16c3ef = jal	7, +0x6c8f0
    inst = mavis_facade.makeInst(0x0f16c3ef, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x0f16c3ef = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex << inst->getSignedOffset() << std::dec << endl;

    // 0xb555 = c.j	 +0xfffffffffffffea4
    inst = mavis_facade.makeInst(0xb555, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xb555 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex << inst->getSignedOffset() << std::dec << endl;

    // 0xa555 = c.j	 +0x6a4
    inst = mavis_facade.makeInst(0xa555, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xa555 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex << inst->getSignedOffset() << std::dec << endl;

    // 0xa9cc0767 = jalr	14,24, IMM=0xfffffffffffffa9c
    inst = mavis_facade.makeInst(0xa9cc0767, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xa9cc0767 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Signed-offset: 0x" << std::hex << inst->getSignedOffset() << std::dec << endl;

    // 0xbe10afa3 = sw	1,1
    inst = mavis_facade.makeInst(0xbe10afa3, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xbe10afa3 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs() << endl;

    mavis::Opcode jalr_stencil = mavis_facade.getOpcode("jalr");
    cout << "line " << dec << __LINE__ << ": " << "Stencil for 'jalr' = 0x" << hex << jalr_stencil << dec << endl;
    inst = mavis_facade.makeInst(jalr_stencil, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x" << hex << jalr_stencil << dec << " = " << inst->dasmString() << endl;

    // 0x60e2 = c.ldsp	1, SP, IMM=0x0
    inst = mavis_facade.makeInst(0x60e2, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x60e2 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Has Immediate? " << (inst->hasImmediate() ? "YES" : "no") << endl;

    // fld       f0,0(x10)
    inst = mavis_facade.makeInst(0x53007, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x53007 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs() << endl;

    // c.fld     f8,64(x10)
    inst = mavis_facade.makeInst(0x2120, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x2120 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs() << endl;

    // mret	0,0,0
    inst = mavis_facade.makeInst(0x30200073, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x30200073 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs() << endl;

    // lr.w 0,0
    inst = mavis_facade.makeInst(0x1000202f, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x1000202f = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs() << endl;

    // c.fld     f10,80(x10)
    inst = mavis_facade.makeInst(0x2928, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x2928 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Int-Sources: " << inst->getIntSourceRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Float-Sources: " << inst->getFloatSourceRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Int-Dests: " << inst->getIntDestRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Float-Dests: " << inst->getFloatDestRegs() << endl;

    // c.fld     f11,88(x10)
    inst = mavis_facade.makeInst(0x2d2c, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x2d2c = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Int-Sources: " << inst->getIntSourceRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Float-Sources: " << inst->getFloatSourceRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Int-Dests: " << inst->getIntDestRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Float-Dests: " << inst->getFloatDestRegs() << endl;

    // 0x72a7f543 fmadd.d  f10 = f15, f10, f14
    inst = mavis_facade.makeInst(0x72a7f543, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x72a7f543 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "fmadd.d RM field = 0x"
         << hex << inst->getSpecialField(mavis::ExtractorIF::SpecialField::RM)
         << dec << endl;

    // Operand types for fcvt.l.d
    mavis::ExtractorDirectInfo ex_info_fcvt ("fcvt.l.d", {1}, {4});
    inst = mavis_facade.makeInstDirectly(ex_info_fcvt, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DIRECT: 'fcvt.l.d' = " << inst->dasmString() << endl;

    // Try Mavis::isOpcodeInfo and Mavis::isOpcodeExtractedInfo
    // 0x000280e7 = jalr	1,5, IMM=0x0
    assert(mavis_facade.isOpcodeInstType(0x000280e7, MavisType::InstructionType::BRANCH));
    assert(mavis_facade.isOpcodeExtractedInstType(0x000280e7, MavisType::ExtractedInstType::CALL));

    //  0x8006 c.mv
    inst = mavis_facade.makeInst(0x8006, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x8006 = " << inst->dasmString() << endl;

    mavis::ExtractorDirectInfo cmov_info ("cmov", {1,2,3}, {4});
    mavis_facade.morphInst(inst, cmov_info);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "MORPH DASM: = " << inst->dasmString() << endl;
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::INT));
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::MOVE));
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::CONDITIONAL));

    // NOP (addi x0, x0, 0)
    inst = mavis_facade.makeInst(mavis::CANONICAL_NOP, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM (CANONICAL_NOP): = " << inst->dasmString() << endl;
    assert(inst->getUID() == NOP_UID);

    // C.NOP (c.addi x0, x0, 0)
    inst = mavis_facade.makeInst(0x1, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM (C.NOP): = " << inst->dasmString() << endl;
    assert(inst->getUID() == NOP_UID);

    // Operand types for floating point compares
    mavis::ExtractorDirectInfo ex_info_feq ("feq.s", {1,2}, {4});
    inst = mavis_facade.makeInstDirectly(ex_info_feq, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DIRECT: 'feq.s' = " << inst->dasmString() << endl;

    //  0x710d c.addi16sp 2, 0x2A0  (0xF...FEA0 when sign extended)
    inst = mavis_facade.makeInst(0x710d, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x710d = " << inst->dasmString() << endl;

    //  0x5769 c.li x14, 0x3A  (0xF...FA when sign extended)
    inst = mavis_facade.makeInst(0x5769, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x5769 = " << inst->dasmString() << endl;

    //  0x7769 c.lui x14, 0x3A000  (0xF...FA000 when sign extended)
    inst = mavis_facade.makeInst(0x7769, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x7769 = " << inst->dasmString() << endl;

    //  0x177c c.addi4spn x15, 0x3AC
    inst = mavis_facade.makeInst(0x177c, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x177c = " << inst->dasmString() << endl;

    //  0x0063b2af amoadd.d 5,7,6, aq=0, rl=0
    inst = mavis_facade.makeInst(0x0063b2af, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x0063b2af = " << inst->dasmString() << endl;

    //  0x8516 c.mv rd, 5 --> add rd, rs1 = x0, rs2 = 5 (must preserve x0)
    inst = mavis_facade.makeInst(0x8516, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x8516 = " << inst->dasmString() << endl;

    //  0xe3c1 c.bnez 15, 0, +0x80 (must preserve x0)
    inst = mavis_facade.makeInst(0xe3c1, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xe3c1 = " << inst->dasmString() << endl;

    //  0x9696 c.add rD, rS --> add rD, rD, rS
    inst = mavis_facade.makeInst(0x9696, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x9696 = " << inst->dasmString() << endl;

    //  0x7FF07057 vsetvli 0, 0, 0x7FF
    inst = mavis_facade.makeInst(0x7FF07057, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x7ff07057 = " << inst->dasmString() << endl;
    assert(inst->getVectorSourceRegs() == 0ull);
    assert(inst->getVectorDestRegs() == 0ull);
    assert(inst->getIntSourceRegs() == 1ull);
    assert(inst->getIntDestRegs() == 1ull);

    // 0x803170D7 vsetvl, x1, x2, x3
    inst = mavis_facade.makeInst(0x803170D7, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x803170d7 = " << inst->dasmString() << endl;
    assert(inst->getVectorSourceRegs() == 0ull);
    assert(inst->getVectorDestRegs() == 0ull);
    assert(inst->getIntSourceRegs() == 12ull);
    assert(inst->getIntDestRegs() == 2ull);

    // 0x2f007, vle64, v0, x5
    inst = mavis_facade.makeInst(0x2f007, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x2f007 = " << inst->dasmString() << endl;
    assert(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) == 0ull);
    assert(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 0ull);
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);

    // 0x102f007, vle64ff, v0, x5
    inst = mavis_facade.makeInst(0x102f007, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x102f007 = " << inst->dasmString() << endl;
    assert(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) == 0ull);
    assert(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 0ull);
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::FAULTFIRST) == 1);

    // 0x202f007, vle64, v0, x5, v0.t
    inst = mavis_facade.makeInst(0x202f007, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x202f007 = " << inst->dasmString() << endl;
    assert(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) == 1ull);
    assert(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 0ull);
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);

    // vloxei8.v,0xc000007
    inst = mavis_facade.makeInst(0xc000007, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xc000007 = " << inst->dasmString() << endl;
    assert(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 0ull);
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);

    // vsuxei8.v,0x4000027
    inst = mavis_facade.makeInst(0x4000027, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x4000027 = " << inst->dasmString() << endl;
    assert(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::NF) == 0ull);

    // vadd.vi v31, v31, 0x1F (-1)
    inst = mavis_facade.makeInst(0x03ffbfd7, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x03ffbfd7 = " << inst->dasmString() << endl;
    assert(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) == 1ull);
    assert(inst->getSignedOffset() == -1);

    // vadd.vv v1, v2, v3, 0x3100D7
    inst = mavis_facade.makeInst(0x3100D7, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x3100D7 = " << inst->dasmString() << endl;
    assert(inst->getVectorSourceRegs() == 0b1100);
    assert(inst->getVectorDestRegs() == 0b10);
    assert(inst->getDestOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RD));
    assert(inst->getDestOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RD) == 1);
    assert(inst->getDestOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RD) == mavis::OpcodeInfo::OperandTypes::VECTOR);
    assert(inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RS1));
    assert(inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RS1) == 2);
    assert(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS1) == mavis::OpcodeInfo::OperandTypes::VECTOR);
    assert(inst->getSourceOpInfo().hasFieldID(mavis::InstMetaData::OperandFieldID::RS2));
    assert(inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RS2) == 3);
    assert(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2) == mavis::OpcodeInfo::OperandTypes::VECTOR);
    try {
        inst->getSourceOpInfo().getFieldValue(mavis::InstMetaData::OperandFieldID::RS3);
    } catch (const mavis::OperandInfoInvalidFieldID& ex) {
        cout << "line " << dec << __LINE__ << ": " << ex.what() << endl;
    }

    // vadc.vvm v1, v2, v3, 0x403100D7
    inst = mavis_facade.makeInst(0x403100D7, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x403100D7 = " << inst->dasmString() << endl;
    assert(inst->getVectorSourceRegs() == 0b1100);
    assert(inst->getVectorDestRegs() == 0b10);
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::MASK) == 0);

    // vmv.v.v v2, v1, 0x5E008157
    inst = mavis_facade.makeInst(0x5E008157, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x5E008157 = " << inst->dasmString() << endl;
    assert(inst->getVectorSourceRegs() == 0b10);
    assert(inst->getVectorDestRegs() == 0b100);

    // vadd.vx v1, v3, x2, 0x3140D7
    inst = mavis_facade.makeInst(0x3140D7, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x3140D7 = " << inst->dasmString() << endl;
    assert(inst->getIntSourceRegs() == 0b100);
    assert(inst->getVectorSourceRegs() == 0b1000);
    assert(inst->getVectorDestRegs() == 0b10);

    // vadc.vxm v1, v3, x2, 0x403140D7
    inst = mavis_facade.makeInst(0x403140D7, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x403140D7 = " << inst->dasmString() << endl;
    assert(inst->getIntSourceRegs() == 0b100);
    assert(inst->getVectorSourceRegs() == 0b1000);
    assert(inst->getVectorDestRegs() == 0b10);

    // vmv.v.x v2, x1, 0x5E00C157
    inst = mavis_facade.makeInst(0x5E00C157, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x5E00C157 = " << inst->dasmString() << endl;
    assert(inst->getIntSourceRegs() == 0b10);
    assert(inst->getVectorDestRegs() == 0b100);

    // vmv1r.v v1, v2, 0x9E2030D7
    inst = mavis_facade.makeInst(0x9E2030D7, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 9E2030D7 = " << inst->dasmString() << endl;
    assert(inst->getVectorSourceRegs() == 0b100);
    assert(inst->getVectorDestRegs() == 0b10);

    // vid.v v1, 0x5008A0D7
    inst = mavis_facade.makeInst(0x5008A0D7, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 5008A0D7 = " << inst->dasmString() << endl;
    assert(inst->getVectorSourceRegs() == 0);
    assert(inst->getVectorDestRegs() == 0b10);

    // vse8.v v1, x2 0x100a7
    inst = mavis_facade.makeInst(0x100a7, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 100a7 = " << inst->dasmString() << endl;
    assert(inst->getIntSourceRegs() == 0b100);
    assert(inst->getVectorSourceRegs() == 0b10);
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 1);

    // 0x6032D0AF, vamoandei16.v r5, v3, v1
    inst = mavis_facade.makeInst(0x6032D0AF, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x6032D0AF = " << inst->dasmString() << endl;
    assert(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) == 0ull);
    assert(inst->getSpecialField(mavis::OpcodeInfo::SpecialField::WD) == 0ull);
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::ATOMIC) == 1);
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::INDEXED) == 1);
    assert(inst->isInstType(mavis::InstMetaData::InstructionTypes::SEGMENT) == 0);

    // Create a new context for RV32
    mavis_facade.makeContext("RV32", {"../json/isa_rv32i.json",
                                      "../json/isa_rv32m.json",
                                      "../json/isa_rv32a.json",
                                      "../json/isa_rv32f.json",
                                      "../json/isa_rv32d.json",
                                      "../json/isa_rv32c.json",
                                      "../json/isa_rv32cf.json",
                                      "../json/isa_rv32cd.json",}, {});
    mavis_facade.switchContext("RV32");
    cout << "line " << dec << __LINE__ << ": " << mavis_facade;

    // c.fld     f8,64(x10)
    inst = mavis_facade.makeInst(0x2120, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x53007 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs() << endl;

    // 0x40e2 = c.lwsp	1, SP, IMM=0x18
    inst = mavis_facade.makeInst(0x40e2, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x40e2 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Has Immediate? " << (inst->hasImmediate() ? "YES" : "no") << endl;

    // 0x60e2 = c.flwsp	1, SP, IMM=0x18
    inst = mavis_facade.makeInst(0x60e2, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x60e2 = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "A-Sources: " << inst->getSourceAddressRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "D-Sources: " << inst->getSourceDataRegs() << endl;
    cout << "line " << dec << __LINE__ << ": " << "Has Immediate? " << (inst->hasImmediate() ? "YES" : "no") << endl;

    // 0x650d = c.lui	10, x3
    inst = mavis_facade.makeInst(0x650d, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x650d = " << inst->dasmString() << endl;

    // 0x12000073 = sfence
    inst = mavis_facade.makeInst(0x12000073, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x12000073 = " << inst->dasmString() << endl;

    // 0xa422 = c.fsdsp	8, SP, IMM=0x8
    inst = mavis_facade.makeInst(0xa422, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0xa422 = " << inst->dasmString() << endl;

    // Create a new context for testing pseudo instructions
    mavis_facade.makeContext("PSEUDO", {"../json/isa_rv64i.json", "isa_pseudo.json"},
                             {"mallard_uarch_rv64g.json", "mallard_uarch_pseudo.json"});
    mavis_facade.switchContext("PSEUDO");

    mavis::ExtractorPseudoInfo pseudo_op
        ("P0",
         {{mavis::InstMetaData::OperandFieldID::RS1, mavis::InstMetaData::OperandTypes::WORD, 1},
          {mavis::InstMetaData::OperandFieldID::RS2, mavis::InstMetaData::OperandTypes::DOUBLE, 2}},
         {{mavis::InstMetaData::OperandFieldID::RD,  mavis::InstMetaData::OperandTypes::WORD, 3}});
    inst = mavis_facade.makePseudoInst(pseudo_op, 0);
    cout << "line " << dec << __LINE__ << ": " << "PSEUDO = " << inst->dasmString() << endl;

    mavis::ExtractorDirectInfo di_pseudo_op("P0", { 1, 2 }, { 3 });
    inst = mavis_facade.makePseudoInst(di_pseudo_op, 0);
    cout << "line " << dec << __LINE__ << ": " << "PSEUDO = " << inst->dasmString() << endl;
    assert(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS1) == mavis::OpcodeInfo::OperandTypes::WORD);
    assert(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2) == mavis::OpcodeInfo::OperandTypes::DOUBLE);
    assert(inst->getDestOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RD) == mavis::OpcodeInfo::OperandTypes::WORD);

    mavis::ExtractorDirectInfoBitMask dibm_pseudo_op("P0", (0x1 << 1) | (0x1 << 2), 0x1 << 3, 0xDEAD);
    inst = mavis_facade.makePseudoInst(dibm_pseudo_op, 0);
    cout << "line " << dec << __LINE__ << ": " << "PSEUDO = " << inst->dasmString() << endl;
    assert(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS1) == mavis::OpcodeInfo::OperandTypes::WORD);
    assert(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2) == mavis::OpcodeInfo::OperandTypes::DOUBLE);
    assert(inst->getDestOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RD) == mavis::OpcodeInfo::OperandTypes::WORD);

    mavis::ExtractorDirectInfo_Stores di_pseudo_op_p1("P1", { 1 }, { 2 }, mavis::ExtractorIF::ValueListType({ 3 }));
    inst = mavis_facade.makePseudoInst(di_pseudo_op_p1, 0);
    cout << "line " << dec << __LINE__ << ": " << "PSEUDO = " << inst->dasmString() << endl;
    cout << "line " << dec << __LINE__ << ": " << "VM = " << inst->getSpecialField(mavis::OpcodeInfo::SpecialField::VM) << endl;
    assert(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS1) == mavis::OpcodeInfo::OperandTypes::LONG);
    assert(inst->getSourceOpInfo().isStoreData(mavis::InstMetaData::OperandFieldID::RS1) == false);
    assert(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2) == mavis::OpcodeInfo::OperandTypes::LONG);
    assert(inst->getSourceOpInfo().isStoreData(mavis::InstMetaData::OperandFieldID::RS2) == true);
    try {
        inst->getSpecialField(mavis::OpcodeInfo::SpecialField::AVL);
        assert(false); // should not get here
    } catch (const mavis::UnsupportedExtractorSpecialFieldID& ex) {
        // OK
    }

    mavis::ExtractorDirectInfoBitMask_Stores dibm_pseudo_op_p1("P1", 0x1 << 1, 0x1 << 2, 0xBEEF);
    inst = mavis_facade.makePseudoInst(dibm_pseudo_op_p1, 0);
    cout << "line " << dec << __LINE__ << ": " << "PSEUDO = " << inst->dasmString() << endl;
    assert(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS1) == mavis::OpcodeInfo::OperandTypes::LONG);
    assert(inst->getSourceOpInfo().isStoreData(mavis::InstMetaData::OperandFieldID::RS1) == false);
    assert(inst->getSourceOpInfo().getFieldType(mavis::InstMetaData::OperandFieldID::RS2) == mavis::OpcodeInfo::OperandTypes::LONG);
    assert(inst->getSourceOpInfo().isStoreData(mavis::InstMetaData::OperandFieldID::RS2) == true);

    mavis::InstructionUniqueID p0_uid = mavis_facade.lookupPseudoInstUniqueID("P0");
    mavis::ExtractorPseudoInfo pseudo_op_by_uid
        (p0_uid,
         {{mavis::InstMetaData::OperandFieldID::RS1, mavis::InstMetaData::OperandTypes::WORD, 1},
          {mavis::InstMetaData::OperandFieldID::RS2, mavis::InstMetaData::OperandTypes::DOUBLE, 2}},
         {{mavis::InstMetaData::OperandFieldID::RD,  mavis::InstMetaData::OperandTypes::WORD, 3}});
    inst = mavis_facade.makePseudoInst(pseudo_op_by_uid, 0);
    cout << "line " << dec << __LINE__ << ": " << "PSEUDO = " << inst->dasmString() << endl;

    // Test out some meta data queries needed for fusion... (using the P0 pseudo inst)
    assert(inst->getOpInfo()->isInstTypeAnyOf(mavis::InstMetaData::InstructionTypes::FLOAT, mavis::InstMetaData::InstructionTypes::BRANCH));
    assert(inst->getOpInfo()->isInstTypeAllOf(mavis::InstMetaData::InstructionTypes::BRANCH, mavis::InstMetaData::InstructionTypes::CONDITIONAL));
    assert(inst->getOpInfo()->isInstTypeSameAs(inst->getOpInfo()->getInstType(), mavis::InstMetaData::InstructionTypes::BRANCH));
    assert(inst->getOpInfo()->isInstTypeSameAs(inst->getOpInfo()->getInstType(), mavis::InstMetaData::InstructionTypes::FLOAT));
    assert(inst->getOpInfo()->isInstTypeSameAs(inst->getOpInfo()->getInstType()));

    // Issue #69
    mavis_facade.switchContext("BASE");
    inst = mavis_facade.makeInst(0x6f8c, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x6f8c = " << inst->dasmString() << endl;

    inst = mavis_facade.makeInst(0x01043823, 0);
    assert(inst != nullptr);
    cout << "line " << dec << __LINE__ << ": " << "DASM: 0x01043823 = " << inst->dasmString() << endl;

    mavis::ExtractorDirectInfo ex_info_srai ("srai", {1,2}, {4});
    inst = mavis_facade.makeInstDirectly(ex_info_srai, 0);
    assert(inst->getuArchInfo()->isROBGrpStart() == true); // was false in the uarch files; overridden with annotations
    assert(inst->getuArchInfo()->isROBGrpEnd() == false);

    return 0;
}
