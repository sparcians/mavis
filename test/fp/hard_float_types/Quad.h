#pragma once

#include "mavis/FloatUtils.h"

// clang-format off

#ifndef DISABLE_MAVIS_FLOAT128_SUPPORT
    // Try to use STL types if we're in C++23 mode
    #if __cplusplus >= 202302L
#if __has_include(<stdfloat>)
        #include <stdfloat>
#endif
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

#ifndef MAVIS_FLOAT128
    #define MAVIS_FLOAT128 void
    #define MAVIS_FLOAT128_UNSUPPORTED 1
#endif

// clang-format on

namespace mavis
{
    using Quad = MAVIS_FLOAT128;

#ifndef MAVIS_FLOAT128_UNSUPPORTED
    namespace float_utils
    {
        // Override the formatter for Quad if we need to use libquadmath
#ifdef FORMAT_FLOAT128_WITH_QUADMATH
        template <> inline void formatFloat<Quad>(std::ostream & os, const Quad & value)
        {
            std::array<char, 256> buf{};
            quadmath_snprintf(buf.data(), buf.size(), "%Qg", value);
            os << buf.data();
        }
#elif defined(MAVIS_HAS_STD_FORMAT)
        template <> inline void formatFloat<Quad>(std::ostream & os, const Quad & value)
        {
            os << std::format("{}", value);
        }
#endif
    } // namespace float_utils
#endif
} // namespace mavis
