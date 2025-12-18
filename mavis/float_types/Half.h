#pragma once
#include "mavis/FloatUtils.h"

// clang-format off

// Try to use STL types if we're in C++23 mode
#if __cplusplus >= 202302L
    #include <stdfloat>

    #ifdef __STDCPP_FLOAT16_T__
        #define MAVIS_FLOAT16 std::float16_t
    #endif
#endif

// If that didn't catch it, try boost standard float type
#ifndef MAVIS_FLOAT16
    #include <boost/cstdfloat.hpp>

    #if defined(BOOST_FLOAT16_C) && !defined(MAVIS_FLOAT16)
        #define MAVIS_FLOAT16 boost::float16_t
    #endif
#endif

// Fall back to simde_float16 if neither of the above worked for float16
#ifndef MAVIS_FLOAT16
    #include "simde/simde-f16.h"
    #define MAVIS_FLOAT16 simde_float16
    #define USING_SIMDE_FLOAT16 1
#endif

// clang-format on

namespace mavis
{
    using Half = MAVIS_FLOAT16;

    namespace float_utils
    {
        // simde_float16 has a special formatter function
#ifdef USING_SIMDE_FLOAT16
        template <>
        inline void formatFloat<simde_float16>(std::ostream & os, const simde_float16 & value)
        {
            os << simde_float16_to_float32(value);
        }
#endif

        template <> struct IEEEFloatDefaults<16> : FloatSettings<Half, uint16_t, 5, 10>
        {
        };
    }
}
