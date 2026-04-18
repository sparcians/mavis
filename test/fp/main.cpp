#include "mavis/Float.h"
#include <cstdlib>
#include <iostream>
#include <numbers>
#include <string>
#include <sstream>
#include <vector>

#include <boost/core/demangle.hpp>

#include "hard_float_types/BFloat.h"
#include "hard_float_types/Half.h"
#include "hard_float_types/Single.h"
#include "hard_float_types/Double.h"
#include "hard_float_types/Quad.h"

template<typename SoftFloatType>
struct HardFloatType;

template<>
struct HardFloatType<float16_t>
{
    using type = mavis::Half;
};

template<>
struct HardFloatType<bfloat16_t>
{
    using type = mavis::BFloat;
};

template<>
struct HardFloatType<float32_t>
{
    using type = mavis::Single;
};

template<>
struct HardFloatType<float64_t>
{
    using type = mavis::Double;
};

template<>
struct HardFloatType<float128_t>
{
    using type = mavis::Quad;
};

template<typename SoftFloatType>
struct HardFloatEnabled
{
    static constexpr bool enabled = !std::is_void_v<typename HardFloatType<SoftFloatType>::type>;
};

template <typename... FloatTypes> struct FloatTester
{
    template<typename float_t>
    static bool isNaN(const float_t& val)
    {
        if constexpr(mavis::is_soft_float<float_t>)
        {
            return !mavis::FloatOps::eq(val, val);
        }
        else
        {
            return val != val;
        }
    }

    template<typename FloatType, typename float_t>
    static bool isEqual(const FloatType& lhs, const float_t& rhs)
    {
        if constexpr(mavis::is_soft_float<float_t>)
        {
            return lhs == rhs;
        }
        else
        {
            bool result = isEqual(lhs, std::bit_cast<typename FloatType::float_type>(rhs));

#ifdef USING_SIMDE_BFLOAT16
            // simde has different rounding rules than softfloat for bfloat16...
            if constexpr(std::is_same_v<float_t, mavis::BFloat>)
            {
                if(!result)
                {
                    result |= lhs == std::bit_cast<typename FloatType::float_type>(static_cast<uint16_t>(std::bit_cast<uint16_t>(rhs) - 1));
                }
            }
#endif
            return result;
        }
    }

    static void printHFMessage()
    {
        std::cout << "Cross-checking with hard-float/library: ";
    }

    template <typename FloatType, typename float_t>
    static void checkResult(const FloatType & lhs, const float_t rhs)
    {
        const bool lhs_is_nan = lhs.isNaN();
        const bool rhs_is_nan = isNaN(rhs);
        bool result;
        if (lhs_is_nan || rhs_is_nan)
        {
            result = lhs_is_nan && rhs_is_nan;
        }
        else
        {
            result = isEqual(lhs, rhs);
        }
        std::cout << (result ? "PASSED" : "FAILED") << std::endl;
        if (!result)
        {
            std::ostringstream ss;
            ss << "result failed, expected ";
            mavis::float_utils::formatFloat(ss, rhs);
            ss << " got " << lhs;
            throw std::runtime_error(ss.str());
        }
    };

    template <typename... Args> static void printHeader(Args &&... args)
    {
        std::cout << "=========== ";
        (std::cout << ... << args);
        std::cout << " ===========" << std::endl;
    }

    template <typename Type> static const std::string & getTypeName()
    {
        static const std::string name = boost::core::demangle(typeid(Type).name());
        return name;
    }

    template <typename FloatLHS, typename FloatRHS>
    static void testBinary(const std::vector<typename FloatLHS::float_type> & values)
    {
        if constexpr (!(std::is_same_v<FloatLHS, mavis::BFloat16>
                           && std::is_same_v<FloatRHS, mavis::Float16>)
                      && !(std::is_same_v<FloatLHS, mavis::Float16>
                           && std::is_same_v<FloatRHS, mavis::BFloat16>))
        {
            using float_lhs_t = typename FloatLHS::float_type;
            using float_rhs_t = typename FloatRHS::float_type;

            constexpr bool hard_float_enabled = HardFloatEnabled<float_lhs_t>::enabled && HardFloatEnabled<float_rhs_t>::enabled;
            using hard_float_lhs_t = typename HardFloatType<float_lhs_t>::type;
            using hard_float_rhs_t = typename HardFloatType<float_rhs_t>::type;

            using vector_elem = std::conditional_t<hard_float_enabled, std::tuple<float_lhs_t, float_rhs_t, hard_float_lhs_t, hard_float_rhs_t>, std::tuple<float_lhs_t, float_rhs_t>>;

            const auto construct_vector_elem = [](const float_lhs_t& lhs, const float_lhs_t& rhs)
            {
                auto elem = std::make_tuple(lhs, mavis::FloatConverter<float_rhs_t>::convert(rhs));

                if constexpr(hard_float_enabled)
                {
                    return std::tuple_cat(
                        elem,
                        std::make_tuple(
                            std::bit_cast<hard_float_lhs_t>(lhs),
                            std::bit_cast<hard_float_rhs_t>(std::get<1>(elem))
                        )
                    );
                }
                else
                {
                    return elem;
                }
            };

            const auto check_hf = []<typename Op, typename Result>(Op&& op, const Result& result, const vector_elem& elem)
            {
                if constexpr(hard_float_enabled)
                {
                    const auto& val1_hf = std::get<2>(elem);
                    const auto& val2_hf = std::get<3>(elem);

                    const auto expected_hf = op(val1_hf, val2_hf);
                    printHFMessage();
                    checkResult(result, expected_hf);
                }
                else
                {
                    std::cout << "Skipping hard-float check" << std::endl;
                }
            };

            std::vector<vector_elem> binary_op_values;

            for (const auto lhs : values)
            {
                for (const auto rhs : values)
                {
                    binary_op_values.emplace_back(construct_vector_elem(lhs, rhs));
                }
            }

            if constexpr (std::is_same_v<FloatLHS, FloatRHS>)
            {
                printHeader("Testing binary operations between ", getTypeName<FloatLHS>());
            }
            else
            {
                printHeader("Testing binary operations between ", getTypeName<FloatLHS>(), " and ",
                            getTypeName<FloatRHS>());
            }

            const auto print_binary_op =
                [](const FloatLHS & lhs, const FloatRHS & rhs, const char op_char)
            { std::cout << "Testing " << lhs << ' ' << op_char << ' ' << rhs << ": "; };

            for (const auto & elem : binary_op_values)
            {
                const auto& val1 = std::get<0>(elem);
                const auto& val2 = std::get<1>(elem);

                const FloatLHS x(val1);
                const FloatRHS y(val2);

                {
                    // Test addition
                    print_binary_op(x, y, '+');
                    const auto z = x + y;
                    const auto expected = mavis::FloatOps::add(val1, val2);
                    checkResult(z, expected);

                    check_hf(std::plus{}, z, elem);
                }

                {
                    // Test subtraction
                    print_binary_op(x, y, '-');
                    const auto z = x - y;
                    const auto expected = mavis::FloatOps::sub(val1, val2);
                    checkResult(z, expected);

                    check_hf(std::minus{}, z, elem);
                }

                {
                    // Test multiplication
                    print_binary_op(x, y, '*');
                    const auto z = x * y;
                    const auto expected = mavis::FloatOps::mul(val1, val2);
                    checkResult(z, expected);

                    check_hf(std::multiplies{}, z, elem);
                }

                {
                    // Test division
                    print_binary_op(x, y, '/');
                    const auto z = x / y;
                    const auto expected = mavis::FloatOps::div(val1, val2);
                    checkResult(z, expected);

                    check_hf(std::divides{}, z, elem);
                }
            }
        }
        else
        {
            printHeader("Skipping binary tests for ", getTypeName<FloatLHS>(), " and ",
                        getTypeName<FloatRHS>());
        }
    }

    template <typename FloatLHS, typename FloatRHS, typename... OtherFloatRHS>
    static void testBinary(const std::vector<typename FloatLHS::float_type> & values)
    requires(sizeof...(OtherFloatRHS) > 0)
    {
        testBinary<FloatLHS, FloatRHS>(values);
        testBinary<FloatLHS, OtherFloatRHS...>(values);
    }

    struct PreInc
    {
        template<typename T>
        void operator()(T& val)
        {
            ++val;
        }
    };

    struct PostInc
    {
        template<typename T>
        void operator()(T& val)
        {
            val++;
        }
    };

    struct PreDec
    {
        template<typename T>
        void operator()(T& val)
        {
            --val;
        }
    };

    struct PostDec
    {
        template<typename T>
        void operator()(T& val)
        {
            val--;
        }
    };

    struct Identity
    {
        template<typename T>
        T operator()(const T& val)
        {
            return +val;
        }
    };

    template <typename FloatType> static void testType()
    {
        if constexpr (true)
        {
            using float_t = typename FloatType::float_type;

            constexpr bool hard_float_enabled = HardFloatEnabled<float_t>::enabled;
            using hard_float_t = typename HardFloatType<float_t>::type;

            const auto check_hf = []<typename Op, typename Result>(Op&& op, const Result& result, const float_t& val)
            {
                if constexpr(hard_float_enabled)
                {
                    auto val_hf = std::bit_cast<hard_float_t>(val);

                    if constexpr(std::is_void_v<std::invoke_result_t<Op, hard_float_t&>>)
                    {
                        op(val_hf);
                    }
                    else
                    {
                        val_hf = op(val_hf);
                    }
                    printHFMessage();
                    checkResult(result, val_hf);
                }
                else
                {
                    std::cout << "Skipping hard-float check" << std::endl;
                }
            };

            std::vector<float_t> test_values{mavis::FloatConverter<float_t>::convert(1.0),
                                             mavis::FloatConverter<float_t>::convert(-1.0),
                                             mavis::FloatConverter<float_t>::convert(2.0),
                                             mavis::FloatConverter<float_t>::convert(-2.0),
                                             mavis::FloatConverter<float_t>::convert(0.5),
                                             mavis::FloatConverter<float_t>::convert(-0.5),
                                             mavis::FloatConverter<float_t>::convert(0.1),
                                             mavis::FloatConverter<float_t>::convert(-0.1),
                                             mavis::FloatConverter<float_t>::convert(std::numbers::e),
                                             mavis::FloatConverter<float_t>::convert(std::numbers::phi),
                                             mavis::FloatConverter<float_t>::convert(std::numbers::pi),
                                             mavis::FloatConverter<float_t>::convert(std::numbers::sqrt2),
                                             FloatType::max_normal().toFloat(),
                                             FloatType::min_normal().toFloat(),
                                             FloatType::lowest().toFloat(),
                                             FloatType::zero().toFloat(),
                                             FloatType::negative_zero().toFloat(),
                                             FloatType::qnan().toFloat(),
                                             FloatType::infinity().toFloat(),
                                             FloatType::negative_infinity().toFloat()};

            printHeader("Testing unary operations for ", getTypeName<FloatType>());

            const auto print_unary_op =
                [](const FloatType & val, const char* op_string, const bool prefix)
            {
                std::cout << "Testing ";

                if (prefix)
                {
                    std::cout << op_string;
                }

                std::cout << '(' << val << ')';

                if (!prefix)
                {
                    std::cout << op_string;
                }

                std::cout << ": ";
            };

            for (const auto val : test_values)
            {
                {
                    // Test prefix increment
                    FloatType x(val);
                    print_unary_op(x, "++", true);
                    ++x;
                    float_t expected = mavis::FloatOps::add(val, 1);
                    checkResult(x, expected);

                    check_hf(PreInc{}, x, val);
                }

                {
                    // Test postfix increment
                    FloatType x(val);
                    print_unary_op(x, "++", false);
                    x++;
                    float_t expected = mavis::FloatOps::add(val, 1);
                    checkResult(x, expected);

                    check_hf(PostInc{}, x, val);
                }

                {
                    // Test prefix decrement
                    FloatType x(val);
                    print_unary_op(x, "--", true);
                    --x;
                    float_t expected = mavis::FloatOps::sub(val, 1);
                    checkResult(x, expected);

                    check_hf(PreDec{}, x, val);
                }

                {
                    // Test postfix decrement
                    FloatType x(val);
                    print_unary_op(x, "--", false);
                    x--;
                    float_t expected = mavis::FloatOps::sub(val, 1);
                    checkResult(x, expected);

                    check_hf(PostDec{}, x, val);
                }

                {
                    // Test +
                    FloatType x(val);
                    print_unary_op(x, "+", true);
                    auto z = +x;
                    auto expected = val;
                    checkResult(z, expected);

                    check_hf(Identity{}, z, val);
                }

                {
                    // Test -
                    FloatType x(val);
                    print_unary_op(x, "-", true);
                    auto z = -x;
                    auto expected = mavis::FloatOps::mul(val, -1);
                    checkResult(z, expected);

                    check_hf(std::negate{}, z, val);
                }
            }

            (testBinary<FloatType, FloatTypes>(test_values), ...);
        }
        else
        {
            printHeader("Skipping arithmetic tests for ", getTypeName<FloatType>());
            printHeader("Testing formatter for ", getTypeName<FloatType>());
            std::cout << FloatType::zero() << std::endl;
            std::cout << FloatType::negative_zero() << std::endl;
            std::cout << FloatType::infinity() << std::endl;
            std::cout << FloatType::negative_infinity() << std::endl;
            std::cout << FloatType::qnan() << std::endl;
            std::cout << FloatType::min_normal() << std::endl;
            std::cout << FloatType::max_normal() << std::endl;
            std::cout << FloatType::lowest() << std::endl;
        }

        std::cout << std::endl;
    }

    static void test() { (testType<FloatTypes>(), ...); }
};

int main(int argc, char* argv[])
{
    FloatTester<mavis::Float16, mavis::BFloat16, mavis::Float32, mavis::Float64,
                mavis::Float128>::test();
    return 0;
}
