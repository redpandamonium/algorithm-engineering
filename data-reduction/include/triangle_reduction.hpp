//
// Created by Leon Suchy on 25.11.18.
//

#ifndef ALGORITHM_ENGINEERING_TRIANGLE_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_TRIANGLE_REDUCTION_HPP

#include <graph_unit.hpp>
#include "data_reduction_node.hpp"

class triangle_reduction : public data_reduction_node
{
public:

    explicit triangle_reduction(graph_unit graph);

    std::string name() const override;

    bool apply(graph_operation::stack& op_stack) override;

    void accept(const graph_operation::operation_type& op) override;

    void reject(const graph_operation::operation_type& op) override;

private:

    bool reduce(graph_operation::stack& op_stack, vertex::id_type v);
};

#endif //ALGORITHM_ENGINEERING_TRIANGLE_REDUCTION_HPP
