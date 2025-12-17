#include "mavis/Float.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <numbers>

#include <boost/core/demangle.hpp>

template <typename FloatType> struct Cast;

template <> struct Cast<mavis::Float16>
{
    using upcast1 = mavis::Float32;
    using upcast2 = mavis::Float64;
    using upcast3 = mavis::Float128;
    using downcast1 = void;
    using downcast2 = void;
    using downcast3 = void;
};

template <> struct Cast<mavis::Float32>
{
    using upcast1 = mavis::Float64;
    using upcast2 = mavis::Float128;
    using upcast3 = void;
    using downcast1 = mavis::Float16;
    using downcast2 = void;
    using downcast3 = void;
};

template <> struct Cast<mavis::Float64>
{
    using upcast1 = mavis::Float128;
    using upcast2 = void;
    using upcast3 = void;
    using downcast1 = mavis::Float32;
    using downcast2 = mavis::Float16;
    using downcast3 = void;
};

template <> struct Cast<mavis::Float128>
{
    using upcast1 = void;
    using upcast2 = void;
    using upcast3 = void;
    using downcast1 = mavis::Float64;
    using downcast2 = mavis::Float32;
    using downcast3 = mavis::Float16;
};

template <typename FloatType, typename float_t>
void checkResult(const FloatType & lhs, const float_t rhs)
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

template <typename... Args> void printHeader(Args &&... args)
{
    std::cout << "=========== ";
    (std::cout << ... << args);
    std::cout << " ===========" << std::endl;
}

template <typename FloatLHS, typename FloatRHS>
void testBinary(const std::vector<typename FloatLHS::float_type> & values)
{
    using float_lhs_t = typename FloatLHS::float_type;
    using float_rhs_t = typename FloatRHS::float_type;

    std::vector<std::pair<float_lhs_t, float_rhs_t>> binary_op_values;

    for (const auto lhs : values)
    {
        for (const auto rhs : values)
        {
            binary_op_values.emplace_back(std::make_pair(lhs, static_cast<float_rhs_t>(rhs)));
        }
    }

    if constexpr (std::is_same_v<FloatLHS, FloatRHS>)
    {
        printHeader("Testing binary operations between ",
                    boost::core::demangle(typeid(FloatLHS).name()));
    }
    else
    {
        printHeader("Testing binary operations between ",
                    boost::core::demangle(typeid(FloatLHS).name()), " and ",
                    boost::core::demangle(typeid(FloatRHS).name()));
    }

    const auto print_binary_op = [](const FloatLHS & lhs, const FloatRHS & rhs, const char op_char)
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

template <typename FloatType> void testType()
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

    printHeader("Testing unary operations for ", boost::core::demangle(typeid(FloatType).name()));

    const auto print_unary_op = [](const FloatType & val, const char* op_string, const bool prefix)
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
            auto expected = val + 1;
            checkResult(x, expected);
        }

        {
            // Test postfix increment
            FloatType x(val);
            print_unary_op(x, "++", false);
            x++;
            auto expected = val + 1;
            checkResult(x, expected);
        }

        {
            // Test prefix decrement
            FloatType x(val);
            print_unary_op(x, "--", true);
            --x;
            auto expected = val - 1;
            checkResult(x, expected);
        }

        {
            // Test postfix decrement
            FloatType x(val);
            print_unary_op(x, "--", false);
            x--;
            auto expected = val - 1;
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

    testBinary<FloatType, FloatType>(test_values);

    if constexpr (!std::is_same_v<typename Cast<FloatType>::upcast1, void>)
    {
        testBinary<FloatType, typename Cast<FloatType>::upcast1>(test_values);
    }

    if constexpr (!std::is_same_v<typename Cast<FloatType>::upcast2, void>)
    {
        testBinary<FloatType, typename Cast<FloatType>::upcast2>(test_values);
    }

    if constexpr (!std::is_same_v<typename Cast<FloatType>::upcast3, void>)
    {
        testBinary<FloatType, typename Cast<FloatType>::upcast3>(test_values);
    }

    if constexpr (!std::is_same_v<typename Cast<FloatType>::downcast1, void>)
    {
        testBinary<FloatType, typename Cast<FloatType>::downcast1>(test_values);
    }

    if constexpr (!std::is_same_v<typename Cast<FloatType>::downcast2, void>)
    {
        testBinary<FloatType, typename Cast<FloatType>::downcast2>(test_values);
    }

    if constexpr (!std::is_same_v<typename Cast<FloatType>::downcast3, void>)
    {
        testBinary<FloatType, typename Cast<FloatType>::downcast3>(test_values);
    }

    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    testType<mavis::Float16>();
    testType<mavis::Float32>();
    testType<mavis::Float64>();
    testType<mavis::Float128>();

    return 0;
}
