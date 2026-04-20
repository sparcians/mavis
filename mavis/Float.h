#pragma once

#include <cmath>
#include <ostream>

extern "C"
{
    #include "softfloat.h"
}

#include "Format.h"
#include "FloatUtils.h"

namespace mavis
{
    template<typename FloatType>
    struct FloatSettings;

    template<>
    struct FloatSettings<float16_t>
    {
        using format_type = float;
        using bits_type = uint16_t;
        static constexpr size_t exponent_bits = 5;
        static constexpr size_t fraction_bits = 10;
        using exponent_type = float_utils::bitset<exponent_bits>;
        using fraction_type = float_utils::bitset<fraction_bits>;
    };

    template<>
    struct FloatSettings<bfloat16_t>
    {
        using format_type = float;
        using bits_type = uint16_t;
        static constexpr size_t exponent_bits = 8;
        static constexpr size_t fraction_bits = 7;
        using exponent_type = float_utils::bitset<exponent_bits>;
        using fraction_type = float_utils::bitset<fraction_bits>;
    };

    template<>
    struct FloatSettings<float32_t>
    {
        using format_type = float;
        using bits_type = uint32_t;
        static constexpr size_t exponent_bits = 8;
        static constexpr size_t fraction_bits = 23;
        using exponent_type = float_utils::bitset<exponent_bits>;
        using fraction_type = float_utils::bitset<fraction_bits>;
    };

    template<>
    struct FloatSettings<float64_t>
    {
        using format_type = double;
        using bits_type = uint64_t;
        static constexpr size_t exponent_bits = 11;
        static constexpr size_t fraction_bits = 52;
        using exponent_type = float_utils::bitset<exponent_bits>;
        using fraction_type = float_utils::bitset<fraction_bits>;
    };

    template<>
    struct FloatSettings<float128_t>
    {
        using format_type = double;
        using bits_type = uint64_t;
        static constexpr size_t exponent_bits = 15;
        static constexpr size_t fraction_bits = 112;
        using exponent_type = float_utils::bitset<exponent_bits>;
        using fraction_type = float_utils::bitset<fraction_bits>;
    };

    template<typename FloatType>
    struct FloatExtractor
    {
        using FloatSettingsType = FloatSettings<FloatType>;
        using format_type = typename FloatSettingsType::format_type;
        using bits_type = typename FloatSettingsType::bits_type;
        using exponent_type = typename FloatSettingsType::exponent_type;
        using fraction_type = typename FloatSettingsType::fraction_type;

        static constexpr size_t exponent_bits = FloatSettingsType::exponent_bits;
        static constexpr size_t fraction_bits = FloatSettingsType::fraction_bits;
        static constexpr size_t sign_bit = exponent_bits + fraction_bits;
        static constexpr size_t bits = sign_bit + 1;

        using storage_type = float_utils::bitset<bits>;

        template<size_t num_bits, size_t bitset_size>
        static consteval float_utils::bitset<bitset_size> genBitMask_()
        {
            float_utils::bitset<bitset_size> val{};

            if constexpr(num_bits != 0)
            {
                val.set();

                if constexpr(num_bits != bitset_size)
                {
                    val >>= bitset_size - num_bits;
                }
            }

            return val;
        }

        template<size_t N, size_t M>
        static constexpr float_utils::bitset<M> convertBitset_(float_utils::bitset<N> val)
        {
            if constexpr(M == N)
            {
                return val;
            }
            else
            {
                constexpr auto chunk_size = std::min(utils::num_bits<unsigned long long>, N);
                constexpr float_utils::bitset<N> mask = genBitMask_<chunk_size, N>();

                float_utils::bitset<M> converted{};
                size_t shift = 0;

                while(val.any() && shift < N)
                {
                    converted |= float_utils::bitset<M>{(val & mask).to_ulong()} << shift;
                    val >>= chunk_size;
                    shift += chunk_size;
                }

                return converted;
            }
        }

        static constexpr exponent_type exponent_max = genBitMask_<exponent_bits, exponent_bits>();
        static constexpr exponent_type exponent_bias = genBitMask_<exponent_bits - 1, exponent_bits>();
        static constexpr fraction_type fraction_max = genBitMask_<fraction_bits, fraction_bits>();

        static constexpr bool sign(const storage_type& val) { return val.test(exponent_bits + fraction_bits); }

        static constexpr exponent_type exponent(storage_type val)
        {
            val.reset(sign_bit);
            val >>= fraction_bits;
            return convertBitset_<bits, exponent_bits>(val);
        }

        static constexpr fraction_type fraction(const storage_type& val)
        {
            return convertBitset_<bits, fraction_bits>(val);
        }

        static constexpr void setSign(storage_type& val, const bool sign)
        {
            val.set(sign_bit, sign);
        }

        static constexpr void setExponent(storage_type& val, const exponent_type exponent)
        {
            constexpr storage_type CLEAR_EXPONENT_MASK = ~(convertBitset_<exponent_bits, bits>(exponent_max) << fraction_bits);
            val = (val & CLEAR_EXPONENT_MASK) | (convertBitset_<exponent_bits, bits>(exponent) << fraction_bits);
        }

        static constexpr void setFraction(storage_type& val, const fraction_type fraction)
        {
            constexpr storage_type CLEAR_FRACTION_MASK = ~genBitMask_<fraction_bits, bits>();
            val = (val & CLEAR_FRACTION_MASK) | convertBitset_<fraction_bits, bits>(fraction);
        }

        static constexpr FloatType toFloat(const storage_type& val) requires (bits <= utils::num_bits<unsigned long long>)
        {
            return FloatType{static_cast<bits_type>(val.to_ullong())};
        }

        static constexpr FloatType toFloat(storage_type val) requires (bits > utils::num_bits<unsigned long long>)
        {
            FloatType float_val{};
            static_assert(std::is_array_v<decltype(float_val.v)> && sizeof(float_val.v[0]) == sizeof(unsigned long long));

            constexpr auto num_elems = sizeof(float_val.v) / sizeof(float_val.v[0]);

            constexpr auto chunk_size = utils::num_bits<unsigned long long>;
            constexpr storage_type mask = genBitMask_<chunk_size, bits>();

            for(size_t i = 0; i < num_elems; ++i)
            {
                float_val.v[i] = (val & mask).to_ullong();
                val >>= chunk_size;
            }

            return float_val;
        }

        static constexpr storage_type fromFloat(const FloatType& float_val)
        {
            constexpr auto chunk_size = utils::num_bits<unsigned long long>;

            if constexpr(bits <= chunk_size)
            {
                return storage_type{float_val.v};
            }
            else
            {
                static_assert(std::is_array_v<decltype(float_val.v)> && sizeof(float_val.v[0]) == sizeof(unsigned long long));

                constexpr auto num_elems = sizeof(float_val.v) / sizeof(float_val.v[0]);

                static_assert(num_elems > 0);

                storage_type val{};
                for(size_t i = 0; i < num_elems; ++i)
                {
                    val |= storage_type{float_val.v[i]} << (i * chunk_size);
                }

                return val;
            }
        }
    };

    template<typename FloatType>
    static constexpr bool is_soft_float = utils::any_of_v<FloatType, float16_t, bfloat16_t, float32_t, float64_t, float128_t>;

    template<typename ToType>
    struct FloatConverter;

    template<>
    struct FloatConverter<float32_t>
    {
        constexpr static float32_t convert(const float val)
        {
            return std::bit_cast<float32_t>(val);
        }

        static float32_t convert(const double val)
        {
            return convert(std::bit_cast<float64_t>(val));
        }

        static float32_t convert(const int val)
        {
            return i32_to_f32(val);
        }

        static float32_t convert(const bfloat16_t& val)
        {
            return bf16_to_f32(val);
        }

        static float32_t convert(const float16_t& val)
        {
            return f16_to_f32(val);
        }

        static float32_t convert(const float32_t& val)
        {
            return val;
        }

        static float32_t convert(const float64_t& val)
        {
            return f64_to_f32(val);
        }

        static float32_t convert(const float128_t& val)
        {
            return f128_to_f32(val);
        }
    };

    template<>
    struct FloatConverter<float16_t>
    {
        static float16_t convert(const float val)
        {
            return convert(std::bit_cast<float32_t>(val));
        }

        static float16_t convert(const double val)
        {
            return convert(std::bit_cast<float64_t>(val));
        }

        static float16_t convert(const int val)
        {
            return i32_to_f16(val);
        }

        static float16_t convert(const float16_t& val)
        {
            return val;
        }

        static float16_t convert(const bfloat16_t& val)
        {
            return convert(FloatConverter<float32_t>::convert(val));
        }

        static float16_t convert(const float32_t& val)
        {
            return f32_to_f16(val);
        }

        static float16_t convert(const float64_t& val)
        {
            return f64_to_f16(val);
        }

        static float16_t convert(const float128_t& val)
        {
            return f128_to_f16(val);
        }
    };

    template<>
    struct FloatConverter<bfloat16_t>
    {
        template<typename FromType>
        static bfloat16_t convert(const FromType& val) requires (!std::is_same_v<FromType, float32_t> && !std::is_same_v<FromType, bfloat16_t>)
        {
            return convert(FloatConverter<float32_t>::convert(val));
        }

        static bfloat16_t convert(const bfloat16_t& val)
        {
            return val;
        }

        static bfloat16_t convert(const float32_t& val)
        {
            return f32_to_bf16(val);
        }
    };

    template<>
    struct FloatConverter<float64_t>
    {
        static float64_t convert(const float val)
        {
            return convert(std::bit_cast<float32_t>(val));
        }

        constexpr static float64_t convert(const double val)
        {
            return std::bit_cast<float64_t>(val);
        }

        static float64_t convert(const int val)
        {
            return i32_to_f64(val);
        }

        static float64_t convert(const bfloat16_t& val)
        {
            return convert(FloatConverter<float32_t>::convert(val));
        }

        static float64_t convert(const float16_t& val)
        {
            return f16_to_f64(val);
        }

        static float64_t convert(const float32_t& val)
        {
            return f32_to_f64(val);
        }

        static float64_t convert(const float64_t& val)
        {
            return val;
        }

        static float64_t convert(const float128_t& val)
        {
            return f128_to_f64(val);
        }
    };

    template<>
    struct FloatConverter<float128_t>
    {
        static float128_t convert(const float val)
        {
            return convert(std::bit_cast<float32_t>(val));
        }

        static float128_t convert(const double val)
        {
            return convert(std::bit_cast<float64_t>(val));
        }

        static float128_t convert(const int val)
        {
            return i32_to_f128(val);
        }

        static float128_t convert(const bfloat16_t& val)
        {
            return convert(FloatConverter<float32_t>::convert(val));
        }

        static float128_t convert(const float16_t& val)
        {
            return f16_to_f128(val);
        }

        static float128_t convert(const float32_t& val)
        {
            return f32_to_f128(val);
        }

        static float128_t convert(const float64_t& val)
        {
            return f64_to_f128(val);
        }

        static float128_t convert(const float128_t& val)
        {
            return val;
        }
    };

    template<>
    struct FloatConverter<float>
    {
        template<typename FromType>
        static float convert(const FromType& val) requires (!std::is_same_v<FromType, float32_t>)
        {
            return convert(FloatConverter<float32_t>::convert(val));
        }

        constexpr static float convert(const float32_t& val)
        {
            return std::bit_cast<float>(val);
        }
    };

    template<>
    struct FloatConverter<double>
    {
        template<typename FromType>
        static double convert(const FromType& val) requires (!std::is_same_v<FromType, float64_t>)
        {
            return convert(FloatConverter<float64_t>::convert(val));
        }

        constexpr static double convert(const float64_t& val)
        {
            return std::bit_cast<double>(val);
        }
    };

    template<>
    struct FloatConverter<int>
    {
        static int convert(const bfloat16_t& val)
        {
            return convert(FloatConverter<float32_t>::convert(val));
        }

        static int convert(const float16_t& val)
        {
            return f16_to_i32_r_minMag(val, false);
        }

        static int convert(const float32_t& val)
        {
            return f32_to_i32_r_minMag(val, false);
        }

        static int convert(const float64_t& val)
        {
            return f64_to_i32_r_minMag(val, false);
        }

        static int convert(const float128_t& val)
        {
            return f128_to_i32_r_minMag(val, false);
        }
    };

    template<typename LHSFloatType, typename RHSFloatType>
    struct FloatPromotionRules
    {
        static constexpr bool is_incompatible = (std::is_same_v<LHSFloatType, float16_t> && std::is_same_v<RHSFloatType, bfloat16_t>) ||
                                                (std::is_same_v<LHSFloatType, bfloat16_t> && std::is_same_v<RHSFloatType, float16_t>);

        static constexpr bool is_promotable = !is_incompatible && is_soft_float<LHSFloatType> && is_soft_float<RHSFloatType> && (sizeof(LHSFloatType) >= sizeof(RHSFloatType));

        using promoted_type = std::conditional_t<(sizeof(LHSFloatType) > sizeof(RHSFloatType)), LHSFloatType, RHSFloatType>;

        static_assert(!is_incompatible,
                      "Cannot auto-promote between float16_t and bfloat16_t. One of the operands must be manually cast to the other type.");

        template<typename T>
        static constexpr promoted_type promote(const T& val)
        {
            if constexpr(std::is_same_v<T, promoted_type>)
            {
                return val;
            }
            else
            {
                return FloatConverter<promoted_type>::convert(val);
            }
        }
    };

    template<typename FloatType>
    struct FloatPromotionRules<FloatType, int>
    {
        static constexpr bool is_promotable = is_soft_float<FloatType>;

        using promoted_type = FloatType;

        template<typename T>
        static constexpr promoted_type promote(const T& val)
        {
            if constexpr(std::is_same_v<T, promoted_type>)
            {
                return val;
            }
            else
            {
                return FloatConverter<promoted_type>::convert(val);
            }
        }
    };

    template<typename FloatType>
    struct FloatPromotionRules<int, FloatType> : public FloatPromotionRules<FloatType, int>
    {
    };

    template<typename FloatType>
    struct FloatPromotionRules<FloatType, float>
    {
        static constexpr bool is_promotable = is_soft_float<FloatType> && sizeof(FloatType) >= sizeof(float);

        using promoted_type = FloatType;

        template<typename T>
        static constexpr promoted_type promote(const T& val)
        {
            if constexpr(std::is_same_v<T, promoted_type>)
            {
                return val;
            }
            else
            {
                return FloatConverter<promoted_type>::convert(val);
            }
        }
    };

    template<typename FloatType>
    struct FloatPromotionRules<float, FloatType> : public FloatPromotionRules<FloatType, float>
    {
    };

    template<typename FloatType>
    struct FloatPromotionRules<FloatType, double>
    {
        static constexpr bool is_promotable = is_soft_float<FloatType> && sizeof(FloatType) >= sizeof(double);

        using promoted_type = FloatType;

        template<typename T>
        static constexpr promoted_type promote(const T& val)
        {
            if constexpr(std::is_same_v<T, promoted_type>)
            {
                return val;
            }
            else
            {
                return FloatConverter<promoted_type>::convert(val);
            }
        }
    };

    template<typename FloatType>
    struct FloatPromotionRules<double, FloatType> : public FloatPromotionRules<FloatType, double>
    {
    };

    struct FloatOps
    {
        template<typename LHSFloatType, typename RHSFloatType>
        static constexpr auto add(const LHSFloatType& lhs, const RHSFloatType& rhs) requires(!std::is_same_v<LHSFloatType, RHSFloatType>)
        {
            using promotion_rules = FloatPromotionRules<LHSFloatType, RHSFloatType>;
            return add(promotion_rules::promote(lhs), promotion_rules::promote(rhs));
        }

        static float16_t add(const float16_t& lhs, const float16_t& rhs)
        {
            return f16_add(lhs, rhs);
        }

        static bfloat16_t add(const bfloat16_t& lhs, const bfloat16_t& rhs)
        {
            return FloatConverter<bfloat16_t>::convert(add(FloatConverter<float32_t>::convert(lhs), FloatConverter<float32_t>::convert(rhs)));
        }

        static float32_t add(const float32_t& lhs, const float32_t& rhs)
        {
            return f32_add(lhs, rhs);
        }

        static float64_t add(const float64_t& lhs, const float64_t& rhs)
        {
            return f64_add(lhs, rhs);
        }

        static float128_t add(const float128_t& lhs, const float128_t& rhs)
        {
            return f128_add(lhs, rhs);
        }

        template<typename LHSFloatType, typename RHSFloatType>
        static constexpr auto sub(const LHSFloatType& lhs, const RHSFloatType& rhs) requires(!std::is_same_v<LHSFloatType, RHSFloatType>)
        {
            using promotion_rules = FloatPromotionRules<LHSFloatType, RHSFloatType>;
            return sub(promotion_rules::promote(lhs), promotion_rules::promote(rhs));
        }

        static float16_t sub(const float16_t& lhs, const float16_t& rhs)
        {
            return f16_sub(lhs, rhs);
        }

        static bfloat16_t sub(const bfloat16_t& lhs, const bfloat16_t& rhs)
        {
            return FloatConverter<bfloat16_t>::convert(sub(FloatConverter<float32_t>::convert(lhs), FloatConverter<float32_t>::convert(rhs)));
        }

        static float32_t sub(const float32_t& lhs, const float32_t& rhs)
        {
            return f32_sub(lhs, rhs);
        }

        static float64_t sub(const float64_t& lhs, const float64_t& rhs)
        {
            return f64_sub(lhs, rhs);
        }

        static float128_t sub(const float128_t& lhs, const float128_t& rhs)
        {
            return f128_sub(lhs, rhs);
        }

        template<typename LHSFloatType, typename RHSFloatType>
        static constexpr auto div(const LHSFloatType& lhs, const RHSFloatType& rhs) requires(!std::is_same_v<LHSFloatType, RHSFloatType>)
        {
            using promotion_rules = FloatPromotionRules<LHSFloatType, RHSFloatType>;
            return div(promotion_rules::promote(lhs), promotion_rules::promote(rhs));
        }

        static float16_t div(const float16_t& lhs, const float16_t& rhs)
        {
            return f16_div(lhs, rhs);
        }

        static bfloat16_t div(const bfloat16_t& lhs, const bfloat16_t& rhs)
        {
            return FloatConverter<bfloat16_t>::convert(div(FloatConverter<float32_t>::convert(lhs), FloatConverter<float32_t>::convert(rhs)));
        }

        static float32_t div(const float32_t& lhs, const float32_t& rhs)
        {
            return f32_div(lhs, rhs);
        }

        static float64_t div(const float64_t& lhs, const float64_t& rhs)
        {
            return f64_div(lhs, rhs);
        }

        static float128_t div(const float128_t& lhs, const float128_t& rhs)
        {
            return f128_div(lhs, rhs);
        }

        template<typename LHSFloatType, typename RHSFloatType>
        static constexpr auto mul(const LHSFloatType& lhs, const RHSFloatType& rhs) requires(!std::is_same_v<LHSFloatType, RHSFloatType>)
        {
            using promotion_rules = FloatPromotionRules<LHSFloatType, RHSFloatType>;
            return mul(promotion_rules::promote(lhs), promotion_rules::promote(rhs));
        }

        static float16_t mul(const float16_t& lhs, const float16_t& rhs)
        {
            return f16_mul(lhs, rhs);
        }

        static bfloat16_t mul(const bfloat16_t& lhs, const bfloat16_t& rhs)
        {
            return FloatConverter<bfloat16_t>::convert(mul(FloatConverter<float32_t>::convert(lhs), FloatConverter<float32_t>::convert(rhs)));
        }

        static float32_t mul(const float32_t& lhs, const float32_t& rhs)
        {
            return f32_mul(lhs, rhs);
        }

        static float64_t mul(const float64_t& lhs, const float64_t& rhs)
        {
            return f64_mul(lhs, rhs);
        }

        static float128_t mul(const float128_t& lhs, const float128_t& rhs)
        {
            return f128_mul(lhs, rhs);
        }

        template<typename LHSFloatType, typename RHSFloatType>
        static constexpr auto eq(const LHSFloatType& lhs, const RHSFloatType& rhs) requires(!std::is_same_v<LHSFloatType, RHSFloatType>)
        {
            using promotion_rules = FloatPromotionRules<LHSFloatType, RHSFloatType>;
            return eq(promotion_rules::promote(lhs), promotion_rules::promote(rhs));
        }

        static bool eq(const float16_t& lhs, const float16_t& rhs)
        {
            return f16_eq(lhs, rhs);
        }

        static bool eq(const bfloat16_t& lhs, const bfloat16_t& rhs)
        {
            return eq(FloatConverter<float32_t>::convert(lhs), FloatConverter<float32_t>::convert(rhs));
        }

        static bool eq(const float32_t& lhs, const float32_t& rhs)
        {
            return f32_eq(lhs, rhs);
        }

        static bool eq(const float64_t& lhs, const float64_t& rhs)
        {
            return f64_eq(lhs, rhs);
        }

        static bool eq(const float128_t& lhs, const float128_t& rhs)
        {
            return f128_eq(lhs, rhs);
        }

        template<typename LHSFloatType, typename RHSFloatType>
        static constexpr auto lt(const LHSFloatType& lhs, const RHSFloatType& rhs) requires(!std::is_same_v<LHSFloatType, RHSFloatType>)
        {
            using promotion_rules = FloatPromotionRules<LHSFloatType, RHSFloatType>;
            return lt(promotion_rules::promote(lhs), promotion_rules::promote(rhs));
        }

        static bool lt(const float16_t& lhs, const float16_t& rhs)
        {
            return f16_lt(lhs, rhs);
        }

        static bool lt(const bfloat16_t& lhs, const bfloat16_t& rhs)
        {
            return lt(FloatConverter<float32_t>::convert(lhs), FloatConverter<float32_t>::convert(rhs));
        }

        static bool lt(const float32_t& lhs, const float32_t& rhs)
        {
            return f32_lt(lhs, rhs);
        }

        static bool lt(const float64_t& lhs, const float64_t& rhs)
        {
            return f64_lt(lhs, rhs);
        }

        static bool lt(const float128_t& lhs, const float128_t& rhs)
        {
            return f128_lt(lhs, rhs);
        }

        template<typename LHSFloatType, typename RHSFloatType>
        static constexpr auto le(const LHSFloatType& lhs, const RHSFloatType& rhs) requires(!std::is_same_v<LHSFloatType, RHSFloatType>)
        {
            using promotion_rules = FloatPromotionRules<LHSFloatType, RHSFloatType>;
            return le(promotion_rules::promote(lhs), promotion_rules::promote(rhs));
        }

        static bool le(const float16_t& lhs, const float16_t& rhs)
        {
            return f16_le(lhs, rhs);
        }

        static bool le(const bfloat16_t& lhs, const bfloat16_t& rhs)
        {
            return le(FloatConverter<float32_t>::convert(lhs), FloatConverter<float32_t>::convert(rhs));
        }

        static bool le(const float32_t& lhs, const float32_t& rhs)
        {
            return f32_le(lhs, rhs);
        }

        static bool le(const float64_t& lhs, const float64_t& rhs)
        {
            return f64_le(lhs, rhs);
        }

        static bool le(const float128_t& lhs, const float128_t& rhs)
        {
            return f128_le(lhs, rhs);
        }

        template<typename FloatType>
        static float toHardFloat(const FloatType& val) requires(sizeof(FloatType) <= sizeof(float))
        {
            return FloatConverter<float>::convert(val);
        }

        template<typename FloatType>
        static double toHardFloat(const FloatType& val) requires(sizeof(FloatType) > sizeof(float))
        {
            return FloatConverter<double>::convert(val);
        }

        template<typename FloatType>
        static void format(std::ostream& os, const FloatType& val) requires(sizeof(FloatType) <= sizeof(double))
        {
            os << toHardFloat(val);
        }

        template<typename FloatType>
        static void format(std::ostream& os, const FloatType& val) requires(sizeof(FloatType) > sizeof(double))
        {
            os << toHardFloat(val);
        }
    };

    template<typename FloatType>
    class Float
    {
        private:
            static_assert(is_soft_float<FloatType>);

            template<typename OtherFloatType>
            static constexpr bool is_float_type = std::is_same_v<FloatType, OtherFloatType>;

            constexpr bool fractionIsZero_() const
            {
                return fraction().none();
            }

            template <typename OtherFloatType>
            using PromotionRules = FloatPromotionRules<FloatType, OtherFloatType>;

            template<typename OtherFloatType>
            static constexpr bool is_promotable = PromotionRules<OtherFloatType>::is_promotable;

            template<typename OtherFloatType>
            using PromotedFloat = Float<typename PromotionRules<OtherFloatType>::promoted_type>;

        public:
            using Extractor = FloatExtractor<FloatType>;

            using format_type = typename Extractor::format_type;
            using storage_type = typename Extractor::storage_type;
            using exponent_type = typename Extractor::exponent_type;
            using fraction_type = typename Extractor::fraction_type;
            using float_type = FloatType;
            static constexpr auto fraction_bits = Extractor::fraction_bits;

        private:
            storage_type data_{};

        public:
            static constexpr auto exponent_bias = Extractor::exponent_bias;

            static constexpr Float zero() { return Float{}; }

            static constexpr Float negative_zero() { return -zero(); }

            static constexpr Float one() { return Float(0, exponent_bias, fraction_type{0}); }

            static constexpr Float infinity() { return Float(0, Extractor::exponent_max, fraction_type{0}); }

            static constexpr Float negative_infinity() { return -infinity(); }

            static constexpr Float qnan()
            {
                return Float(0, Extractor::exponent_max, fraction_type{}.set(Extractor::fraction_bits - 1));
            };

            static constexpr Float min_normal() { return Float(0, exponent_type{1}, fraction_type{0}); }

            static constexpr Float max_normal()
            {
                return Float(0, Extractor::exponent_max << 1, Extractor::fraction_max);
            };

            static constexpr Float lowest() { return -max_normal(); };

            constexpr Float() = default;

            constexpr Float(const bool sign, const exponent_type exponent,
                            const fraction_type fraction)
            {
                setSign(sign);
                setExponent(exponent);
                setFraction(fraction);
            }

            explicit constexpr Float(const FloatType& val) :
                Float(Extractor::fromFloat(val))
            {
            }

            explicit constexpr Float(const storage_type& val) :
                data_(val)
            {
            }

            explicit constexpr Float(storage_type&& val) :
                data_(std::move(val))
            {
            }

            template<typename OtherFloatType>
            explicit Float(const OtherFloatType& val) requires (!is_float_type<OtherFloatType> && (is_soft_float<OtherFloatType> || utils::any_of_v<OtherFloatType, float, double>)) :
                Float(FloatConverter<FloatType>::convert(val))
            {
            }

            template<typename OtherFloatType>
            explicit constexpr Float(const Float<OtherFloatType>& val) :
                Float(val.toFloat())
            {
            }

            constexpr void setSign(const bool sign)
            {
                Extractor::setSign(data_, sign);
            }

            constexpr void setExponent(const exponent_type exponent)
            {
                Extractor::setExponent(data_, exponent);
            }

            constexpr void setFraction(const fraction_type fraction)
            {
                Extractor::setFraction(data_, fraction);
            }

            constexpr FloatType toFloat() const
            {
                return Extractor::toFloat(data_);
            }

            explicit constexpr operator FloatType() const
            {
                return toFloat();
            }

            constexpr auto sign() const { return Extractor::sign(data_); }

            constexpr auto exponent() const { return Extractor::exponent(data_); }

            constexpr auto fraction() const { return Extractor::fraction(data_); }

            constexpr bool isZero() const { return exponent().none() && fractionIsZero_(); }

            constexpr bool isSubnormal() const { return exponent().none() && !fractionIsZero_(); }

            constexpr bool isInf() const { return exponent() == Extractor::exponent_max && fractionIsZero_(); }

            constexpr bool isNaN() const { return exponent() == Extractor::exponent_max && !fractionIsZero_(); }

            constexpr auto operator+() const
            {
                return *this;
            }

            constexpr Float operator-() const
            {
                Float temp(*this);
                temp.setSign(!sign());
                return temp;
            }

            template<typename OtherFloatType>
            constexpr auto operator+(const Float<OtherFloatType>& rhs) const
            {
                return *this + rhs.toFloat();
            }

            template<typename OtherFloatType>
            constexpr auto operator+(const OtherFloatType& rhs) const requires (is_soft_float<OtherFloatType> || is_promotable<OtherFloatType>)
            {
                return PromotedFloat<OtherFloatType>(FloatOps::add(toFloat(), rhs));
            }

            template<typename OtherFloatType>
            constexpr auto operator-(const Float<OtherFloatType>& rhs) const
            {
                return *this - rhs.toFloat();
            }

            template<typename OtherFloatType>
            constexpr auto operator-(const OtherFloatType& rhs) const requires (is_soft_float<OtherFloatType> || is_promotable<OtherFloatType>)
            {
                return PromotedFloat<OtherFloatType>(FloatOps::sub(toFloat(), rhs));
            }

            template<typename OtherFloatType>
            constexpr auto operator*(const Float<OtherFloatType>& rhs) const
            {
                return *this * rhs.toFloat();
            }

            template<typename OtherFloatType>
            constexpr auto operator*(const OtherFloatType& rhs) const requires (is_soft_float<OtherFloatType> || is_promotable<OtherFloatType>)
            {
                return PromotedFloat<OtherFloatType>(FloatOps::mul(toFloat(), rhs));
            }

            template<typename OtherFloatType>
            constexpr auto operator/(const Float<OtherFloatType>& rhs) const
            {
                return *this / rhs.toFloat();
            }

            template<typename OtherFloatType>
            constexpr auto operator/(const OtherFloatType& rhs) const requires (is_soft_float<OtherFloatType> || is_promotable<OtherFloatType>)
            {
                return PromotedFloat<OtherFloatType>(FloatOps::div(toFloat(), rhs));
            }

            template<typename OtherFloatType>
            constexpr Float& operator+=(const Float<OtherFloatType>& rhs) requires(is_promotable<OtherFloatType>)
            {
                *this = *this + rhs;
                return *this;
            }

            template<typename OtherFloatType>
            constexpr Float& operator+=(const OtherFloatType& rhs) requires(is_promotable<OtherFloatType>)
            {
                *this = *this + rhs;
                return *this;
            }

            template<typename OtherFloatType>
            constexpr Float& operator-=(const Float<OtherFloatType>& rhs) requires(is_promotable<OtherFloatType>)
            {
                *this = *this - rhs;
                return *this;
            }

            template<typename OtherFloatType>
            constexpr Float& operator-=(const OtherFloatType& rhs) requires(is_promotable<OtherFloatType>)
            {
                *this = *this - rhs;
                return *this;
            }

            template<typename OtherFloatType>
            constexpr Float& operator*=(const Float<OtherFloatType>& rhs) requires(is_promotable<OtherFloatType>)
            {
                *this = *this * rhs;
                return *this;
            }

            template<typename OtherFloatType>
            constexpr Float& operator*=(const OtherFloatType& rhs) requires(is_promotable<OtherFloatType>)
            {
                *this = *this * rhs;
                return *this;
            }

            template<typename OtherFloatType>
            constexpr Float& operator/=(const Float<OtherFloatType>& rhs) requires(is_promotable<OtherFloatType>)
            {
                *this = *this / rhs;
                return *this;
            }

            template<typename OtherFloatType>
            constexpr Float& operator/=(const OtherFloatType& rhs) requires(is_promotable<OtherFloatType>)
            {
                *this = *this / rhs;
                return *this;
            }

            constexpr Float& operator++()
            {
                *this = *this + one();
                return *this;
            }

            constexpr Float operator++(int)
            {
                Float temp(*this);
                ++(*this);
                return temp;
            }

            constexpr Float& operator--()
            {
                *this = *this - one();
                return *this;
            }

            constexpr Float operator--(int)
            {
                Float temp(*this);
                --(*this);
                return temp;
            }

            template<typename OtherFloatType>
            constexpr bool operator==(const Float<OtherFloatType>& rhs) const
            {
                return *this == rhs.toFloat();
            }

            template<typename OtherFloatType>
            constexpr bool operator==(const OtherFloatType& rhs) const requires is_soft_float<OtherFloatType>
            {
                return FloatOps::eq(toFloat(), rhs);
            }

            template<typename OtherFloatType>
            constexpr bool operator!=(const Float<OtherFloatType>& rhs) const
            {
                return !(*this == rhs);
            }

            template<typename OtherFloatType>
            constexpr bool operator!=(const OtherFloatType& rhs) const requires is_soft_float<OtherFloatType>
            {
                return !(*this == rhs);
            }

            template<typename OtherFloatType>
            constexpr bool operator<(const Float<OtherFloatType>& rhs) const
            {
                return *this < rhs.toFloat();
            }

            template<typename OtherFloatType>
            constexpr bool operator<(const OtherFloatType& rhs) const requires is_soft_float<OtherFloatType>
            {
                return FloatOps::lt(toFloat(), rhs);
            }

            template<typename OtherFloatType>
            constexpr bool operator<=(const Float<OtherFloatType>& rhs) const
            {
                return *this <= rhs.toFloat();
            }

            template<typename OtherFloatType>
            constexpr bool operator<=(const OtherFloatType& rhs) const requires is_soft_float<OtherFloatType>
            {
                return FloatOps::le(toFloat(), rhs);
            }

            template<typename OtherFloatType>
            constexpr bool operator>(const Float<OtherFloatType>& rhs) const
            {
                return rhs < *this;
            }

            template<typename OtherFloatType>
            constexpr bool operator>(const OtherFloatType& rhs) const requires is_soft_float<OtherFloatType>
            {
                return rhs < *this;
            }

            template<typename OtherFloatType>
            constexpr bool operator>=(const Float<OtherFloatType>& rhs) const
            {
                return rhs <= *this;
            }

            template<typename OtherFloatType>
            constexpr bool operator>=(const OtherFloatType& rhs) const requires is_soft_float<OtherFloatType>
            {
                return rhs <= *this;
            }

            explicit constexpr operator float() const
            {
                return FloatConverter<float>::convert(toFloat());
            }

            explicit constexpr operator double() const
            {
                return FloatConverter<double>::convert(toFloat());
            }

            explicit constexpr operator int() const
            {
                return FloatConverter<int>::convert(toFloat());
            }

            constexpr auto toHardFloat() const
            {
                return FloatOps::toHardFloat(toFloat());
            }

            void formatHex(std::ostream & os) const
            {
                constexpr auto chunk_size = utils::num_bits<unsigned long long>;

                constexpr auto format_chunk = [&os](const storage_type& data)
                {
#ifdef MAVIS_HAS_STD_FORMAT
                    os << std::format("{:x}", static_cast<typename Extractor::bits_type>(data.to_ullong()));
#else
                    os << boost::format("%|x|") % static_cast<typename Extractor::bits_type>(data.to_ullong());
#endif
                };

                if constexpr(Extractor::bits <= chunk_size)
                {
                    format_chunk(data_);
                }
                else
                {
                    constexpr auto mask = Extractor::template genBitMask_<chunk_size, Extractor::bits>();

                    for(size_t shift = chunk_size; shift <= Extractor::bits; shift += chunk_size)
                    {
                        format_chunk((data_ >> (Extractor::bits - shift)) & mask);
                    }
                }
            }
    };

    template<typename FloatType>
    std::ostream& operator<<(std::ostream& os, const Float<FloatType>& rhs)
    {
        FloatOps::format(os, rhs.toFloat());
        return os;
    }

    template<int power, const bool scale_down>
    void scale10(Float<float128_t>& val, int32_t& exponent)
    {
        static_assert(power > 0);

        static const Float<float128_t> constant(std::pow(10, scale_down ? -power : power));

        const auto comp = [](const Float<float128_t>& val)
        {
            if constexpr(scale_down)
            {
                static const Float<float128_t> comp_constant(std::pow(10, power));
                return val >= comp_constant;
            }
            else
            {
                static const Float<float128_t> comp_constant(std::pow(10, -power + 1));
                return val < comp_constant;
            }
        };

        while(comp(val))
        {
            val *= constant;

            if constexpr(scale_down)
            {
                exponent += power;
            }
            else
            {
                exponent -= power;
            }
        }

        if constexpr(power > 1)
        {
            scale10<power - 1, scale_down>(val, exponent);
        }
    }

    inline std::pair<Float<float128_t>, int32_t> scientificScale(Float<float128_t> val)
    {
        static constexpr int START_POWER = 8;

        // Exponent is positive -> divide by 10 until we hit the range [1, 10)
        // Exponent is negative -> multiply by 10 until we hit the range [1, 10)
        const bool scale_down = val.exponent().to_ullong() >= val.exponent_bias.to_ullong();

        int32_t exponent = 0;

        if(scale_down)
        {
            scale10<START_POWER, true>(val, exponent);
        }
        else
        {
            scale10<START_POWER, false>(val, exponent);
        }

        return std::make_pair(val, exponent);
    }

    template<>
    std::ostream& operator<<(std::ostream& os, const Float<float128_t>& rhs)
    {
        // Only deal with absolute values to keep the code simple
        if(rhs.sign())
        {
            os << '-' << -rhs;
        }
        else
        {
            bool fits_in_double;

            // Zero/inf/NaN can all be converted directly to double
            if(rhs.isZero() || rhs.isInf() || rhs.isNaN())
            {
                fits_in_double = true;
            }
            else
            {
                fits_in_double = rhs >= Float<float64_t>::min_normal() && rhs <= Float<float64_t>::max_normal();
            }

            // If it fits in a double, just convert it to a double
            if(fits_in_double)
            {
                os << static_cast<double>(rhs);
            }
            else
            {
                // The value is too large/small to convert directly to a double.
                // Instead, do a naive conversion to scientific notation, scaling
                // the value by powers of 10 until it falls into the range [1, 10).
                // Then we can convert *that* value to a double.
                // This loses precision, but we still get a double's worth of correct
                // fractional digits and the correct exponent.
                const auto [scaled_val, exponent] = scientificScale(rhs);

                os << static_cast<double>(scaled_val) << 'e' << exponent;
            }
        }

        return os;
    }

    namespace float_utils
    {
        template<typename SoftFloatType>
        inline void formatFloat(std::ostream& os, const SoftFloatType& value) requires (is_soft_float<SoftFloatType>)
        {
            FloatOps::format(os, value);
        }
    }

    using Float16 = Float<float16_t>;
    using BFloat16 = Float<bfloat16_t>;
    using Float32 = Float<float32_t>;
    using Float64 = Float<float64_t>;
    using Float128 = Float<float128_t>;
} // namespace mavis

#ifdef MAVIS_HAS_STD_FORMAT
template <typename FloatType>
struct std::formatter<mavis::Float<FloatType>> :
    std::formatter<typename mavis::Float<FloatType>::format_type>
{
    constexpr auto parse(std::format_parse_context & ctx)
    {
        return std::formatter<typename mavis::Float<FloatType>::format_type>::parse(
            ctx);
    }

    auto format(const mavis::Float<FloatType> & obj, std::format_context & ctx) const
    {
        return std::formatter<typename mavis::Float<FloatType>::format_type>::format(
            obj.toHardFloat(), ctx);
    }
};
#endif
