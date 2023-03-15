//
// Created by synis on 2/21/19.
//

#ifndef ALGORITHM_ENGINEERING_EXHAUSTIVE_ITERATIVE_CONTAINER_HPP
#define ALGORITHM_ENGINEERING_EXHAUSTIVE_ITERATIVE_CONTAINER_HPP


#include "data_reduction_container.hpp"

class exhaustive_iterative_container : public data_reduction_container {
public:

    exhaustive_iterative_container(graph_unit unit, std::vector<std::unique_ptr<data_reduction_node>>&& children, unsigned frequency);

    std::string name() const override;

    bool apply(graph_operation::stack &op_stack) override;

    void accept(const graph_operation::operation_type &op) override;

    void reject(const graph_operation::operation_type &op) override;

private:

    unsigned m_frequency;
};


#endif //ALGORITHM_ENGINEERING_EXHAUSTIVE_ITERATIVE_CONTAINER_HPP
