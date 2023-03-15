//
// Created by Leon Suchy on 17.11.18.
//

#include "../include/graph.hpp"
#include "graph_iterator.hpp"

/*
 * GRAPH ITERATOR
 * ---------------------------------------------------------------------------------------------------------------------
 */

const graph_iterator graph_iterator::operator++(int offset)
{
    graph_iterator it(*this);
    for (auto i = 0; i < offset + 1; i++)
    {
        ++(*this);
    }
    return it;
}

graph_iterator& graph_iterator::operator++()
{
    if (*this == m_graph->end())
        return *this;

    // increment until we hit the end or find a valid spot
    do
    {
        m_index++;
    } while (*this != m_graph->end() && !(*m_graph).data().vertices[m_index].valid());

    return *this;
}

vertex* graph_iterator::operator->() const
{
    return &(*m_graph).m_data.vertices[m_index];
}

vertex& graph_iterator::operator*() const
{
    return *operator->();
}

graph_iterator::graph_iterator(graph& graph, graph::size_type index)
        : m_graph(&graph), m_index(index)
{
    while (m_index < (*m_graph).data().vertices.size() && !(*m_graph).data().vertices[m_index].valid())
        m_index++;
}

bool graph_iterator::operator==(const graph_iterator& rhs) const
{
    return m_index == rhs.m_index;
}

bool graph_iterator::operator!=(const graph_iterator& rhs) const
{
    return !operator==(rhs);
}

/*
 * CONST GRAPH ITERATOR
 * ---------------------------------------------------------------------------------------------------------------------
 */

const_graph_iterator::const_graph_iterator(const graph& graph, const_graph_iterator::size_type index)
        : m_graph(&graph), m_index(index)
{
    while (m_index < (*m_graph).data().vertices.size() && !(*m_graph).data().vertices[m_index].valid())
        m_index++;
}

const vertex& const_graph_iterator::operator*() const
{
    return *operator->();
}

const vertex* const_graph_iterator::operator->() const
{
    return &(*m_graph).data().vertices[m_index];
}

const_graph_iterator& const_graph_iterator::operator++()
{
    if (*this == m_graph->cend())
        return *this;

    // increment until we hit the end or find a valid spot
    do
    {
        m_index++;
    } while (*this != m_graph->cend() && !(*m_graph).data().vertices[m_index].valid());

    return *this;
}

const const_graph_iterator const_graph_iterator::operator++(int offset)
{
    const_graph_iterator it(*this);
    for (auto i = 0; i < offset; i++)
    {
        ++(*this);
    }
    return it;
}

bool const_graph_iterator::operator==(const const_graph_iterator& rhs) const
{
    return m_index == rhs.m_index;
}

bool const_graph_iterator::operator!=(const const_graph_iterator& rhs) const
{
    return !operator==(rhs);
}
