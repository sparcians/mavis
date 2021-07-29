#pragma once

#include <array>
#include <cassert>

namespace mavis {

class Swizzler
{
public:
    struct Range {
        uint32_t                span;
        uint64_t                mask;

        Range(uint32_t x, uint32_t y):
            span(y - x + 1), mask(-1ull >> (sizeof(mask) * 8 - span)), r_{x, y}
        {
            assert(x <= y && "invalid range");
            assert(x < sizeof(mask) * 8 && "invalid x");
            assert(y < sizeof(mask) * 8 && "invalid y");
        }

        explicit Range(uint32_t x):
            span(1), mask(0x1ull), r_{x, x}
        {
            assert(x < sizeof(mask) * 8 && "invalid x");
        }

        Range(const Range&) = default;

        uint32_t operator[](uint32_t i) const {
            return r_[i];
        }

    private:
        std::array<uint32_t, 2> r_;
    };

public:
    template<typename T>
    static inline uint64_t extract(uint64_t x, const T& r) {
        return (x & r.mask) << r[0];
    }

    template<typename T, typename ...ArgTypes>
    static inline uint64_t extract(uint64_t x, const T& r, ArgTypes&&... args) {
        return extract(x, r) | extract(x >> r.span, std::forward<ArgTypes>(args)...);
    }
};

} // namespace mavis
