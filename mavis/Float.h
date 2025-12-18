#pragma once

#include <bit>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <limits>

#include <boost/int128.hpp>

#include "Utils.h"

// clang-format off

// Try to use STL types if we're in C++23 mode
#if __cplusplus >= 202302L
    #include <stdfloat>

    #ifdef __STDCPP_FLOAT16_T__
        #define MAVIS_FLOAT16 std::float16_t
    #endif

    #ifdef __STDCPP_BFLOAT16_T__
        #define MAVIS_BFLOAT16 std::bfloat16_t
    #endif

    #ifdef __STDCPP_FLOAT32_T__
        #define MAVIS_FLOAT32 std::float32_t
    #endif

    #ifdef __STDCPP_FLOAT64_T__
        #define MAVIS_FLOAT64 std::float64_t
    #endif

    #ifdef __STDCPP_FLOAT128_T__
        #define MAVIS_FLOAT128 std::float128_t
    #endif
#endif

#if !defined(MAVIS_FLOAT128) && LDBL_MANT_DIG == 113
    #define MAVIS_FLOAT128 long double
#endif

// ... if that didn't catch all of them, try boost standard float types
#if !defined(MAVIS_FLOAT16) || !defined(MAVIS_FLOAT32) || !defined(MAVIS_FLOAT64) || !defined(MAVIS_FLOAT128)
    #include <boost/cstdfloat.hpp>

    #if defined(BOOST_FLOAT16_C) && !defined(MAVIS_FLOAT16)
        #define MAVIS_FLOAT16 boost::float16_t
    #endif
    #if defined(BOOST_FLOAT32_C) && !defined(MAVIS_FLOAT32)
        #define MAVIS_FLOAT32 boost::float32_t
    #endif
    #if defined(BOOST_FLOAT64_C) && !defined(MAVIS_FLOAT64)
        #define MAVIS_FLOAT64 boost::float64_t
    #endif
    #if defined(BOOST_FLOAT128_C) && !defined(MAVIS_FLOAT128)
        #define MAVIS_FLOAT128 boost::float128_t
        #if defined(__clang__) && !defined(__APPLE__)
            #define FORMAT_FLOAT128_WITH_QUADMATH 1
        #endif
    #endif
#endif

// Fall back to simde_float16 if neither of the above worked for float16
#ifndef MAVIS_FLOAT16
    #include "simde/simde-f16.h"
    #define MAVIS_FLOAT16 simde_float16
    #define USING_SIMDE_FLOAT16 1
#endif

#ifndef MAVIS_BFLOAT16
    #include "simde/simde-bf16.h"
    #define MAVIS_BFLOAT16 simde_bfloat16
    #define USING_SIMDE_BFLOAT16 1
    #if SIMDE_BFLOAT16_API == SIMDE_BFLOAT16_API_PORTABLE
        #define BFLOAT16_REQUIRES_WRAPPER 1
    #endif
#endif

#ifndef BFLOAT16_REQUIRES_WRAPPER
    #define BFLOAT16_REQUIRES_WRAPPER 0
#endif

// If we haven't found a guaranteed 32 bit float, just try float
#ifndef MAVIS_FLOAT32
    #define MAVIS_FLOAT32 float
#endif

// If we haven't found a guaranteed 64 bit float, just try double
#ifndef MAVIS_FLOAT64
    #define MAVIS_FLOAT64 double
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

    // If we've gotten this far it's unlikely there are built-in formatters for
    // 128 bit floats, so we'll use libquadmath
    #define FORMAT_FLOAT128_WITH_QUADMATH 1
#endif

// Make sure libquadmath is available
#ifdef FORMAT_FLOAT_128_WITH_QUADMATH
    #if __has_include(<quadmath.h>)
        #include <quadmath.h>
    #else
        #error "Could not find quadmath.h"
    #endif
#endif

#ifdef DISABLE_MAVIS_FLOAT128_SUPPORT
    #undef MAVIS_FLOAT128
    #undef FORMAT_FLOAT128_WITH_QUADMATH
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
            os << std::format("0x{:016x}{:016x}", static_cast<uint64_t>(value.value_ >> 64),
                              static_cast<uint64_t>(value.value_));
            return os;
        }
    };

    using Half = MAVIS_FLOAT16;
    using Single = MAVIS_FLOAT32;
    using Double = MAVIS_FLOAT64;
    using Quad = MAVIS_FLOAT128;

#if BFLOAT16_REQUIRES_WRAPPER
    template <typename Type>
    concept BFloatAutoPromoted =
        utils::any_of_v<Type, Single, Double, Quad> || std::is_integral_v<Type>;

    // Wraps the underlying MAVIS_BFLOAT16 type to implement arithmetic operations and conversions
    class BFloat
    {
      private:
        MAVIS_BFLOAT16 value_;

        void fromFloat_(const float value) { value_ = simde_bfloat16_from_float32(value); }

        inline float toFloat_() const { return simde_bfloat16_to_float32(value_); }

      public:
        BFloat() = default;

        BFloat(const MAVIS_BFLOAT16 value) : value_(value) {}

        explicit BFloat(const Half value) { fromFloat_(value); }

        template <typename RHS>
        BFloat(const RHS value)
        requires std::is_arithmetic_v<RHS>
        {
            fromFloat_(value);
        }

        BFloat(const BFloat & rhs) = default;

        BFloat & operator=(const BFloat & rhs) = default;

        BFloat & operator=(const float rhs)
        {
            fromFloat_(rhs);
            return *this;
        }

        inline BFloat operator+() const { return *this; }

        inline BFloat operator-() const { return -toFloat_(); }

        inline BFloat operator+(const BFloat & rhs) const { return toFloat_() + rhs.toFloat_(); }

        inline BFloat operator-(const BFloat & rhs) const { return toFloat_() - rhs.toFloat_(); }

        inline BFloat operator*(const BFloat & rhs) const { return toFloat_() * rhs.toFloat_(); }

        inline BFloat operator/(const BFloat & rhs) const { return toFloat_() / rhs.toFloat_(); }

        template <typename RHS>
        inline auto operator+(const RHS rhs) const
        requires BFloatAutoPromoted<RHS>
        {
            return toFloat_() + rhs;
        }

        template <typename RHS>
        inline auto operator-(const RHS rhs) const
        requires BFloatAutoPromoted<RHS>
        {
            return toFloat_() - rhs;
        }

        template <typename RHS>
        inline auto operator*(const RHS rhs) const
        requires BFloatAutoPromoted<RHS>
        {
            return toFloat_() * rhs;
        }

        template <typename RHS>
        inline auto operator/(const RHS rhs) const
        requires BFloatAutoPromoted<RHS>
        {
            return toFloat_() / rhs;
        }

        inline BFloat & operator++()
        {
            auto temp = toFloat_();
            ++temp;
            fromFloat_(temp);
            return *this;
        }

        inline BFloat operator++(int)
        {
            const auto temp = *this;
            ++(*this);
            return temp;
        }

        inline BFloat & operator--()
        {
            auto temp = toFloat_();
            --temp;
            fromFloat_(temp);
            return *this;
        }

        inline BFloat operator--(int)
        {
            const auto temp = *this;
            --(*this);
            return temp;
        }

        inline BFloat & operator+=(const BFloat & rhs)
        {
            *this = *this + rhs;
            return *this;
        }

        inline BFloat & operator-=(const BFloat & rhs)
        {
            *this = *this - rhs;
            return *this;
        }

        inline BFloat & operator*=(const BFloat & rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        inline BFloat & operator/=(const BFloat & rhs)
        {
            *this = *this / rhs;
            return *this;
        }

        template <typename RHS>
        inline BFloat & operator+=(const RHS rhs)
        requires BFloatAutoPromoted<RHS>
        {
            *this = *this + rhs;
            return *this;
        }

        template <typename RHS>
        inline BFloat & operator-=(const RHS rhs)
        requires BFloatAutoPromoted<RHS>
        {
            *this = *this - rhs;
            return *this;
        }

        template <typename RHS>
        inline BFloat & operator*=(const RHS rhs)
        requires BFloatAutoPromoted<RHS>
        {
            *this = *this * rhs;
            return *this;
        }

        template <typename RHS>
        inline BFloat & operator/=(const RHS rhs)
        requires BFloatAutoPromoted<RHS>
        {
            *this = *this / rhs;
            return *this;
        }

        bool operator==(const BFloat & rhs) const { return toFloat_() == rhs.toFloat_(); }

        template <typename RHS>
        bool operator==(const RHS rhs) const
        requires BFloatAutoPromoted<RHS>
        {
            return toFloat_() == rhs;
        }

        auto operator<=>(const BFloat & rhs) const { return toFloat_() <=> rhs.toFloat_(); }

        template <typename RHS>
        auto operator<=>(const RHS rhs) const
        requires BFloatAutoPromoted<RHS>
        {
            return toFloat_() <=> rhs;
        }

        operator float() const { return toFloat_(); }

        template <typename CastType>
        explicit operator CastType() const
        requires std::is_arithmetic_v<CastType>
        {
            return static_cast<CastType>(toFloat_());
        }

        friend inline std::ostream & operator<<(std::ostream & os, const BFloat & value)
        {
            return os << value.toFloat_();
        }
    };
#else
    using BFloat = MAVIS_BFLOAT16;
#endif

    namespace utils
    {
        template <> inline constexpr size_t num_bits<Half> = 16;

        template <> inline constexpr size_t num_bits<BFloat> = 16;

        template <> inline constexpr size_t num_bits<Quad> = 128;
    } // namespace utils

    namespace float_utils
    {
        template <typename FloatType>
        inline void formatFloat(std::ostream & os, const FloatType & value)
        {
            os << std::format("{}", value);
        }

        // simde_float16 has a special formatter function
#ifdef USING_SIMDE_FLOAT16
        template <>
        inline void formatFloat<simde_float16>(std::ostream & os, const simde_float16 & value)
        {
            os << simde_float16_to_float32(value);
        }
#endif

        // simde_bfloat16 has a special formatter function
#ifdef USING_SIMDE_BFLOAT16
        template <>
        inline void formatFloat<simde_bfloat16>(std::ostream & os, const simde_bfloat16 & value)
        {
            os << simde_bfloat16_to_float32(value);
        }
#endif

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

        template <typename FloatType, typename IntType, size_t ExponentBits, size_t FractionBits>
        struct FloatSettings
        {
            static inline constexpr size_t exponent_bits = ExponentBits;
            static inline constexpr size_t fraction_bits = FractionBits;
            using int_type = IntType;
            using float_type = FloatType;
        };

        template <size_t Bits> struct IEEEFloatDefaults;

        template <> struct IEEEFloatDefaults<16> : FloatSettings<Half, uint16_t, 5, 10>
        {
        };

        template <> struct IEEEFloatDefaults<32> : FloatSettings<Single, uint32_t, 8, 23>
        {
        };

        template <> struct IEEEFloatDefaults<64> : FloatSettings<Double, uint64_t, 11, 52>
        {
        };

        template <>
        struct IEEEFloatDefaults<128> : FloatSettings<Quad, boost::int128::uint128_t, 15, 112>
        {
        };

        using BFloatSettings = FloatSettings<BFloat, uint16_t, 8, 7>;

        template <typename Type, size_t... Bits>
        concept SupportedFloatBits =
            utils::any_of_v<Type, typename IEEEFloatDefaults<Bits>::float_type..., BFloat>;

        template <typename Type>
        concept SupportedFloatType = SupportedFloatBits<Type, 16, 32, 64, 128>;
    } // namespace float_utils

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
            using mavis_float_type = std::conditional_t<FloatLHS::bits >= FloatRHS::bits, FloatLHS, FloatRHS>;
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
        requires float_utils::SupportedFloatType<ValueType>
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
        requires float_utils::SupportedFloatType<ValueType>
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
        requires float_utils::SupportedFloatType<ValueType>
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
        requires float_utils::SupportedFloatType<ValueType>
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
        requires float_utils::SupportedFloatType<ValueType>
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

template <> struct std::formatter<mavis::BFloat> : std::formatter<float>
{
    constexpr auto parse(std::format_parse_context & ctx)
    {
        return std::formatter<float>::parse(ctx);
    }

    auto format(const mavis::BFloat & obj, std::format_context & ctx) const
    {
        return std::formatter<float>::format(static_cast<float>(obj), ctx);
    }
};
