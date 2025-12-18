#pragma once

#include "mavis/FloatUtils.h"

// clang-format off

// Try to use STL types if we're in C++23 mode
#if __cplusplus >= 202302L
    #include <stdfloat>

    #ifdef __STDCPP_FLOAT32_T__
        #define MAVIS_FLOAT32 std::float32_t
    #endif
#endif

// If that didn't catch it, try boost standard float type
#ifndef MAVIS_FLOAT32
    #include <boost/cstdfloat.hpp>

    #if defined(BOOST_FLOAT32_C) && !defined(MAVIS_FLOAT32)
        #define MAVIS_FLOAT32 boost::float32_t
    #endif
#endif

// If we haven't found a guaranteed 32 bit float, just try float
#ifndef MAVIS_FLOAT32
    #define MAVIS_FLOAT32 float
#endif

// clang-format on

namespace mavis
{
    using Single = MAVIS_FLOAT32;

    namespace float_utils
    {
        template <> struct IEEEFloatDefaults<32> : FloatSettings<Single, uint32_t, 8, 23>
        {
        };
    }
}
