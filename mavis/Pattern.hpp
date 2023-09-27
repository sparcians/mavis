#pragma once

#include <regex>

namespace mavis {

class Pattern
{
public:
    explicit Pattern(const std::string& p):
        p_string_(p), is_empty_(p.empty()), p_rex_(p, std::regex::optimize)
    {}
    Pattern(const Pattern&) = default;
    Pattern& operator=(const Pattern&) = default;

    bool isEmpty() const
    {
        return is_empty_;
    }

    bool match(const std::string& s) const
    {
        return std::regex_match(s, p_rex_, std::regex_constants::match_any);
    }

private:
    std::string   p_string_;
    bool          is_empty_;
    std::regex    p_rex_;
};
} // namespace mavis