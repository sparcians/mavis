#pragma once

#include <array>
#include "Utils.h"

namespace mavis
{

    class Swizzler
    {
      public:
        struct Range
        {
            uint32_t span;
            uint64_t mask;

            Range(uint32_t x, uint32_t y) :
                span(y - x + 1),
                mask(-1ull >> (MASK_SIZE_ - span)),
                r_{x, y}
            {
                if (x > y) [[unlikely]]
                {
                    throw std::invalid_argument("invalid range");
                }
                assertMask_(x, "x");
                assertMask_(y, "y");
            }

            explicit Range(uint32_t x) : span(1), mask(0x1ull), r_{x, x} { assertMask_(x, "x"); }

            Range(const Range &) = default;

            uint32_t operator[](uint32_t i) const { return r_[i]; }

          private:
            inline static constexpr auto MASK_SIZE_ = mavis::utils::num_bits<decltype(mask)>;

            inline static void assertMask_(const uint32_t val, const char* value_name)
            {
                if (val >= MASK_SIZE_)
                {
                    throw std::invalid_argument("invalid " + std::string(value_name));
                }
            }

            std::array<uint32_t, 2> r_;
        };

      public:
        template <typename T> static inline uint64_t extract(uint64_t x, const T & r)
        {
            return (x & r.mask) << r[0];
        }

        template <typename T, typename... ArgTypes>
        static inline uint64_t extract(uint64_t x, const T & r, ArgTypes &&... args)
        {
            return extract(x, r) | extract(x >> r.span, std::forward<ArgTypes>(args)...);
        }
    };

} // namespace mavis
