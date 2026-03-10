#pragma once

#include "DecoderTypes.h"
#include "DecoderExceptions.h"
#include "SimpleDynArray.hpp"
#include <map>

namespace mavis {

/**
 * \brief Regsitry of instruction unique ID's (UID's)
 */
// TODO: Rename this to InstructionUIDRegistry
class InstructionRegistry
{
public:
    typedef std::shared_ptr<InstructionRegistry>        PtrType;

private:
    class UIDManager {
    public:
        UIDManager() = default;
        UIDManager(const UIDManager& other) = delete;

        InstructionUniqueID getUID()
        {
            return next_uid_;
        }

        InstructionUniqueID advanceUID() {
            return next_uid_++;
        }

        void allocateUID(InstructionUniqueID uid) {
            next_uid_ = std::max(uid + 1, next_uid_);
        }

    private:
        // NOTE: Using a static declaration here to assure all UID's are unique, even with multiple
        // instances of this class. This is just for safety, since with multiple mavis contexts, it's
        // possible for a given opcode to be decoded and annotated differently among contexts. Giving
        // the instruction a different UID in each context can help prevent the case where the user
        // has saved the UID under one context, then attempts to look that saved UID up in a different context
        // (and inadvertently retrieve info for the wrong instruction)
        static inline InstructionUniqueID next_uid_ = 1;
    };

public:
    InstructionRegistry(const InstUIDList& uid_list = {})
    {
        std::set<InstructionUniqueID>   used_uids;
        for (const auto& [mnemonic, uid] : uid_list) {
            if (uid == INVALID_UID) {
                throw BuilderInstIDListInvalidUID(mnemonic, uid);
            }

            if (used_uids.find(uid) == used_uids.end()) {
                used_uids.insert(uid);
                uid_man_.allocateUID(uid);
            } else {
                throw BuilderInstIDListUIDNotUnique(mnemonic, uid);
            }

            if (id_map_.find(mnemonic) == id_map_.end()) {
                id_map_[mnemonic] = uid;
                mnemonic_array_.add(uid, mnemonic);
            } else {
                throw BuilderInstIDListAlreadyRegistered(mnemonic);
            }
        }
    }

    InstructionRegistry(const InstructionRegistry&) = delete;

    InstructionUniqueID lookupUID(const std::string& mnemonic) const
    {
        try {
            return id_map_.at(mnemonic);
        } catch (const std::out_of_range& exc) {
            return INVALID_UID;
        }
    }

    const std::string& lookupMnemonic(const InstructionUniqueID uid) const
    {
        return mnemonic_array_[uid];
    }

    // This method is used by the builder to set up an "alias" from a compressed
    // instruction to its expanded form. Both the compressed and the expansion
    // share the same UID. We don't want to add this to the mnemonic_array_ since it
    // will replace the expansion's mnemonic with the compressed inst mnemonic.
    // Instead, we'll just be sure the UID is already registered in the
    // mnemonic_array
    bool aliasInstWithUID(const std::string& mnemonic, const InstructionUniqueID uid)
    {
        if ((uid == INVALID_UID) || (uid >= uid_man_.getUID())) [[unlikely]]
        {
            throw std::invalid_argument("invalid uid");
        }

        if (id_map_.find(mnemonic) == id_map_.end())
        {
            if (!mnemonic_array_.contains(uid)) [[unlikely]]
            {
                throw std::invalid_argument("unknown mnemonic uid");
            }
            id_map_[mnemonic] = uid;
            return true;
        }
        else
        {
            return false;
        }
    }

    InstructionUniqueID registerInst(const std::string& mnemonic)
    {
        const auto iter = id_map_.find(mnemonic);
        if (iter == id_map_.end()) {
            InstructionUniqueID uid = uid_man_.advanceUID();
            id_map_[mnemonic] = uid;
            mnemonic_array_.add(uid, mnemonic);
            return uid;
        } else {
            return iter->second;
        }
    }

    void addInstWithUIDUniquely(const std::string& mnemonic, const InstructionUniqueID uid)
    {
        if ((uid == INVALID_UID) || (uid >= uid_man_.getUID())) [[unlikely]]
        {
            throw std::invalid_argument("invalid uid");
        }

        if (id_map_.find(mnemonic) != id_map_.end())
        {
            throw BuildErrorDuplicateMnemonic(mnemonic);
        }
        id_map_[mnemonic] = uid;
        mnemonic_array_.add(uid, mnemonic);
    }

    InstructionUniqueID registerInstUniquely(const std::string& mnemonic)
    {
        InstructionUniqueID uid = uid_man_.advanceUID();
        addInstWithUIDUniquely(mnemonic, uid);
        return uid;
    }

private:
    UIDManager                                       uid_man_;
    std::map<const std::string, InstructionUniqueID> id_map_;
    SimpleDynArray<std::string>                      mnemonic_array_;
};

} // namespace mavis

