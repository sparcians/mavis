#include <iostream>

#include "mavis/FormStub.h"
#include "impl/forms/ExtractorDerived.h"

template<typename FormType>
void dumpTable(const char suffix)
{
    std::cout << "=========== fli." << suffix << " ===========" << std::endl;
    for(const auto& val: mavis::Extractor<FormType>::TABLE)
    {
        std::cout << val << std::endl;
    }
}

int main(int argc, char* argv[])
{
    dumpTable<mavis::Form_Rfloat_fli_h>('h');
    dumpTable<mavis::Form_Rfloat_fli_s>('s');
    dumpTable<mavis::Form_Rfloat_fli_d>('d');
    dumpTable<mavis::Form_Rfloat_fli_q>('q');
    return 0;
}
