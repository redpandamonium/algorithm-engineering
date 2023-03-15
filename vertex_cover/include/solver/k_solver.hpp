//
// Created by Leon Suchy on 28.10.18.
//

#ifndef ALGORITHM_ENGINEERING_K_SOLVER_HPP
#define ALGORITHM_ENGINEERING_K_SOLVER_HPP

#include <graph.hpp>
#include <graph_unit.hpp>

class k_solver
{
public:

    explicit k_solver(graph_unit g);
    virtual ~k_solver() = default;
    virtual bool solve(int k) = 0;
    virtual bool solve_for(int k) = 0;

    virtual uint32_t recursive_steps() = 0;

protected:

    graph_unit m_graph;
};

#endif //ALGORITHM_ENGINEERING_K_SOLVER_HPP
