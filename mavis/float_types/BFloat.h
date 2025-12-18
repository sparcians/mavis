#pragma once

#include "mavis/Utils.h"
#include "mavis/FloatUtils.h"
#include "Half.h"
#include "Single.h"
#include "Double.h"
#include "Quad.h"

// clang-format off

#if __cplusplus >= 202302L
    #include <stdfloat>

    #ifdef __STDCPP_BFLOAT16_T__
        #define MAVIS_BFLOAT16 std::bfloat16_t
    #endif
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

// clang-format on

namespace mavis
{
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

    namespace float_utils
    {
        using BFloatSettings = FloatSettings<BFloat, uint16_t, 8, 7>;

    // simde_bfloat16 has a special formatter function
#ifdef USING_SIMDE_BFLOAT16
        template <>
        inline void formatFloat<simde_bfloat16>(std::ostream & os, const simde_bfloat16 & value)
        {
            os << simde_bfloat16_to_float32(value);
        }
#endif
    }
}

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
