//
// Created by Leon Suchy on 07.11.18.
//

#include "heap/heap_comparators.hpp"

min_comparator::min_comparator(const graph& graph)
    : m_graph(&graph)
{ }

// behave like !std::less
bool min_comparator::operator()(const vertex::id_type& first, const vertex::id_type& second) const
{
    return (*m_graph)[first].degree() > (*m_graph)[second].degree();
}

max_comparator::max_comparator(const graph& graph)
    : m_graph(&graph)
{ }

// behave like std::less
bool max_comparator::operator()(const vertex::id_type& first, const vertex::id_type& second) const
{
    return (*m_graph)[first].degree() < (*m_graph)[second].degree();
}
