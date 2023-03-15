//
// Created by Leon Suchy on 27.10.18.
//

#include <iostream>
#include <fstream>
#include "../include/vertex_cover_solver.hpp"
#include "../include/solver/max_degree_neighbor_rule_extend_solver.hpp"

int vertex_cover_solver::solve()
{
    bool found = false;
    unsigned long k = 0;

    for (; k <= (m_bound.is_set() ? m_bound.get() : m_graph.graph().size()); ++k) {
        if (m_solver->solve_for(k))
        {
            found = true;
            break;
        }
    }

    if (!found)
        return not_found;

    return k;
}

int vertex_cover_solver::operator()()
{
    return solve();
}

int vertex_cover_solver::recursive_steps() const
{
    return m_solver->recursive_steps();
}

vertex_cover_solver::vertex_cover_solver(graph_unit unit, const configuration& config,
                                         const data_reduction_factory& reduction_factory,
                                         bound b)
    : m_graph(unit), m_config(config), m_reduction_factory(reduction_factory),
      m_solver(new max_degree_neighbor_rule_extend_solver(m_graph, m_reduction_factory, m_config)),
      m_bound(b)
{

}

vertex_cover_solver::bound::bound()
    : m_bound(no_bound)
{ }

vertex_cover_solver::bound::bound(int k)
    : m_bound(k)
{ }

bool vertex_cover_solver::bound::is_set() const
{
    return m_bound != no_bound;
}

int vertex_cover_solver::bound::get() const
{
    return m_bound;
}
