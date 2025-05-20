#pragma once

#include <fstream>
#include <string_view>
#include <boost/json.hpp>

namespace mavis
{
    // Parses the JSON file at the given path
    inline boost::json::value parseJSON(const std::string& path)
    {
        std::ifstream fs;

#ifndef TARGET_OS_MAC
        // Enable failbit exceptions so we throw an
        // std::ifstream::failure exception if the open fails (not support on MacOS)
        std::ios_base::iostate exceptionMask = fs.exceptions() | std::ios::failbit;
        fs.exceptions(exceptionMask);
#endif

        fs.open(path);

#ifndef TARGET_OS_MAC
        // Turn fail exceptions off now that the open succeeded (not support on MacOS)
        exceptionMask &= ~std::ios::failbit;
        fs.exceptions(exceptionMask);
#endif
        boost::system::error_code ec;

#if (BOOST_VERSION / 100 >= 1081)
        // Boost v1.81+ has an override for boost::json::parse that takes an std::ifstream
        const boost::json::value json = boost::json::parse(fs, ec);

        if (json.is_null() || ec)
        {
            throw boost::system::system_error(ec);
        }
#else
        // For older versions we have to handle the std::ifstream ourselves
        boost::json::stream_parser parser;
        std::string buf;
        while(std::getline(fs, buf))
        {
            parser.write(buf);
        }
        parser.finish(ec);

        if(ec)
        {
            throw boost::system::system_error(ec);
        }

        const boost::json::value json = parser.release();
#endif
        return json;
    }

    // Attempts to parse the JSON file at the given path, throwing OpenFailedExceptionType
    // if there was an error opening the file
    template<typename OpenFailedExceptionType>
    inline boost::json::value parseJSONWithException(const std::string& path)
    {
        try
        {
            return parseJSON(path);
        }
        catch(const std::ifstream::failure&)
        {
            throw OpenFailedExceptionType(path);
        }
    }

    // Comparer object that allows using a boost::json::string to look up values in an std::map<std::string, T> without any copying overhead
    struct JSONStringMapCompare : public std::less<std::string>
    {
        // Indicates to the standard library that this comparator is able to perform heterogeneous comparisons without conversion
        // See "Transparent function objects" at https://en.cppreference.com/w/cpp/utility/functional
        // When is_transparent is defined to any type, overrides (3) and (4) of std::map::find (https://en.cppreference.com/w/cpp/container/map/find)
        // become available
        using is_transparent = void;

        // Inherit the existing std::less<std::string> operator so we can still compare an std::string against another std::string
        using std::less<std::string>::operator();

        // Both boost::json::string and std::string can be cheaply wrapped by std::string_view,
        // which provides the same < operator as std::string
        inline bool operator()(const boost::json::string& lhs, const std::string& rhs) const
        {
            return std::string_view(lhs) < std::string_view(rhs);
        }

        // Same as above but with left and right hand sides swapped
        inline bool operator()(const std::string& lhs, const boost::json::string& rhs) const
        {
            return std::string_view(lhs) < std::string_view(rhs);
        }
    };
}
