#include <numeric>
#include "stubs/quadmath_stubs.h"

__float128 __builtin_huge_valq()
{
    static constexpr unsigned __int128 INF128_EXPONENT{0x7FFF};
    static constexpr unsigned __int128 INF128{INF128_EXPONENT << 112};
    return std::bit_cast<__float128>(INF128);
}
