#pragma once

#include <boost/int128.hpp>
#include "mavis/FloatUtils.h"

// clang-format off

#ifndef DISABLE_MAVIS_FLOAT128_SUPPORT
    // Try to use STL types if we're in C++23 mode
    #if __cplusplus >= 202302L
        #include <stdfloat>

        #ifdef __STDCPP_FLOAT128_T__
            #define MAVIS_FLOAT128 std::float128_t
        #endif
    #endif

    // Use long double if the platform defines it as 128-bit
    #if !defined(MAVIS_FLOAT128) && LDBL_MANT_DIG == 113
        #define MAVIS_FLOAT128 long double
    #endif

    // If that didn't catch it, try boost standard float type
    #ifndef MAVIS_FLOAT128
        // Boost versions < 1.84 have a definition conflict with libstdc++ 14 and newer
        #include <boost/version.hpp>
        #if BOOST_VERSION >= 108400
            #include <boost/cstdfloat.hpp>

            #if defined(BOOST_FLOAT128_C) && !defined(MAVIS_FLOAT128)
                #define MAVIS_FLOAT128 boost::float128_t
            #endif
        #endif
    #endif

    // Fall back to built-in float128 types if we haven't found one yet
    // These aren't available in Apple clang yet
    #if !defined(MAVIS_FLOAT128) && !(defined(__clang__) && defined(__APPLE__))
        // Clang has __float128
        #if defined(__clang__)
            #define MAVIS_FLOAT128 __float128
        // GCC has _Float128
        #else
            #define __STDC_WANT_IEC_60559_TYPES_EXT__
            #include <cfloat>

            #if !defined(MAVIS_FLOAT128) && defined(FLT128_MAX)
                #define MAVIS_FLOAT128 _Float128
            #endif
        #endif
    #endif

    // libstdc++ defines _GLIBCXX_FORMAT_F128 if std::format works with float128 types
    #if !defined(_GLIBCXX_FORMAT_F128) && !(defined(__clang__) && defined(__APPLE__))
        #define FORMAT_FLOAT128_WITH_QUADMATH 1
    #endif

    // Make sure libquadmath is available
    #ifdef FORMAT_FLOAT128_WITH_QUADMATH
        #if __has_include(<quadmath.h>)
            #include <quadmath.h>
        #else
            #error "Could not find quadmath.h"
        #endif
    #endif
#endif

// Use UnsupportedFloat128 if all other attempts failed
#ifndef MAVIS_FLOAT128
    #define MAVIS_FLOAT128 UnsupportedFloat128
#endif

// clang-format on

namespace mavis
{
    // Stores an IEEE quad float in a 128 bit integer
    // Does not support any arithmetic operations
    class UnsupportedFloat128
    {
      private:
        static constexpr boost::int128::uint128_t SIGN_MASK{boost::int128::uint128_t(0x1U) << 127};
        static constexpr boost::int128::uint128_t EXPONENT_MASK{boost::int128::uint128_t(0x7FFFU)
                                                                << 112};
        static constexpr boost::int128::uint128_t FRACTION_MASK{~(SIGN_MASK | EXPONENT_MASK)};

        static constexpr bool isNaN_(const boost::int128::uint128_t value)
        {
            return ((value & EXPONENT_MASK) == EXPONENT_MASK) && ((value & FRACTION_MASK) != 0U);
        }

        boost::int128::uint128_t value_;

      public:
        boost::int128::uint128_t getValue() const { return value_; }

        constexpr bool operator==(const UnsupportedFloat128 & rhs) const
        {
            if (isNaN_(value_) || isNaN_(rhs.value_))
            {
                return false;
            }

            return value_ == rhs.value_;
        }

        constexpr auto operator<=>(const UnsupportedFloat128 & rhs) const
        {
            return value_ <=> rhs.value_;
        }

        friend inline std::ostream & operator<<(std::ostream & os,
                                                const UnsupportedFloat128 & value)
        {
#ifdef MAVIS_HAS_STD_FORMAT
            os << std::format("0x{:016x}{:016x}", static_cast<uint64_t>(value.value_ >> 64),
                              static_cast<uint64_t>(value.value_));
#else
            os << boost::format("0x%|016x|%|016x|") % static_cast<uint64_t>(value.value_ >> 64) % static_cast<uint64_t>(value.value_);
#endif
            return os;
        }
    };

    using Quad = MAVIS_FLOAT128;

    namespace float_utils
    {
        template <>
        struct IEEEFloatDefaults<128> : FloatSettings<Quad, boost::int128::uint128_t, 15, 112>
        {
        };

        // Override the formatter for Quad if we need to use libquadmath
#ifdef FORMAT_FLOAT128_WITH_QUADMATH
        template <> inline void formatFloat<Quad>(std::ostream & os, const Quad & value)
        {
            std::array<char, 256> buf{};
            quadmath_snprintf(buf.data(), buf.size(), "%Qg", value);
            os << buf.data();
        }
#endif

        template <>
        inline void formatFloat<UnsupportedFloat128>(std::ostream & os,
                                                     const UnsupportedFloat128 & value)
        {
            os << value;
        }
    } // namespace float_utils
} // namespace mavis
