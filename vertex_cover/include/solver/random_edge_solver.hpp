//
// Created by Leon Suchy on 28.10.18.
//

#ifndef ALGORITHM_ENGINEERING_RANDOM_EDGE_SOLVER_HPP
#define ALGORITHM_ENGINEERING_RANDOM_EDGE_SOLVER_HPP


#include "k_solver.hpp"

class random_edge_solver : public k_solver
{
public:

    explicit random_edge_solver(graph_unit g);

    bool solve(int k) override;

    uint32_t recursive_steps() override;

    bool solve_for(int k) override;

private:

    edge find_edge();
};


#endif //ALGORITHM_ENGINEERING_RANDOM_EDGE_SOLVER_HPP
