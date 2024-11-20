#include "mavis/ExtensionManager.hpp"

int main(int argc, char* argv[])
{
    ExtensionManager rv32g_man("rv32g", "json");
    ExtensionManager rv64g_man("rv64g", "json");

    return 0;
}
