//
// Created by synis on 2/17/19.
//

#include <stdexcept>
#include "fast_set.hpp"

fast_set::fast_set(size_t size) : m_set_id(initial_set_id), m_size(size), m_set(size, initial_set_value) {}

fast_set::fast_set(size_t size, size_t set_id) : m_set_id(set_id), m_size(size), m_set(size, initial_set_value) {}

bool fast_set::add(size_t elem)
{
    // REVIEW maybe throw exception here
    if (elem >= m_size)
        return false;

    bool member = !is_member(elem);
    m_set[elem] = m_set_id;
    return member;
}

void fast_set::remove(size_t elem)
{
    // REVIEW maybe throw exception here
    if (elem >= m_size)
        return;

    m_set[elem] = m_set_id - 1;
}

bool fast_set::is_member(size_t elem) const
{
    // REVIEW maybe throw exception here
    if (elem >= m_size)
        return false;

    return m_set[elem] == m_set_id;
}

void fast_set::clear()
{
    // the current set id is at the maximum
    if (m_set_id == m_max_id)
    {
       for (auto& elem : m_set)
           elem = initial_set_value;
       m_set_id = initial_set_id;
       return;
    }

    m_set_id++;
}

bool fast_set::operator[](size_t elem)
{
    return add(elem);
}