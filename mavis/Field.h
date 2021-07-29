#pragma once

#include <ostream>
#include <string>
#include <array>
#include <cassert>

namespace mavis {

/**
 * Field
 */
class Field
{
public:
    Field(const std::string &name, const uint32_t rpos, const uint32_t len) :
        name_(name), rpos_(rpos), len_(len)
    {
        mask_ = -1ull >> (sizeof(mask_) * 8 - len);
        shift_ = rpos_;
        size_ = 1 << len_;
    }

    Field(const Field &) = default;

    virtual Field *clone() const
    {
        return new Field(*this);
    }

    virtual ~Field() = default;

    std::string getName() const
    {
        return name_;
    }

    uint32_t getLength() const
    {
        return len_;
    }

    uint32_t getSize() const
    {
        return size_;
    }

    virtual uint64_t getShiftedMask() const
    {
        return mask_ << shift_;
    }

    virtual bool isEquivalent(const Field &other) const
    {
        return (rpos_ == other.rpos_) &&
               (len_ == other.len_);
    }

    virtual uint64_t extract(const uint64_t icode) const
    {
        return (icode >> shift_) & mask_;
    }

    virtual void print(std::ostream &os) const
    {
        os << name_ << ":"
           << " 0x" << std::hex << mask_
           << " >> " << std::dec << shift_;
    }

protected:
    const std::string name_;
    const uint32_t rpos_ = 0;
    uint32_t len_ = 0;
    uint64_t mask_ = 0;
    uint32_t shift_ = 0;
    uint32_t size_ = 0;

protected:
    Field() :
        name_(""), rpos_(0)
    {}
};

inline std::ostream &operator<<(std::ostream &os, const Field &f)
{
    f.print(os);
    return os;
}

/**
 * ConcatField: a field which is the concatenation of sub-fields
 * @tparam N
 */
template<uint32_t N>
class ConcatField : public Field
{
public:
    template<typename ...FieldArgs>
    ConcatField(const FieldArgs &... flds)
    {
        fields_ = new std::array<Field, N> {flds...};
        for (const auto &f : *fields_) {
            len_ += f.getLength();
            mask_ |= f.getShiftedMask();
        }
        size_ = 1ull << len_;
    }

    ConcatField(const ConcatField &other) :
        Field(other)
    {
        fields_ = new std::array<Field, N>(*other.fields_);
        size_ = other.getSize();
        len_ = other.getLength();
        mask_ = other.mask_;
    }

    virtual Field *clone() const
    {
        return new ConcatField(*this);
    }

    virtual ~ConcatField()
    {
        delete fields_;
    }

    virtual uint64_t getShiftedMask() const
    {
        return mask_;
    }

    virtual uint64_t extract(uint64_t icode) const
    {
        uint64_t offset = 0;
        uint64_t value = 0;
        for (const auto &f : *fields_) {
            value |= (f.extract(icode) << offset);
            offset += f.getLength();
        }
        return value;
    }

    virtual void print(std::ostream &os) const
    {
        os << name_ << ": "
           << "size=" << std::dec << size_
           << ", len=" << len_
           << std::endl;
        for (const auto &f : *fields_) {
            os << f;
        }
    }

private:
    std::array<Field, N> *fields_;

    // For now...
    virtual bool isEquivalent(const Field &) const
    {
        assert(false);
        return false;
    }
};

template<uint32_t N>
std::ostream &operator<<(std::ostream &os, const ConcatField<N> &f)
{
    f.print(os);
    return os;
}

} // namespace mavis
