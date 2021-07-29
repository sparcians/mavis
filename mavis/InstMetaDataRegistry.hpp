#pragma once

#include "DecoderTypes.h"
#include "DecoderExceptions.h"
#include "InstMetaData.h"
#include <map>

namespace mavis {

class InstMetaDataRegistry
{
public:
    InstMetaDataRegistry() = default;

    InstMetaDataRegistry(const InstMetaDataRegistry& other) = delete;

    template<typename ...ArgTypes>
    InstMetaData::PtrType makeInstMetaData(const std::string& mnemonic, ArgTypes&& ...args)
    {
        const InstMetaData::PtrType& meta = std::make_shared<InstMetaData>(std::forward<ArgTypes>(args)...);
        if (registry_.find(mnemonic) == registry_.end()) {
            registry_[mnemonic] = meta;
        } else {
            throw BuildErrorDuplicateMnemonic(mnemonic);
        }
        return meta;
    }

    InstMetaData::PtrType lookup(const std::string& mnemonic) const
    {
        const auto iter = registry_.find(mnemonic);
        if (iter != registry_.end()) {
            return iter->second;
        } else {
            return nullptr;
        }
    }

private:
    std::map<std::string, InstMetaData::PtrType> registry_;
};

} // namespace mavis

