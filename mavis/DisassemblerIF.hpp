#pragma once

#include "Extractor.h"
#include "InstMetaData.h"
#include <memory>

namespace mavis {

/**
 * @brief Disassembler Interface
 */
class DisassemblerIF
{
public:
    typedef std::shared_ptr<DisassemblerIF>   PtrType;

public:
    DisassemblerIF() = default;
    DisassemblerIF(const DisassemblerIF&) = default;
    virtual ~DisassemblerIF() = default;

    virtual std::string toString(const std::string &mnemonic, Opcode icode,
                                 const InstMetaData::PtrType& meta,
                                 const ExtractorIF::PtrType& extractor) const = 0;
};

} // namespace mavis
