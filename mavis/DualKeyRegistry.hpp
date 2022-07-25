#pragma once

#include "DecoderTypes.h"
#include "DecoderExceptions.h"
#include <map>

namespace mavis {

template<typename T>
class DualKeyRegistry
{
private:
    static constexpr uint32_t  INITIAL_SIZE = 1000;
    static constexpr uint32_t  INITIAL_CAP = 2 * INITIAL_SIZE;
    static constexpr uint32_t  DEFAULT_UP_SIZE = INITIAL_SIZE / 10;

public:
    typedef std::shared_ptr<DualKeyRegistry<T>>        PtrType;

public:
    class DuplicateKey : public std::exception
    {
    public:
        virtual const char *what() const noexcept override
        {
            return why_.c_str();
        }

        explicit DuplicateKey(const std::string &key)
        {
            std::stringstream ss;
            ss << "String key '" << key << "' "
               << " has duplicate registry entry.";
            why_ = ss.str();
        }
    private:
        std::string why_;
    };

    class UnassignedKey : public std::exception
    {
    public:
        virtual const char *what() const noexcept override
        {
            return why_.c_str();
        }

        explicit UnassignedKey(uint32_t key)
        {
            std::stringstream ss;
            ss << "Numeric key '" << key << "' "
               << " has not been registered";
            why_ = ss.str();
        }
    private:
        std::string why_;
    };

private:
    typedef std::map<const std::string, T>      MapType;
    struct Wrapper {
        T                                       obj;
        typename MapType::const_iterator        m_iter;         // For debugging and sanity checks
        bool                                    valid = false;
    };
    typedef std::vector<Wrapper>                VectorType;

public:
    DualKeyRegistry(uint32_t initial_size = INITIAL_SIZE,
                    uint32_t up_size = DEFAULT_UP_SIZE,
                    uint32_t initial_cap = INITIAL_CAP) :
        up_size_(up_size < 1 ? 1 : up_size)
    {
        vect_.reserve(initial_cap);
        vect_.resize(initial_size);
    }
    DualKeyRegistry(const DualKeyRegistry&) = delete;

    size_t size() const
    {
        return vect_.size();
    }

    bool contains(const std::string& key) const
    {
        return (map_.find(key) != map_.end());
    }

    bool contains(uint32_t key) const
    {
        return ((key < vect_.size()) && vect_[key].valid);
    }

    const T& lookup(const std::string& key) const
    {
        return map_.at(key);
    }

    const T& lookup(uint32_t key) const
    {
        const Wrapper& w = vect_.at(key);
        if (w.valid) {
            return w.obj;
        } else {
            throw UnassignedKey(key);
        }
    }

    void add(const std::string& skey, const uint32_t ukey, const T& obj)
    {
        if (map_.find(skey) == map_.end()) {
            const auto& [map_iter, success] = map_.insert({skey, obj});
            (void) success; // Silence the compiler warning for un-used variable
            if (ukey >= vect_.size()) {
                vect_.resize(ukey + up_size_);
            }
            vect_[ukey] = {obj, map_iter, true};
        } else {
            throw DuplicateKey(skey);
        }
    }

    void print(std::ostream& os) const
    {
        if (map_.empty()) {
            os << "EMPTY" << std::endl;
            return;
        }

        uint32_t n = 0;
        for (const auto& i : vect_) {
            if (i.valid) {
                os << "[" << n << "] "
                   << "('" << i.m_iter->first << "'): "
                   << i.obj << std::endl;
            }
            ++n;
        }
    }

    bool sanityCheck() const
    {
        for (const auto& i : vect_) {
            if (i.valid) {
                if (i.m_iter->second != i.obj) {
                    return false;
                }
            }
        }
        return true;
    }

private:
    MapType     map_;
    VectorType  vect_;
    uint32_t    up_size_ = DEFAULT_UP_SIZE;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const DualKeyRegistry<T>& dkr)
{
    dkr.print(os);
    return os;
}

} // namespace mavis


