//
// Created by iarch on 12/15/18.
//

#ifndef ALGORITHM_ENGINEERING_MAX_DEGREE_HEURISTIC_HPP
#define ALGORITHM_ENGINEERING_MAX_DEGREE_HEURISTIC_HPP

#include <graph.hpp>
#include <graph_unit.hpp>

class max_degree_heuristic {
public:

    explicit max_degree_heuristic(graph_unit g);
    void compute_vertex_cover_heuristic();

    static constexpr const vertex::color_type taken = 0x2;

private:

    graph_unit m_graph;
};


#endif //ALGORITHM_ENGINEERING_MAX_DEGREE_HEURISTIC_HPP
