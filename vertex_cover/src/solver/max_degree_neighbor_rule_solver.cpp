//
// Created by Leon Suchy on 28.10.18.
//

#include <algorithm> // max_element

#include "../../include/solver/max_degree_neighbor_rule_solver.hpp"
#include <util.hpp>

bool max_degree_neighbor_rule_solver::solve(int k)
{
    if (k < 0)
        return false;

    // NOTE this can potentially/definitely get improved by cunning heuristics
    auto v_it = std::max_element(m_graph.graph().begin(), m_graph.graph().end(), [](auto& a, auto& b){ return (a.degree() < b.degree());});
    if (v_it == m_graph.graph().end()) // G empty
        return true;
    if (v_it->degree() == 0)
        return true; // empty VC

    // Take v into MVC
    {
        m_graph.controller().include_into_vertex_cover(v_it->id());
        if (solve(k - 1))
            return true;
        m_graph.controller().exclude_from_vertex_cover(v_it->id());
    }

    // Take all neighbors of v into MVC
    auto v_neighbors = v_it->neighbors();
    {
        m_graph.controller().include_into_vertex_cover(v_neighbors);
        if (solve(static_cast<int>(k - v_neighbors.size())))
            return true;
        m_graph.controller().exclude_from_vertex_cover(v_neighbors);
    }

    return false;
}

max_degree_neighbor_rule_solver::max_degree_neighbor_rule_solver(graph_unit g)
    : k_solver(g)
{

}

uint32_t max_degree_neighbor_rule_solver::recursive_steps()
{
    return 0; // TODO: implement
}

bool max_degree_neighbor_rule_solver::solve_for(int k)
{
    return false;
}
