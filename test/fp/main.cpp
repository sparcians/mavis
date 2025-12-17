#include "mavis/Float.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <numbers>

#include <boost/core/demangle.hpp>

template <typename... FloatTypes> struct FloatTester
{
    template <typename FloatType, typename float_t>
    static void checkResult(const FloatType & lhs, const float_t rhs)
    {
        const bool lhs_is_nan = lhs.isNaN();
        const bool rhs_is_nan = rhs != rhs;
        bool result;
        if (lhs_is_nan || rhs_is_nan)
        {
            result = lhs_is_nan && rhs_is_nan;
        }
        else
        {
            result = lhs == rhs;
        }
        std::cout << (result ? "PASSED" : "FAILED") << std::endl;
        assert(result);
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
        if constexpr (FloatLHS::supports_arithmetic_operations
                      && FloatRHS::supports_arithmetic_operations
                      && !(std::is_same_v<FloatLHS, mavis::BFloat16>
                           && std::is_same_v<FloatRHS, mavis::Float16>)
                      && !(std::is_same_v<FloatLHS, mavis::Float16>
                           && std::is_same_v<FloatRHS, mavis::BFloat16>))
        {
            using float_lhs_t = typename FloatLHS::float_type;
            using float_rhs_t = typename FloatRHS::float_type;

            std::vector<std::pair<float_lhs_t, float_rhs_t>> binary_op_values;

            for (const auto lhs : values)
            {
                for (const auto rhs : values)
                {
                    binary_op_values.emplace_back(
                        std::make_pair(lhs, static_cast<float_rhs_t>(rhs)));
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

            for (const auto [val1, val2] : binary_op_values)
            {
                const FloatLHS x(val1);
                const FloatRHS y(val2);

                {
                    // Test addition
                    print_binary_op(x, y, '+');
                    const auto z = x + y;
                    const auto expected = val1 + val2;
                    checkResult(z, expected);
                }

                {
                    // Test subtraction
                    print_binary_op(x, y, '-');
                    const auto z = x - y;
                    const auto expected = val1 - val2;
                    checkResult(z, expected);
                }

                {
                    // Test multiplication
                    print_binary_op(x, y, '*');
                    const auto z = x * y;
                    const auto expected = val1 * val2;
                    checkResult(z, expected);
                }

                {
                    // Test division
                    print_binary_op(x, y, '/');
                    const auto z = x / y;
                    const auto expected = val1 / val2;
                    checkResult(z, expected);
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

    template <typename FloatType> static void testType()
    {
        if constexpr (FloatType::supports_arithmetic_operations)
        {
            using float_t = typename FloatType::float_type;

            std::vector<float_t> test_values{float_t(1.0),
                                             float_t(-1.0),
                                             float_t(2.0),
                                             float_t(-2.0),
                                             float_t(0.5),
                                             float_t(-0.5),
                                             float_t(0.1),
                                             float_t(-0.1),
                                             float_t(std::numbers::e),
                                             float_t(std::numbers::phi),
                                             float_t(std::numbers::pi),
                                             float_t(std::numbers::sqrt2),
                                             float_t(FloatType::max_normal()),
                                             float_t(FloatType::min_normal()),
                                             float_t(FloatType::lowest()),
                                             float_t(FloatType::zero()),
                                             float_t(FloatType::negative_zero()),
                                             float_t(FloatType::qnan()),
                                             float_t(FloatType::infinity()),
                                             float_t(FloatType::negative_infinity())};

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
                    float_t expected = val + 1;
                    checkResult(x, expected);
                }

                {
                    // Test postfix increment
                    FloatType x(val);
                    print_unary_op(x, "++", false);
                    x++;
                    float_t expected = val + 1;
                    checkResult(x, expected);
                }

                {
                    // Test prefix decrement
                    FloatType x(val);
                    print_unary_op(x, "--", true);
                    --x;
                    float_t expected = val - 1;
                    checkResult(x, expected);
                }

                {
                    // Test postfix decrement
                    FloatType x(val);
                    print_unary_op(x, "--", false);
                    x--;
                    float_t expected = val - 1;
                    checkResult(x, expected);
                }

                {
                    // Test +
                    FloatType x(val);
                    print_unary_op(x, "+", true);
                    auto z = +x;
                    auto expected = +val;
                    checkResult(z, expected);
                }

                {
                    // Test -
                    FloatType x(val);
                    print_unary_op(x, "-", true);
                    auto z = -x;
                    auto expected = -val;
                    checkResult(z, expected);
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
