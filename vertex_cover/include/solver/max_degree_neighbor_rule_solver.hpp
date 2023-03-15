//
// Created by Leon Suchy on 28.10.18.
//

#ifndef ALGORITHM_ENGINEERING_MAX_DEGREE_NEIGHBOR_RULE_SOLVER_HPP
#define ALGORITHM_ENGINEERING_MAX_DEGREE_NEIGHBOR_RULE_SOLVER_HPP


#include "k_solver.hpp"

class max_degree_neighbor_rule_solver : public k_solver
{
public:

    max_degree_neighbor_rule_solver(graph_unit g);
    bool solve(int k) override;

    uint32_t recursive_steps() override;

    bool solve_for(int k) override;

private:


};


#endif //ALGORITHM_ENGINEERING_MAX_DEGREE_NEIGHBOR_RULE_SOLVER_HPP
