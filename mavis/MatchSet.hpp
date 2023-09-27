#pragma once

#include "json.hpp"
#include <set>

namespace mavis {

template<typename T>
class MatchSet
{
public:
    MatchSet():
        is_empty_(true)
    {}

    explicit MatchSet(const std::vector<std::string>& v):
        t_vect_(v.cbegin(), v.cend()), is_empty_(v.empty()), s_set_(v.cbegin(), v.cend())
    {}
    explicit MatchSet(const std::set<std::string>& s):
        t_vect_(s.cbegin(), s.cend()), is_empty_(s.empty()), s_set_(s)
    {}
    MatchSet(const MatchSet<T>&) = default;
    MatchSet<T>& operator=(const MatchSet<T>&) = default;

    //explicit MatchSet(const nlohmann::json& jobj):
        //MatchSet(jobj.get<const std::vector<std::string>>())
    //{}

    bool isEmpty() const
    {
        return is_empty_;
    }

    bool isMember(const std::string& s) const
    {
        return s_set_.find(s) != s_set_.cend();
    }

    const std::vector<T>& getV() const
    {
        return t_vect_;
    }

    void merge(const std::set<std::string>& other_set)
    {
        std::set<std::string> set_union;
        std::set_union(s_set_.cbegin(), s_set_.cend(), other_set.cbegin(), other_set.cend(),
                       std::inserter(set_union, set_union.cbegin()));
        *this = MatchSet<T>(set_union);
    }

    void merge(const MatchSet<T>& other)
    {
        merge(other.s_set_);
    }

    void merge(const std::vector<std::string>& s_list)
    {
        merge(std::set<std::string>(s_list.cbegin(), s_list.cend()));
    }

    template<typename P>
    bool matchAny(const P& p) const
    {
        return std::any_of(t_vect_.cbegin(), t_vect_.cend(),
                           [&](const auto& m) { return m.match(p); });
    }

    template<typename P>
    bool matchAll(const P& p) const
    {
        return std::all_of(t_vect_.cbegin(), t_vect_.cend(),
                           [&](const auto& m) { return m.match(p); });
    }

    template<typename P>
    bool matchAnyAny(const MatchSet<P>& mp) const
    {
        return std::any_of(t_vect_.cbegin(), t_vect_.cend(),
                           [&](const auto& m) { return std::any_of(mp.getV().cbegin(), mp.getV().cend(),
                                                                   [&](const auto p) { return m.match(p); }); });
    }

    template<typename P>
    bool matchAnyAll(const MatchSet<P>& mp) const
    {
        return std::any_of(t_vect_.cbegin(), t_vect_.cend(),
                           [&](const auto& m) { return std::all_of(mp.getV().cbegin(), mp.getV().cend(),
                                                                   [&](const auto p) { return m.match(p); }); });
    }

    template<typename P>
    bool matchAllAny(const MatchSet<P>& mp) const
    {
        return std::all_of(t_vect_.cbegin(), t_vect_.cend(),
                           [&](const auto& m) { return std::any_of(mp.getV().cbegin(), mp.getV().cend(),
                                                                   [&](const auto p) { return m.match(p); }); });
    }

    template<typename P>
    bool matchAllAll(const MatchSet<P>& mp) const
    {
        return std::all_of(t_vect_.cbegin(), t_vect_.cend(),
                           [&](const auto& m) { return std::all_of(mp.getV().cbegin(), mp.getV().cend(),
                                                                   [&](const auto p) { return m.match(p); }); });
    }

private:
    std::vector<T>          t_vect_;        // Vector of template type T objects from the string list at construction
    bool                    is_empty_;      // TRUE if MatchSet<T> is empty
    std::set<std::string>   s_set_;         // std::set from the string list at construction
};

} // namespace mavis