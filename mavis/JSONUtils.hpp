#pragma once

#include <fstream>
#include <string_view>
#include <boost/json.hpp>

namespace mavis
{
    inline boost::json::value parseJSON(const std::string& path)
    {
        std::ifstream fs;

        // Enable failbit exceptions so we throw an std::ifstream::failure exception if the open fails
        std::ios_base::iostate exceptionMask = fs.exceptions() | std::ios::failbit;
        fs.exceptions(exceptionMask);
        fs.open(path);
        // Turn fail exceptions off now that the open succeeded
        exceptionMask &= ~std::ios::failbit;
        fs.exceptions(exceptionMask);

        boost::system::error_code ec;

#if (BOOST_VERSION / 100 >= 1081)
        const boost::json::value json = boost::json::parse(fs, ec);

        if (json.is_null() || ec)
        {
            throw boost::system::system_error(ec);
        }
#else
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

    struct JSONStringMapCompare : public std::less<std::string>
    {
        using is_transparent = void;

        using std::less<std::string>::operator();

        inline bool operator()(const boost::json::string& lhs, const std::string& rhs) const
        {
            return std::string_view(lhs) < std::string_view(rhs);
        }

        inline bool operator()(const std::string& lhs, const boost::json::string& rhs) const
        {
            return std::string_view(lhs) < std::string_view(rhs);
        }
    };
}
