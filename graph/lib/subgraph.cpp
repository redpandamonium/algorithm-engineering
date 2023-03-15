//
// Created by Leon Suchy on 23.11.18.
//

#include <subgraph.hpp>
#include <iostream>

#include "subgraph.hpp"
#include "util.hpp"

using graphlib::detail::util;

subgraph::subgraph(const graph& supergraph, const std::vector<vertex::id_type>& vertices)
    : graph(vertices.size()), m_supergraph(supergraph), m_backmap()
{
    std::map<vertex::id_type, vertex::id_type> from_supergraph;

    // insert vertices
    for (auto vertex : vertices)
    {
        // vertex not in supergraph
        if (supergraph.get_vertex_including_inactive(vertex) == nullptr)
            throw invalid_graph_operation("Vertices that induce a subgraph do not belong to the supergraph: " + std::to_string(vertex));

        // ignore invalid vertices
        if (!supergraph[vertex].valid())
            continue;

        // insert vertex, copy static info
        vertex::id_type id = add_vertex();;

        (*this)[id].color(supergraph[vertex].color());

        // map the original vertex to the new subgraph vertex
        from_supergraph.emplace(vertex, id);
    }

    // insert edges
    for (auto vertex : vertices)
    {
        // ignore invalid vertices
        if (!supergraph[vertex].valid())
            continue;

        add_incident_edges(from_supergraph, vertex);
    }

    // flip the mapping, from now on we only need the other direction
    m_backmap = util::flip(from_supergraph);
}

const graph& subgraph::supergraph() const
{
    return m_supergraph;
}

void subgraph::add_incident_edges(const std::map<vertex::id_type, vertex::id_type>& from_supergraph, vertex::id_type supergraph_vertex)
{
    // find vertex in the subgraph
    auto subgraph_vertex_it = from_supergraph.find(supergraph_vertex);
    if (subgraph_vertex_it == from_supergraph.end())
        throw std::logic_error("Vertex " + std::to_string(supergraph_vertex) + "was not inserted, but should have been.");

    vertex::id_type subgraph_vertex = subgraph_vertex_it->second;

    // find the vertex' neighbors
    auto supergraph_neighbors = m_supergraph[supergraph_vertex].neighbors();

    std::vector<vertex::id_type> subgraph_neighbors;
    for (auto supergraph_neighbor : supergraph_neighbors)
    {
        auto subgraph_neighbor_it = from_supergraph.find(supergraph_neighbor);
        if (subgraph_neighbor_it == from_supergraph.end())
            throw std::logic_error("Vertex " + std::to_string(supergraph_vertex) + "was not inserted, but should have been.");

        subgraph_neighbors.push_back(subgraph_neighbor_it->second);
    }

    // add edges
    for (auto subgraph_neighbor : subgraph_neighbors)
    {
        try // ignore warnings about double edges, they're not inserted anyway
        {
            add_edge(subgraph_vertex, subgraph_neighbor);
        } catch (const invalid_graph_operation& invalid_op_ex) { /* ignore */ }
    }
}

vertex::id_type subgraph::translate_to_super(vertex::id_type id) const
{
    auto it = m_backmap.find(id);
    if (it == m_backmap.end())
        return vertex::invalid_id;

    return it->second;
}

void subgraph::on_element_replace(vertex& replaced, vertex& replacer)
{
    transfer_id(replacer.id(), replaced.id());
}

void subgraph::on_element_remove(vertex& removed)
{
    auto it = m_backmap.find(removed.id());
    if (it != m_backmap.end())
        m_backmap.erase(it);
}

void subgraph::transfer_id(vertex::id_type from, vertex::id_type to)
{
    auto from_it = m_backmap.find(from);
    auto to_it = m_backmap.find(to);

    if (from_it == m_backmap.end() && to_it == m_backmap.end())
        return;

    // from is not in the backmap so to should not be after the transfer
    if (from_it == m_backmap.end())
    {
        m_backmap.erase(to_it);
        return;
    }

    // insert new value
    if (to_it == m_backmap.end())
    {
        m_backmap.emplace(to, from_it->second);
        return;
    }

    // swap
    vertex::id_type to_id = to_it->second;
    m_backmap[to] = from_it->second;
    m_backmap[from] = to_id;
}
