#pragma once

#include "mavis/FloatUtils.h"

// clang-format off

// Try to use STL types if we're in C++23 mode
#if __cplusplus >= 202302L
    #include <stdfloat>

    #ifdef __STDCPP_FLOAT64_T__
        #define MAVIS_FLOAT64 std::float64_t
    #endif
#endif

// If that didn't catch it, try boost standard float type
#ifndef MAVIS_FLOAT64
    #include <boost/cstdfloat.hpp>

    #if defined(BOOST_FLOAT64_C) && !defined(MAVIS_FLOAT64)
        #define MAVIS_FLOAT64 boost::float64_t
    #endif
#endif

// If we haven't found a guaranteed 64 bit float, just try double
#ifndef MAVIS_FLOAT64
    #define MAVIS_FLOAT64 double
#endif

// clang-format on

namespace mavis
{
    using Double = MAVIS_FLOAT64;

    namespace float_utils
    {
        template <> struct IEEEFloatDefaults<64> : FloatSettings<Double, uint64_t, 11, 52>
        {
        };
    } // namespace float_utils
} // namespace mavis
