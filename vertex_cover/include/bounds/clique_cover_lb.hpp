//
// Created by toad on 11/4/18.
//

#ifndef ALGORITHM_ENGINEERING_CLIQUE_COVER_LB_HPP
#define ALGORITHM_ENGINEERING_CLIQUE_COVER_LB_HPP

#include <graph.hpp>
#include <fast_set.hpp>
#include "graph_unit.hpp"

class clique_cover_lb
{
public:

    enum class order { none, ascending, descending };

    struct order_options
    {
        order iteration_order;
        order neighbor_iteration_order;
    };

    explicit clique_cover_lb(graph_unit unit, order_options orders);

    uint32_t calculate();

private:

    graph_unit m_graph;
    order_options m_orders;

    fast_set m_set;

    void sort_into_clique(vertex::id_type v, std::vector<vertex::id_type>& clique, std::vector<vertex::id_type>& clique_size, std::vector<vertex::id_type>& temp);
};


#endif //ALGORITHM_ENGINEERING_CLIQUE_COVER_LB_HPP
