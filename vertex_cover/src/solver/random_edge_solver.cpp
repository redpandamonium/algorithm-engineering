//
// Created by Leon Suchy on 28.10.18.
//

#include "../../include/solver/random_edge_solver.hpp"
#include <util.hpp>

random_edge_solver::random_edge_solver(graph_unit g) : k_solver(g)
{ }

bool random_edge_solver::solve(int k)
{
    if (k < 0)
        return false;

    // select edge
    edge e = find_edge();

    // edge invalid: no edge left in the graph to cover
    if (!m_graph.graph()[e.get_a()].valid() || !m_graph.graph()[e.get_b()].valid())
        return true;

    // egde covered by taking a
    m_graph.controller().include_into_vertex_cover(e.get_a());
    if (solve(k - 1))
        return true;
    m_graph.controller().exclude_from_vertex_cover(e.get_a());

    // egde covered by taking b
    m_graph.controller().include_into_vertex_cover(e.get_b());
    if (solve(k - 1))
        return true;
    m_graph.controller().exclude_from_vertex_cover(e.get_b());

    // edge cannot be covered
    return false;
}

edge random_edge_solver::find_edge()
{
    vertex::id_type v1 {};
    vertex::id_type v2 {};

    bool found = false;

    for (auto& vertex1 : m_graph.graph())
    {
        // vertex already in the VC
        if (vertex1.color() == vertex::color_true)
            continue;

        auto neighbors = vertex1.neighbors();


        for (auto neighbor : neighbors)
        {
            // a neighbor is covering this edge
            if (m_graph.graph()[neighbor].color() == vertex::color_true)
                continue;

            // we found an edge between
            v1 = vertex1.id();
            v2 = neighbor;
            found = true;
            break;
        }

        // we found an edge, stop looking at vertices
        if (found) break;
    }

    return edge(v1, v2);
}

uint32_t random_edge_solver::recursive_steps()
{
    return 0;
}

bool random_edge_solver::solve_for(int k)
{
    return false;
}
