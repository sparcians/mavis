#include "mavis/Float.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

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
                                     float_t(3.14159),
                                     float_t(FloatType::max_normal()),
                                     float_t(FloatType::min_normal()),
                                     float_t(FloatType::lowest()),
                                     float_t(FloatType::zero()),
                                     float_t(FloatType::negative_zero()),
                                     float_t(FloatType::qnan()),
                                     float_t(FloatType::infinity()),
                                     float_t(FloatType::negative_infinity())};

    std::vector<std::pair<float_t, float_t>> binary_op_values;

    for (const auto lhs : test_values)
    {
        for (const auto rhs : test_values)
        {
            binary_op_values.emplace_back(std::make_pair(lhs, rhs));
        }
    }

    const auto check_result = [](const FloatType lhs, const float_t rhs)
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

    for (const auto val : test_values)
    {
        {
            // Test increment
            FloatType x(val);
            std::cout << "Testing ++(" << x << "): ";
            ++x;
            auto expected = val + 1;
            check_result(x, expected);
        }

        {
            // Test decrement
            FloatType x(val);
            std::cout << "Testing --(" << x << "): ";
            --x;
            auto expected = val - 1;
            check_result(x, expected);
        }
    }

    for (const auto [val1, val2] : binary_op_values)
    {
        {
            // Test addition
            FloatType x(val1);
            FloatType y(val2);
            std::cout << "Testing " << x << " + " << y << ": ";
            auto z = x + y;
            auto expected = val1 + val2;
            check_result(z, expected);
        }

        {
            // Test subtraction
            FloatType x(val1);
            FloatType y(val2);
            std::cout << "Testing " << x << " - " << y << ": ";
            auto z = x - y;
            auto expected = val1 - val2;
            check_result(z, expected);
        }

        {
            // Test multiplication
            FloatType x(val1);
            FloatType y(val2);
            std::cout << "Testing " << x << " * " << y << ": ";
            auto z = x * y;
            auto expected = val1 * val2;
            check_result(z, expected);
        }

        {
            // Test division
            FloatType x(val1);
            FloatType y(val2);
            std::cout << "Testing " << x << " / " << y << ": ";
            auto z = x / y;
            auto expected = val1 / val2;
            check_result(z, expected);
        }
    }
}

int main(int argc, char* argv[])
{
    testType<mavis::Float16>();
    testType<mavis::Float32>();
    testType<mavis::Float64>();
    testType<mavis::Float128>();

    return 0;
}
