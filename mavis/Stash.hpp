#pragma once

#include <map>
#include <array>
#include <cstdint>

namespace mavis {

/**
 * \brief Simple "Stash" class: provides a storage area for key-value pairs (KVP's)
 * \tparam KeyType
 * \tparam ValueType
 * \tparam Size
 */
template<typename KeyType, typename ValueType, uint32_t HashSize, uint32_t MapSize = HashSize * 10>
class Stash
{
private:
    struct Node
    {
        bool valid = false;
        KeyType key;
        ValueType value;
    };

    typedef std::function<uint64_t(const KeyType&)> HashFunctionType;
    typedef std::array<Node, HashSize> HashType;
    typedef std::map<KeyType, Node> MapType;

public:
    /**
     * \brief Constructor
     * \param name
     * \param lambda hashing function
     */
    Stash(const std::string& name, const HashFunctionType& lambda) :
        name_(name), hash_func_(lambda)
    {}

    /**
     * \brief Constructor using default hash function
     * \param name
     */
    Stash(const std::string& name) :
        Stash(name, [](const KeyType& key) { return key; })
    {}

    Stash(const Stash&) = delete;

    /**
     * \brief Set a new KVP into the Stash
     * \param key
     * \param val
     */
    void set(const KeyType& key, const ValueType& val)
    {
        mru_ = {true, key, val};
        const uint32_t hash_index = hash_func_(key);
        hash_[hash_index % HashSize] = mru_;
        map_[hash_index % MapSize] = mru_;
    }

    /**
     * \brief Construct a new stash entry
     * \tparam ArgTypes
     * \param key
     * \param args
     */
    template<typename ...ArgTypes>
    const ValueType* allocate(const KeyType& key, ArgTypes&& ...args)
    {
        set(key, {std::forward<ArgTypes>(args)...});
        return &(mru_.value);
    }

    /**
     * \brief Lookup a key (return a pointer to its value, or nullptr if not found)
     * \param key
     * \return
     */
    const ValueType* lookup(const KeyType& key) const
    {
        if (mru_.valid && (mru_.key == key)) {
            return &(mru_.value);
        } else {
            const uint32_t hash_index = hash_func_(key);
            const Node* entry = &(hash_[hash_index % HashSize]);
            if (entry->valid && (entry->key == key)) {
                return &(entry->value);
            } else {
                const auto iter = map_.find(hash_index % MapSize);
                if ((iter != map_.end()) && (iter->second.key == key)) {
                    return &(iter->second.value);
                } else {
                    return nullptr;
                }
            }
        }
    }

private:
    std::string name_;
    Node mru_;
    HashFunctionType hash_func_;
    HashType hash_;
    MapType map_;
};
} // namespace mavis
