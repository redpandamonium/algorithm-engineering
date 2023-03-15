//
// Created by synis on 1/31/19.
//

#include <util.hpp>
#include "bounds/cycle_cover_lb.hpp"

cycle_cover_lb::cycle_cover_lb(graph_unit unit) : m_graph(unit) {}

uint32_t cycle_cover_lb::calculate()
{
    m_graph.controller().lp_update_matching();
     m_graph.controller().lp_matching();

    const auto l_r = m_graph.meta_data().left_right();
    const auto r_l = m_graph.meta_data().right_left();

    uint32_t lower_bound = 0;
    // cycle can be at most the size of the graph
    std::vector<vertex::id_type> cycle;
    uint32_t valid_until;
    std::vector<vertex::id_type> visited;
    visited.resize(m_graph.graph().size_including_inactive(), vertex::invalid_id);

    for (auto& v : m_graph.graph())
    {
        auto start = v.id();

        if (visited[start] != vertex::invalid_id)
            continue;

        auto u = start;
        valid_until = 0;
        do
        {
            visited[u] = start;
            auto matched = r_l[u];

            if (matched == vertex::invalid_id)
                break;

            cycle.emplace_back(matched);

            // check here if there is a short cut to the start to have a "backup" in case
            if (m_graph.graph().has_edge(matched, start))
                valid_until++;

            u = matched;
        } while (u != start);

        cycle.resize(valid_until + 1);
        // maybe check here if the cycle is a clique
        // also check if it can be split into to odd cycles if its of even size
        if (is_clique(cycle, visited))
        {
            lower_bound += cycle.size()-1;
        }
        else
        {
            lower_bound += (cycle.size() + 1) / 2;
        }

        cycle.clear();
    }

    return lower_bound;
}

bool cycle_cover_lb::is_clique(std::vector<vertex::id_type> cycle, const std::vector<vertex::id_type>& visited)
{
    bool clique = true;
    if (cycle.empty())
        return false;

    for (vertex::id_type v : cycle)
    {
        // count of same neighbors that would make up clique
        vertex::id_type c = 0;
        for (auto n : m_graph.graph()[v].neighbors())
            if (visited[v] == visited[n])
                c++;

        if (c != (cycle.size() - 1))
        {
            clique = false;
            break;
        }
    }
    return clique;
}

