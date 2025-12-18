#pragma once

#include <ostream>
#include <format>

namespace mavis::float_utils
{
    template <typename FloatType, typename IntType, size_t ExponentBits, size_t FractionBits>
    struct FloatSettings
    {
        static inline constexpr size_t exponent_bits = ExponentBits;
        static inline constexpr size_t fraction_bits = FractionBits;
        using int_type = IntType;
        using float_type = FloatType;
    };

    template <size_t Bits> struct IEEEFloatDefaults;

    template <typename FloatType>
    inline void formatFloat(std::ostream & os, const FloatType & value)
    {
        os << std::format("{}", value);
    }
}
