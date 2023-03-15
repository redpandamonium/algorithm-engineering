//
// Created by toad on 11/4/18.
//

#include "bounds/clique_cover_lb.hpp"
#include <util.hpp>
#include <bounds/clique_cover_lb.hpp>

clique_cover_lb::clique_cover_lb(graph_unit unit, order_options orders)
        : m_graph(unit), m_orders(orders), m_set(m_graph.graph().size_including_inactive())
{ }

uint32_t clique_cover_lb::calculate()
{
    std::vector<vertex::id_type> clique (m_graph.graph().size_including_inactive(), 0);
    std::vector<vertex::id_type> clique_size (m_graph.graph().size_including_inactive(), 0);
    std::vector<vertex::id_type> temp (m_graph.graph().size_including_inactive(), 0);

    if (m_orders.iteration_order == order::none)
        throw std::logic_error("Clique cover iteration order can not be none");

    if (m_orders.iteration_order == order::ascending)
    {
        for (auto vertex_id : m_graph.meta_data().min_heap())
            sort_into_clique(vertex_id, clique, clique_size, temp);
    }
    else
    {
        for (auto vertex_id : m_graph.meta_data().max_heap())
            sort_into_clique(vertex_id, clique, clique_size, temp);
    }

    u_int32_t lb = 0;

    for (const auto size : clique_size)
    {
        if (size == 0)
            continue;
        lb += size - 1;
    }

    return lb;
}

void clique_cover_lb::sort_into_clique(
        vertex::id_type v,
        std::vector<vertex::id_type>& clique,
        std::vector<vertex::id_type>& clique_size,
        std::vector<vertex::id_type>& temp
)
{
    vertex::id_type to = v;
    u_int64_t largest = 0;

    auto neighbors = m_graph.graph()[v].neighbors();

    // init temp vector for the cliques of all neighbours
    for (auto neighbor : neighbors)
        if (m_set.is_member(neighbor))
           temp[clique[neighbor]] = 0;

    for (auto neighbor : neighbors)
    {
        if (!m_set.is_member(neighbor))
            continue;

        // get the clique the neighbour belongs to
        vertex::id_type clique_index = clique[neighbor];
        temp[clique_index]++;

        if (temp[clique_index] == clique_size[clique_index] && largest < clique_size[clique_index])
        {
            // we can include v to the current neighbour
            to = clique_index;
            largest = clique_size[clique_index];
        }
    }

    clique[v] = to;
    if (to != v)
    {
        // increase size of clique v was added to
        clique_size[to]++;
    } else
    {
        // new clique
        clique_size[to] = 1;
    }

    m_set.add(v);
}
