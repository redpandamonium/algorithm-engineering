//
// Created by Leon Suchy on 28.10.18.
//

#include <algorithm> // max_element

#include "../../include/solver/max_degree_neighbor_rule_extend_solver.hpp"
#include <util.hpp>
#include "../../include/bounds/clique_cover_lb.hpp"
#include <hopcroft_karp_matching.hpp>
#include <solver/max_degree_neighbor_rule_extend_solver.hpp>

#include "../../include/vertex_cover_solver.hpp"
#include "../../include/mirrors.hpp"

bool max_degree_neighbor_rule_extend_solver::solve(int k)
{
    m_recursive_steps++;
    m_graph.controller().increment_recursion_depth();
    bool res = solve_step(k);
    m_graph.controller().decrement_recursion_depth();
    return res;
}

bool max_degree_neighbor_rule_extend_solver::kernel_edge_condition()
{
    return (m_graph.graph().num_edges() > (static_cast<graph::size_type>(m_graph.meta_data().vertex_cover_size()) * static_cast<graph::size_type>(m_graph.meta_data().vertex_cover_size())));
}


uint32_t max_degree_neighbor_rule_extend_solver::best_lowerbound()
{
    uint32_t best = 0;

    // calculate clique cover lower bound
    clique_cover_lb clique_cover_bound(m_graph, m_config.lower_bounds().clique_cover_orders);
    auto lower_clique = clique_cover_bound.calculate();
    best = lower_clique;

    // check if this lower bound is already enough
    if (m_graph.meta_data().vertex_cover_size() < static_cast<long>(best))
        return best;
    return best;
}


max_degree_neighbor_rule_extend_solver::max_degree_neighbor_rule_extend_solver(
        graph_unit g, const data_reduction_factory& data_reduction_factory, const configuration& config)
        : k_solver(g), m_recursive_steps(0), m_data_reduction_factory(data_reduction_factory),
          m_config(config)
{ }

uint32_t max_degree_neighbor_rule_extend_solver::recursive_steps()
{
    return m_recursive_steps;
}

bool max_degree_neighbor_rule_extend_solver::solve_for(int k)
{
    m_graph.controller().set_vertex_cover_size(k);
    return solve(k);
}

bool max_degree_neighbor_rule_extend_solver::branch_on(vertex::id_type v)
{
    if (take_v(v))
        return true;
    if (take_v_neighbors(v)) // tHis StaTeMenT cAn be sImPLifIed
        return true;

    return false;
}

bool max_degree_neighbor_rule_extend_solver::branch()
{
    std::vector<std::unique_ptr<subgraph>> comps = m_graph.controller().components();

    // only one component, proceed normally, free the unnecessary memory
    if (comps.size() <= 1)
    {
        comps.clear();
        if (solve(m_graph.meta_data().vertex_cover_size()))
            return true;
    }
    // multiple components, build graph units
    else
    {
        // sort by component size
        // smaller components are easier to solve so we can reduce k quickly
        std::sort(comps.begin(), comps.end(), [] (const auto& left, const auto& right) { return left->size() < right->size(); });
        for (auto& comp : comps)
        {
            if (!solve_component(*comp))
                return false;
        }

        merge_subgraph_solutions(comps);
        return true;
    }

    return false;
}

void max_degree_neighbor_rule_extend_solver::merge_subgraph_solutions(const std::vector<std::unique_ptr<subgraph>>& subgraphs)
{
    for (auto& component : subgraphs)
    {
        for (const auto& v : component->data().vertices)
        {
            if (util::is_in_vertex_cover(*component, v.id()))
            {
                vertex::id_type id = component->translate_to_super(v.id());
                if (id == vertex::invalid_id)
                    throw std::runtime_error("A subgraph's vertex cannot be inside the VC and not in the original graph.");
                m_graph.controller().revalidate(id);
                m_graph.controller().include_into_vertex_cover(id);
            }
        }
    }
}

vertex::id_type max_degree_neighbor_rule_extend_solver::select_vertex() const
{
    if (m_graph.graph().empty())
        return vertex::invalid_id;

    return m_graph.meta_data().max_heap().top();
}

bool max_degree_neighbor_rule_extend_solver::solve_step(int k)
{
    if (m_graph.meta_data().vertex_cover_size() < 0)
        return false;

    if (m_graph.graph().num_edges() == 0)
        return true;

    auto reductions = m_data_reduction_factory.build(m_graph);

    reductions->apply();

    // the data reductions have reduced the graph to a size in which we cannot find a vertex cover
    if (m_graph.meta_data().vertex_cover_size() < 0)
        return false;

    if (m_graph.graph().num_edges() == 0)
    {
        reductions->accept();
        return true;
    }

    // the graph has more than k^2 edges
    if (m_config.kernel_edge_condition_enabled())
    {
        if (kernel_edge_condition())
            return false;
    }

    if (m_graph.meta_data().vertex_cover_size() < static_cast<int>(best_lowerbound()))
        return false;

    vertex::id_type selected = select_vertex();

    if (selected == vertex::invalid_id)
        throw std::logic_error("Not finding a vertex to branch on should be impossible on non-empty graphs.");

    // branch
    if (branch_on(selected))
    {
        reductions->accept();
        return true;
    }

    return false;
}

bool max_degree_neighbor_rule_extend_solver::take_v(vertex::id_type v)
{
    std::vector<vertex::id_type> take;

    // if mirrors are applicable (enabled) search for them
    if (m_config.max_mirror_degree() >= m_graph.graph()[v].degree())
    {
        mirrors mirror_finder(m_graph);
        take = mirror_finder.find_mirrors(v);
    }
    else
    {
        take.push_back(v);
    }

    // Mirrors too big to take.
    if (take.size() > static_cast<unsigned long>(m_graph.meta_data().vertex_cover_size()))
        return false;

    // branch
    m_graph.controller().include_into_vertex_cover(take);
    if (branch())
        return true;
    m_graph.controller().exclude_from_vertex_cover(take);

    return false;
}

bool max_degree_neighbor_rule_extend_solver::take_v_neighbors(vertex::id_type v)
{
    // Take all neighbors of v into MVC.
    auto v_neighbors = m_graph.graph()[v].neighbors();

    // Too many neighbors. A vertex cover of size k is not possible.
    if (v_neighbors.size() > static_cast<unsigned long>(m_graph.meta_data().vertex_cover_size()))
        return false;

    m_graph.controller().include_into_vertex_cover(v_neighbors);
    m_graph.controller().invalidate(v);
    if (branch())
        return true;
    m_graph.controller().revalidate(v);
    m_graph.controller().exclude_from_vertex_cover(v_neighbors);

    return false;
}

bool max_degree_neighbor_rule_extend_solver::solve_component(subgraph& comp)
{
    // One component needed more than we have available. We cannot find a vertex cover of size k in that case.
    if (m_graph.meta_data().vertex_cover_size() < 0)
        return false;

    // build a solver for this component
    graph_unit_storage unit(comp);
    unit.controller.set_vertex_cover_size(m_graph.meta_data().vertex_cover_size());

    vertex_cover_solver solver
    (
        unit.unit,
        m_config,
        m_data_reduction_factory,
        vertex_cover_solver::bound(m_graph.meta_data().vertex_cover_size())
    );

    // solve
    auto k = solver.solve();
    m_recursive_steps += solver.recursive_steps();

    if (k == vertex_cover_solver::not_found)
        return false;

    // adjust cover size on success
    m_graph.controller().decrease_vertex_cover_size(k);
    return true;
}
