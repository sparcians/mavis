#pragma once

#include <ostream>
#include "Format.h"

#include <bitset>

// std::bitset was not constexpr until recently
#ifdef __cpp_lib_constexpr_bitset
#define MAVIS_FLOAT_BITSET std::bitset
#else
// If std::bitset isn't constexpr, use bitset2 (which is)
#include "bitset2.hpp"
#define MAVIS_FLOAT_BITSET Bitset2::bitset2
#endif

#include "Utils.h"

namespace mavis::float_utils
{
    template<size_t N>
    using bitset = MAVIS_FLOAT_BITSET<N>;

    template<size_t N>
    class bitset_with_arithmetic : public bitset<N>
    {
        private:
            static constexpr auto ullong_bits_ = utils::num_bits<unsigned long long>;
            static constexpr bool fits_in_ullong_ = N <= ullong_bits_;

            static constexpr unsigned long long getChunk_(const bitset<N>& bits, const size_t start_bit) requires (!fits_in_ullong_)
            {
                constexpr auto MASK = bitset<N>{}.set() >> (N - ullong_bits_);
                return ((bits >> start_bit) & MASK).to_ullong();
            }

            template<typename CarryOp>
            constexpr bitset_with_arithmetic carryOp_(CarryOp&& op, const bitset<N>& rhs) const requires (!fits_in_ullong_)
            {
                bitset_with_arithmetic result;
                bool carry = false;

                for(size_t i = 0; i < N; i += ullong_bits_)
                {
                    const unsigned long long lhs_chunk = getChunk_(*this, i);
                    const unsigned long long rhs_chunk = getChunk_(rhs, i);

                    unsigned long long result_chunk;
                    carry = op(rhs_chunk, carry, &result_chunk);
                    carry |= op(lhs_chunk, result_chunk, &result_chunk);

                    result |= bitset_with_arithmetic{result_chunk} << i;
                }

                return result;
            }

            template<typename CarryOp>
            constexpr bitset_with_arithmetic carryOp_(CarryOp&& op, const unsigned long long rhs) const requires (!fits_in_ullong_)
            {
                bitset_with_arithmetic result;

                unsigned long long result_chunk;
                bool carry = op(getChunk_(*this, 0), rhs, &result_chunk);
                result |= bitset_with_arithmetic{result_chunk};

                for(size_t i = ullong_bits_; carry && (i < N); i += ullong_bits_)
                {
                    carry = op(getChunk_(*this, i), carry, &result_chunk);
                    result |= bitset_with_arithmetic{result_chunk} << i;
                }

                return result;
            }

        public:
            using bitset<N>::bitset;
            using bitset<N>::to_ullong;
            using bitset<N>::operator&=;
            using bitset<N>::operator|=;
            using bitset<N>::operator^=;
            using bitset<N>::operator~;
            using bitset<N>::operator<<=;
            using bitset<N>::operator>>=;

            constexpr bitset_with_arithmetic(const bitset_with_arithmetic&) = default;

            explicit constexpr bitset_with_arithmetic(const bitset<N>& rhs) :
                bitset<N>(rhs)
            {
            }

            constexpr bitset_with_arithmetic& operator=(const bitset_with_arithmetic&) = default;

            constexpr bitset_with_arithmetic& operator=(const bitset<N>& rhs)
            {
                bitset<N>::operator=(rhs);
                return *this;
            }

            constexpr bitset_with_arithmetic operator+(const bitset<N>& rhs) const requires (fits_in_ullong_)
            {
                return *this + rhs.to_ullong();
            }

            constexpr bitset_with_arithmetic operator+(const bitset<N>& rhs) const requires (!fits_in_ullong_)
            {
                return carryOp_([](const unsigned long long lhs, const unsigned long long rhs, unsigned long long* result) { return __builtin_uaddll_overflow(lhs, rhs, result); }, rhs);
            }

            constexpr bitset_with_arithmetic operator+(const unsigned long long rhs) const requires (fits_in_ullong_)
            {
                return bitset_with_arithmetic{to_ullong() + rhs};
            }

            constexpr bitset_with_arithmetic operator+(const unsigned long long rhs) const requires (!fits_in_ullong_)
            {
                return carryOp_([](const unsigned long long lhs, const unsigned long long rhs, unsigned long long* result) { return __builtin_uaddll_overflow(lhs, rhs, result); }, rhs);
            }

            constexpr bitset_with_arithmetic operator-(const bitset<N>& rhs) const requires (fits_in_ullong_)
            {
                return *this - rhs.to_ullong();
            }

            constexpr bitset_with_arithmetic operator-(const bitset<N>& rhs) const requires (!fits_in_ullong_)
            {
                return carryOp_([](const unsigned long long lhs, const unsigned long long rhs, unsigned long long* result) { return __builtin_usubll_overflow(lhs, rhs, result); }, rhs);
            }

            constexpr bitset_with_arithmetic operator-(const unsigned long long rhs) const requires (fits_in_ullong_)
            {
                return bitset_with_arithmetic{to_ullong() - rhs};
            }

            constexpr bitset_with_arithmetic operator-(const unsigned long long rhs) const requires (!fits_in_ullong_)
            {
                return carryOp_([](const unsigned long long lhs, const unsigned long long rhs, unsigned long long* result) { return __builtin_usubll_overflow(lhs, rhs, result); }, rhs);
            }

            constexpr bitset_with_arithmetic& operator+=(const bitset<N>& rhs)
            {
                *this = *this + rhs;
                return *this;
            }

            constexpr bitset_with_arithmetic& operator+=(const unsigned long long rhs)
            {
                *this = *this + rhs;
                return *this;
            }

            constexpr bitset_with_arithmetic& operator-=(const bitset<N>& rhs)
            {
                *this = *this - rhs;
                return *this;
            }

            constexpr bitset_with_arithmetic& operator-=(const unsigned long long rhs)
            {
                *this = *this - rhs;
                return *this;
            }
    };

    template <typename FloatType>
    inline void formatFloat(std::ostream & os, const FloatType & value)
    {
        os << value;
    }
} // namespace mavis::float_utils

#undef MAVIS_FLOAT_BITSET
