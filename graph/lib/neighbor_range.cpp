//
// Created by synis on 3/2/19.
//

#include "neighbor_range.hpp"

neighbor_range::neighbor_range(const graph& graph, vertex::id_type vertex) : m_graph(graph), m_vertex(m_graph[vertex]) {}

neighbor_range::const_iterator_type neighbor_range::cbegin() const
{
    return const_iterator_type{m_graph, m_vertex, m_vertex.get_incident_edges_including_inactive().cbegin()};
}

neighbor_range::const_iterator_type neighbor_range::cend() const
{
    return const_iterator_type{m_graph, m_vertex, m_vertex.get_incident_edges_including_inactive().cend()};
}

neighbor_range::const_iterator_type neighbor_range::begin() const
{
    return cbegin();
}

neighbor_range::const_iterator_type neighbor_range::end() const
{
    return cend();
}
