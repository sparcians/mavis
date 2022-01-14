#pragma once

#include "DecoderTypes.h"

namespace mavis {

constexpr uint32_t REGISTER_X0 = 0;
constexpr uint32_t REGISTER_LINK = 1;
constexpr uint32_t REGISTER_SP = 2;
constexpr uint32_t REGISTER_ALT_LINK = 5;
constexpr uint32_t MAX_REG_NUM = 31;

constexpr Opcode CANONICAL_NOP = 0x13;
constexpr Opcode CANONICAL_CNOP = 0x1;

constexpr InstructionUniqueID INVALID_UID = 0;

} // namespace mavis
