//
// Created by iarch on 11/11/18.
//

#ifndef ALGORITHM_ENGINEERING_KONIG_LP_SOLVER_HPP
#define ALGORITHM_ENGINEERING_KONIG_LP_SOLVER_HPP

#include <bipartite_graph.hpp>
#include <graph_unit.hpp>

class konig_lp_solver {
public:

    struct solution
    {
        std::vector<vertex::id_type> take;
        std::vector<vertex::id_type> drop;
        uint32_t lower_bound;
    };

    // TODO change into graph unit
    explicit konig_lp_solver (graph_unit g_unit, const std::vector<edge>& matching);

    solution solve() const;

private:
    graph_unit m_g_unit;
    const std::vector<edge> m_matching;
    //bool has_connection (vertex::id_type from, vertex::id_type to);
};


#endif //ALGORITHM_ENGINEERING_KONIG_LP_SOLVER_HPP
