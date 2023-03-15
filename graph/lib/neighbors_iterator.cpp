//
// Created by synis on 3/2/19.
//

#include "neighbors_iterator.hpp"

neighbors_iterator::neighbors_iterator(graph& graph, vertex &vertex, neighbors_iterator::container_iterator_type it) : m_graph(graph), m_vertex(vertex), m_it(it)
{
}

neighbors_iterator& neighbors_iterator::operator++()
{

    if (m_it == m_vertex.get_incident_edges_including_inactive().end())
        return *this;

    do
    {
        m_it++;
    } while(!m_graph[(*m_it).get_to()].valid() && m_it != m_vertex.get_incident_edges_including_inactive().end());

    return *this;
}

const neighbors_iterator neighbors_iterator::operator++(int offset)
{
    neighbors_iterator it(*this);

    for (auto i = 0; i <= offset; i++)
    {
        ++(*this);
    }
    return it;
}

vertex* neighbors_iterator::operator->() const
{
    return &m_graph[(*m_it).get_to()];
}

vertex& neighbors_iterator::operator*() const
{
    return *operator->();
}

bool neighbors_iterator::operator==(const neighbors_iterator &n) const
{
    return m_it == n.m_it;
}

bool neighbors_iterator::operator!=(const neighbors_iterator &n) const
{
    return m_it != n.m_it;
}

// CONST
const_neighbors_iterator::const_neighbors_iterator(const graph &graph, const vertex &vertex, const_neighbors_iterator::container_iterator_type it) :
    m_graph(graph),
    m_vertex(vertex),
    m_it(it)
{
    while (m_it != m_vertex.get_incident_edges_including_inactive().cend() && !m_graph[(*m_it).get_to()].valid())
        m_it++;
}

const_neighbors_iterator& const_neighbors_iterator::operator++()
{
    if (m_it == m_vertex.get_incident_edges_including_inactive().cend())
        return *this;

    do
    {
        m_it++;
    } while(m_it != m_vertex.get_incident_edges_including_inactive().cend() && !m_graph[(*m_it).get_to()].valid());

    return *this;
}

const const_neighbors_iterator const_neighbors_iterator::operator++(int offset)
{
    const_neighbors_iterator it(*this);

    for (auto i = 0; i <= offset; i++)
    {
        ++(*this);
    }
    return it;
}

const vertex* const_neighbors_iterator::operator->() const
{
    return &m_graph[(*m_it).get_to()];
}

const vertex& const_neighbors_iterator::operator*() const
{
    return *operator->();
}

bool const_neighbors_iterator::operator==(const const_neighbors_iterator &n) const
{
    return m_it == n.m_it;
}

bool const_neighbors_iterator::operator!=(const const_neighbors_iterator &n) const
{
    return !operator==(n);
}