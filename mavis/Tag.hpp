#pragma once

#include "Pattern.hpp"

namespace mavis {

class Tag
{
public:
    explicit Tag(const std::string& t):
        t_string_(t), is_empty_(t.empty())
    {}
    Tag(const Tag&) = default;
    Tag& operator=(const Tag&) = default;

    bool isEmpty() const
    {
        return is_empty_;
    }

    bool match(const Pattern& p) const
    {
        return p.match(t_string_);
    }

    const std::string &getV() const {
        return t_string_;
    }

private:
    std::string t_string_;
    bool        is_empty_;
};

} // namespace mavis
