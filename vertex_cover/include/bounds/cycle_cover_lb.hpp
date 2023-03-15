//
// Created by synis on 1/31/19.
//

#ifndef ALGORITHM_ENGINEERING_CYCLE_COVER_LB_HPP
#define ALGORITHM_ENGINEERING_CYCLE_COVER_LB_HPP


#include <graph_unit.hpp>

class cycle_cover_lb {
public:

    explicit cycle_cover_lb(graph_unit unit);
    uint32_t calculate();

private:

    graph_unit m_graph;

    bool is_clique(std::vector<vertex::id_type> cycle, const std::vector<vertex::id_type>& visited);
};


#endif //ALGORITHM_ENGINEERING_CYCLE_COVER_LB_HPP
