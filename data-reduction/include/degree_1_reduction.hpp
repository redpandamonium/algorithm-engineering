//
// Created by toad on 10/29/18.
//

#ifndef ALGORITHM_ENGINEERING_DEGREE_ONE_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_DEGREE_ONE_REDUCTION_HPP

#include <fast_set.hpp>
#include "data_reduction_node.hpp"

class degree_1_reduction : public data_reduction_node {
public:

    explicit degree_1_reduction(graph_unit graph);

    std::string name() const override;

    bool apply(graph_operation::stack& op_stack) override;

    void accept(const graph_operation::operation_type& op) override;

    void reject(const graph_operation::operation_type& op) override;

private:

    bool reduce(graph_operation::stack& op_stack, vertex::id_type v);
    bool searching_reduce(graph_operation::stack& op_stack);

    fast_set m_set;
};


#endif //ALGORITHM_ENGINEERING_DEGREE_ONE_REDUCTION_HPP
