#pragma once

#include <cstddef>
#include <type_traits>

namespace mavis::utils
{
    template <typename ValType, typename OtherValType>
    constexpr bool isOneOf(const ValType val, const OtherValType other)
    {
        return (val == other);
    }

    template <typename ValType, typename OtherValType, typename... OtherValTypes>
    constexpr bool isOneOf(const ValType val, const OtherValType other, const OtherValTypes... rest)
    {
        return (val == other) || isOneOf(val, rest...);
    }

    template <typename T> inline constexpr size_t num_bits = 8 * sizeof(T);

    // any_of::value is true if Type is any of the types in OtherTypes
    template <typename Type, typename... OtherTypes>
    struct any_of : std::disjunction<std::is_same<Type, OtherTypes>...>
    {
    };

    template <typename Type, typename... OtherTypes>
    inline constexpr bool any_of_v = any_of<Type, OtherTypes...>::value;
} // namespace mavis::utils
