#pragma once

#include <sstream>
#include <vector>

namespace mavis {

template<typename T>
class SimpleDynArray
{
private:
    static constexpr uint32_t  INITIAL_SIZE = 1000;
    static constexpr uint32_t  INITIAL_CAP = 2 * INITIAL_SIZE;
    static constexpr uint32_t  DEFAULT_UP_SIZE = INITIAL_SIZE / 10;

public:
    typedef std::shared_ptr<SimpleDynArray<T>>        PtrType;

public:
    class UnassignedIndex : public std::exception
    {
    public:
        virtual const char *what() const noexcept override
        {
            return why_.c_str();
        }

        explicit UnassignedIndex(uint32_t key)
        {
            std::stringstream ss;
            ss << "Index '" << key << "' "
               << " has not been assigned/added";
            why_ = ss.str();
        }
    private:
        std::string why_;
    };

private:
    struct Wrapper {
        T       obj;
        bool    valid = false;
    };
    typedef std::vector<Wrapper>                VectorType;

public:
    SimpleDynArray(uint32_t initial_size = INITIAL_SIZE,
                   uint32_t up_size = DEFAULT_UP_SIZE,
                   uint32_t initial_cap = INITIAL_CAP) :
        up_size_(up_size < 1 ? 1 : up_size)
    {
        vect_.reserve(initial_cap);
        vect_.resize(initial_size);
    }

    size_t size() const
    {
        return vect_.size();
    }

    bool contains(uint32_t key) const
    {
        return ((key < vect_.size()) && vect_[key].valid);
    }

    const T& lookup(uint32_t key) const
    {
        const Wrapper& w = vect_.at(key);
        if (w.valid) {
            return w.obj;
        } else {
            throw UnassignedIndex(key);
        }
    }

    const T& operator[](uint32_t key) const
    {
        return lookup(key);
    }

    void add(const uint32_t ukey, const T& obj)
    {
        if (ukey >= vect_.size()) {
            vect_.resize(ukey + up_size_);
        }
        vect_[ukey] = {obj, true};
    }

    void print(std::ostream& os) const
    {
        uint32_t n = 0;
        for (const auto& i : vect_) {
            if (i.valid) {
                os << "[" << n << "]: "
                   << i.obj << std::endl;
            }
            ++n;
        }
    }

private:
    VectorType  vect_;
    uint32_t    up_size_ = DEFAULT_UP_SIZE;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const SimpleDynArray<T>& sda)
{
    sda.print(os);
    return os;
}

} // namespace mavis