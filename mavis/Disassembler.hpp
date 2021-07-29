#pragma once

#include "DisassemblerIF.hpp"

namespace mavis {

/**
 * @brief Disassembler Object
 */
class Disassembler : public DisassemblerIF
{
public:
    typedef std::shared_ptr<Disassembler>   PtrType;

public:
    Disassembler() = default;
    Disassembler(const Disassembler&) = default;

    std::string toString(const std::string &mnemonic, Opcode icode,
                         const InstMetaData::PtrType& meta, const ExtractorIF::PtrType& extractor) const override
    {
        return extractor->dasmString(mnemonic, icode, meta);
    }
};

} // namespace mavis