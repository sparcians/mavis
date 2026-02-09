#pragma once

#include <ostream>
#ifdef __cpp_lib_format
#include <format>
#else
#include <boost/format.hpp>
#endif

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
#ifdef __cpp_lib_format
        os << std::format("{}", value);
#else
        os << boost::format("%1%") % value;
#endif
    }
} // namespace mavis::float_utils
