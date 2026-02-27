#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>

#include <boost/core/demangle.hpp>

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

    template <typename T> inline T* notNull(T* p)
    {
        if (p == nullptr) [[unlikely]]
        {
            throw std::runtime_error("notNull: pointer was null: "
                                     + boost::core::demangle(typeid(T).name()));
        }
        return p;
    }

    template <typename T> inline const std::unique_ptr<T> & notNull(const std::unique_ptr<T> & p)
    {
        if (p == nullptr) [[unlikely]]
        {
            throw std::runtime_error("notNull: pointer was null: "
                                     + boost::core::demangle(typeid(T).name()));
        }
        return p;
    }

    template <typename T> inline std::unique_ptr<T> & notNull(std::unique_ptr<T> & p)
    {
        if (p == nullptr) [[unlikely]]
        {
            throw std::runtime_error("notNull: pointer was null: "
                                     + boost::core::demangle(typeid(T).name()));
        }
        return p;
    }

    template <typename T> inline const std::shared_ptr<T> & notNull(const std::shared_ptr<T> & p)
    {
        if (p == nullptr) [[unlikely]]
        {
            throw std::runtime_error("notNull: pointer was null: "
                                     + boost::core::demangle(typeid(T).name()));
        }
        return p;
    }

    template <typename T> inline std::shared_ptr<T> & notNull(std::shared_ptr<T> & p)
    {
        if (p == nullptr) [[unlikely]]
        {
            throw std::runtime_error("notNull: pointer was null: "
                                     + boost::core::demangle(typeid(T).name()));
        }
        return p;
    }

} // namespace mavis::utils
