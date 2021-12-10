#include <iostream>
#include "Mavis.h"
#include "Inst.h"
#include "uArchInfo.h"

using namespace std;
using json = nlohmann::json;

using MavisType = Mavis<Instruction<uArchInfo>, uArchInfo>;

int main() {

    // Minimal example
    // We read rv64i only, and use Mavis to create a few Instruction objects and print them
    // TODO: for the most minimal example, probably too much uArchInfo in there complicating things

    // Mavis based on rv64i only
    MavisType mavis_facade({"../../../json/isa_rv64i.json"},
            {});
    cout << mavis_facade;

    Instruction<uArchInfo>::PtrType inst = nullptr;
    std::vector<mavis::Opcode> icodes = {0x00E00513, 0x08000513, 0x0805D263};
    for (const auto icode : icodes) {
        try {
            inst = mavis_facade.makeInst(icode, 0);
        } catch (const mavis::IllegalOpcode& ex) {
            cout << ex.what() << endl;
        }
        if (inst != nullptr) {
            cout << "DASM: " << std::hex << icode << std::dec << " = " << inst->dasmString() << endl;
        }
    }

    return 0;
}
