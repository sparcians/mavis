#pragma once

#include <cstddef>

namespace mavis::utils
{
    template <typename ValType, typename OtherValType>
    constexpr bool isOneOf(const ValType val, const OtherValType other)
    {
        return (val == other);
    }

    template <typename ValType, typename OtherValType, typename... OtherValTypes>
    constexpr bool isOneOf(const ValType val, const OtherValType other,
                           const OtherValTypes... rest)
    {
        return (val == other) || isOneOf(val, rest...);
    }

    template <typename T>
    inline constexpr size_t num_bits = 8 * sizeof(T);
}
