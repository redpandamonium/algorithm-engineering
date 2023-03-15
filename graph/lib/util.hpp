//
// Created by Leon Suchy on 23.11.18.
//

#ifndef ALGORITHM_ENGINEERING_UTIL_HPP
#define ALGORITHM_ENGINEERING_UTIL_HPP

#include <map>

namespace graphlib
{
namespace detail
{
    struct util
    {
        template<class U, class V>
        static std::map<U, V> flip(const std::map<V, U>& m);
    };

    template<class U, class V>
    std::map<U, V> util::flip(const std::map<V, U>& m)
    {
        std::map<U, V> result;
        for (auto entry : m)
            result.emplace(entry.second, entry.first);

        return result;
    }
} // namespace detail
} // namespace graphlib

#endif //ALGORITHM_ENGINEERING_UTIL_HPP
