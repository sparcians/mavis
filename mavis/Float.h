#pragma once

#include <bit>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <limits>

#include "float_types/Half.h"
#include "float_types/Single.h"
#include "float_types/Double.h"
#include "float_types/Quad.h"
#include "float_types/BFloat.h"
#include "Utils.h"

namespace mavis
{
    namespace float_impl
    {
        template <typename Type, size_t... Bits>
        concept SupportedFloatBits =
            utils::any_of_v<Type, typename float_utils::IEEEFloatDefaults<Bits>::float_type..., BFloat>;

        template <typename Type>
        concept SupportedFloatType = SupportedFloatBits<Type, 16, 32, 64, 128>;
    } // namespace float_impl

    template <size_t Bits, typename FloatSettingsType = float_utils::IEEEFloatDefaults<Bits>>
    class Float
    {
      private:
        using IEEEDefaultFormat = float_utils::IEEEFloatDefaults<Bits>;

      public:
        using storage_type = FloatSettingsType::int_type;
        static_assert(std::is_nothrow_default_constructible_v<storage_type>);
        static_assert(utils::num_bits<storage_type> == Bits);

        using float_type = FloatSettingsType::float_type;
        static_assert(utils::num_bits<float_type> == Bits);

        static constexpr size_t bits = Bits;
        static constexpr size_t exponent_bits = FloatSettingsType::exponent_bits;
        static constexpr size_t fraction_bits = FloatSettingsType::fraction_bits;

        static_assert(bits == 1 + exponent_bits + fraction_bits);

        static constexpr bool is_ieee_format =
            (IEEEDefaultFormat::exponent_bits == exponent_bits)
            && (IEEEDefaultFormat::fraction_bits == fraction_bits);
        static constexpr bool supports_arithmetic_operations =
            !std::is_same_v<float_type, UnsupportedFloat128>;

        static constexpr storage_type exponent_mask = (storage_type(1U) << exponent_bits) - 1U;
        static constexpr storage_type exponent_bias =
            (storage_type(1U) << (exponent_bits - 1)) - 1U;
        static constexpr storage_type fraction_mask = (storage_type(1U) << fraction_bits) - 1U;

      private:
        template <typename SourceType> constexpr void convertFrom_(const SourceType value)
        {
            if constexpr (std::is_same_v<SourceType, float_type>)
            {
                data_ = std::bit_cast<storage_type>(value);
            }
            else
            {
                convertFrom_(static_cast<float_type>(value));
            }
        }

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr void convertFrom_(const Float<OtherBits, OtherFloatSettingsType> & other)
        {
            using OtherType = std::remove_cvref_t<decltype(other)>;
            static_assert(!((exponent_bits >= OtherType::exponent_bits
                             && fraction_bits < OtherType::fraction_bits)
                            || (exponent_bits < OtherType::exponent_bits
                                && fraction_bits >= OtherType::fraction_bits)));

            if constexpr (std::is_same_v<std::remove_pointer_t<decltype(this)>, OtherType>)
            {
                data_ = other.data_;
            }
            else
            {
                convertFrom_(other.asFloat());
            }
        }

        template <typename FloatLHS, typename FloatRHS> struct upcast_floats
        {
            using mavis_float_type =
                std::conditional_t<FloatLHS::bits >= FloatRHS::bits, FloatLHS, FloatRHS>;
            using cast_type = typename mavis_float_type::float_type;

            static constexpr auto upcast(const FloatLHS & lhs, const FloatRHS & rhs)
            {
                return std::make_pair(static_cast<cast_type>(lhs), static_cast<cast_type>(rhs));
            }
        };

        // Automatically upcasts to the highest common float type for arithmetic operations and
        // comparisons
        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr auto upcast_(const Float<OtherBits, OtherFloatSettingsType> & rhs) const
        {
            return upcast_floats<Float, Float<OtherBits, OtherFloatSettingsType>>::upcast(*this,
                                                                                          rhs);
        }

        template <typename ReturnType, template <typename> typename Op, size_t OtherBits,
                  typename OtherFloatSettingsType>
        constexpr ReturnType
        genericBinaryOp_(const Float<OtherBits, OtherFloatSettingsType> & rhs) const
        {
            const auto [arg1, arg2] = upcast_(rhs);
            constexpr Op<decltype(arg1)> op;
            return op(arg1, arg2);
        }

        template <template <typename> typename Op, size_t OtherBits,
                  typename OtherFloatSettingsType>
        constexpr auto binaryArithOp_(const Float<OtherBits, OtherFloatSettingsType> & rhs) const
        {
            using UpcastType = upcast_floats<Float, Float<OtherBits, OtherFloatSettingsType>>;
            using ReturnType = typename UpcastType::mavis_float_type;
            constexpr Op<typename UpcastType::cast_type> op;
            const auto [arg1, arg2] = UpcastType::upcast(*this, rhs);
            return ReturnType(op(arg1, arg2));
        }

        storage_type data_ = 0;

      public:
        static constexpr Float zero() { return Float{}; }

        static constexpr Float negative_zero() { return -zero(); };

        static constexpr Float infinity() { return Float(0, exponent_mask, 0); }

        static constexpr Float negative_infinity() { return -infinity(); }

        static constexpr Float qnan()
        {
            return Float(0, exponent_mask, (storage_type(1U) << (fraction_bits - 1)));
        };

        static constexpr Float min_normal() { return Float(0, 1, 0); }

        static constexpr Float max_normal()
        {
            return Float(0, exponent_mask & ~1U, fraction_mask);
        };

        static constexpr Float lowest() { return -max_normal(); };

        constexpr Float() = default;

        template <typename ValueType>
        constexpr Float(const ValueType value)
        requires float_impl::SupportedFloatType<ValueType>
        {
            convertFrom_(value);
        }

        constexpr Float(const Float &) = default;

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr Float(const Float<OtherBits, OtherFloatSettingsType> & other)
        {
            convertFrom_(other);
        }

        constexpr Float & operator=(const Float &) = default;

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr Float & operator=(const Float<OtherBits, OtherFloatSettingsType> & other)
        {
            convertFrom_(other);
            return *this;
        }

        template <typename ValueType>
        constexpr Float & operator=(const ValueType rhs)
        requires float_impl::SupportedFloatType<ValueType>
        {
            convertFrom_(rhs);
            return *this;
        }

        constexpr Float(const storage_type sign, const storage_type exponent,
                        const storage_type fraction)
        {
            setSign(sign);
            setExponent(exponent);
            setFraction(fraction);
        }

        constexpr void setSign(const storage_type sign)
        {
            constexpr storage_type CLEAR_SIGN_MASK =
                static_cast<storage_type>(~(storage_type(1U) << (exponent_bits + fraction_bits)));
            data_ = (data_ & CLEAR_SIGN_MASK) | (sign << (exponent_bits + fraction_bits));
        }

        constexpr void setExponent(const storage_type exponent)
        {
            constexpr storage_type CLEAR_EXPONENT_MASK = ~(exponent_mask << fraction_bits);
            data_ = (data_ & CLEAR_EXPONENT_MASK) | (exponent << fraction_bits);
        }

        constexpr void setFraction(const storage_type fraction)
        {
            constexpr storage_type CLEAR_FRACTION_MASK = ~fraction_mask;
            data_ = (data_ & CLEAR_FRACTION_MASK) | fraction;
        }

        constexpr storage_type sign() const { return data_ >> (exponent_bits + fraction_bits); }

        constexpr storage_type exponent() const { return (data_ >> fraction_bits) & exponent_mask; }

        constexpr storage_type fraction() const { return data_ & fraction_mask; }

        constexpr bool isZero() const { return exponent() == 0U && fraction() == 0U; }

        constexpr bool isSubnormal() const { return exponent() == 0U && fraction() != 0U; }

        constexpr bool isInf() const { return exponent() == exponent_mask && fraction() == 0U; }

        constexpr bool isNaN() const { return exponent() == exponent_mask && fraction() != 0U; }

        constexpr float_type asFloat() const { return static_cast<float_type>(*this); }

        template <typename ValueType>
        constexpr operator ValueType() const
        requires float_impl::SupportedFloatType<ValueType>
        {
            if constexpr (std::is_same_v<ValueType, float_type>)
            {
                return std::bit_cast<ValueType>(data_);
            }
            else
            {
                return static_cast<ValueType>(static_cast<float_type>(*this));
            }
        }

        constexpr auto operator+() const { return *this; }

        constexpr Float operator-() const
        {
            Float temp(*this);
            temp.setSign(~sign() & 1U);
            return temp;
        }

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr auto operator+(const Float<OtherBits, OtherFloatSettingsType> & rhs) const
        requires supports_arithmetic_operations
        {
            return binaryArithOp_<std::plus>(rhs);
        }

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr auto operator-(const Float<OtherBits, OtherFloatSettingsType> & rhs) const
        requires supports_arithmetic_operations
        {
            return binaryArithOp_<std::minus>(rhs);
        }

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr auto operator*(const Float<OtherBits, OtherFloatSettingsType> & rhs) const
        requires supports_arithmetic_operations
        {
            return binaryArithOp_<std::multiplies>(rhs);
        }

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr auto operator/(const Float<OtherBits, OtherFloatSettingsType> & rhs) const
        requires supports_arithmetic_operations
        {
            return binaryArithOp_<std::divides>(rhs);
        }

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr Float & operator+=(const Float<OtherBits, OtherFloatSettingsType> & rhs)
        requires supports_arithmetic_operations
        {
            *this = *this + rhs;
        }

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr Float & operator-=(const Float<OtherBits, OtherFloatSettingsType> & rhs)
        requires supports_arithmetic_operations
        {
            *this = *this - rhs;
        }

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr Float & operator*=(const Float<OtherBits, OtherFloatSettingsType> & rhs)
        requires supports_arithmetic_operations
        {
            *this = *this * rhs;
        }

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr Float & operator/=(const Float<OtherBits, OtherFloatSettingsType> & rhs)
        requires supports_arithmetic_operations
        {
            *this = *this / rhs;
        }

        constexpr Float & operator++()
        requires supports_arithmetic_operations
        {
            auto float_val = asFloat();
            ++float_val;
            *this = float_val;
            return *this;
        }

        constexpr Float operator++(int)
        requires supports_arithmetic_operations
        {
            Float temp(*this);
            ++(*this);
            return temp;
        }

        constexpr Float & operator--()
        requires supports_arithmetic_operations
        {
            auto float_val = asFloat();
            --float_val;
            *this = float_val;
            return *this;
        }

        constexpr Float operator--(int)
        requires supports_arithmetic_operations
        {
            Float temp(*this);
            --(*this);
            return temp;
        }

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr auto operator<=>(const Float<OtherBits, OtherFloatSettingsType> & rhs) const
        {
            const auto [arg1, arg2] = upcast_(rhs);
            return arg1 <=> arg2;
        }

        template <typename ValueType>
        constexpr auto operator<=>(const ValueType rhs) const
        requires float_impl::SupportedFloatType<ValueType>
        {
            if constexpr (utils::num_bits<ValueType> == bits)
            {
                return *this <=> Float(rhs);
            }
            else
            {
                return *this <=> Float<utils::num_bits<ValueType>>(rhs);
            }
        }

        template <size_t OtherBits, typename OtherFloatSettingsType>
        constexpr bool operator==(const Float<OtherBits, OtherFloatSettingsType> & rhs) const
        {
            const auto [arg1, arg2] = upcast_(rhs);
            return arg1 == arg2;
        }

        template <typename ValueType>
        constexpr bool operator==(const ValueType rhs) const
        requires float_impl::SupportedFloatType<ValueType>
        {
            if constexpr (utils::num_bits<ValueType> == bits)
            {
                return *this == Float(rhs);
            }
            else
            {
                return *this == Float<utils::num_bits<ValueType>>(rhs);
            }
        }

        friend inline std::ostream & operator<<(std::ostream & os, const Float & val)
        {
            float_utils::formatFloat(os, val.asFloat());
            return os;
        }
    };

    using Float16 = Float<16>;
    using BFloat16 = Float<16, float_utils::BFloatSettings>;
    using Float32 = Float<32>;
    using Float64 = Float<64>;
    using Float128 = Float<128>;
} // namespace mavis

template <size_t Bits, typename FloatSettingsType>
struct std::formatter<mavis::Float<Bits, FloatSettingsType>> :
    std::formatter<typename mavis::Float<Bits, FloatSettingsType>::float_type>
{
    constexpr auto parse(std::format_parse_context & ctx)
    {
        return std::formatter<typename mavis::Float<Bits, FloatSettingsType>::float_type>::parse(
            ctx);
    }

    auto format(const mavis::Float<Bits, FloatSettingsType> & obj, std::format_context & ctx) const
    {
        return std::formatter<typename mavis::Float<Bits, FloatSettingsType>::float_type>::format(
            obj.asFloat(), ctx);
    }
};
