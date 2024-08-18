#pragma once

#include <cstdint>
#include <set>
#include <vector>
#include <tuple>
#include <string>
#include<cstdint>

namespace mavis {

typedef std::set<std::string> StringSetType;
typedef std::vector<std::string> StringListType;

typedef StringSetType FieldNameSetType;
typedef StringListType UnitNameListType;
typedef StringListType FileNameListType;

typedef uint64_t Opcode;
typedef uint32_t InstructionUniqueID;

struct InstUIDListElement {
    std::string         mnemonic;
    InstructionUniqueID uid;
};
typedef std::vector<InstUIDListElement>                 InstUIDList;

using AnnotationOverrides = std::vector<std::pair<std::string, std::string>>;

} // namespace mavis
