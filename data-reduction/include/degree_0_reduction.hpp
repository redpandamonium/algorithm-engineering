//
// Created by Leon Suchy on 11.11.18.
//

#ifndef ALGORITHM_ENGINEERING_DEGREE_0_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_DEGREE_0_REDUCTION_HPP

#include <graph_unit.hpp>
#include "data_reduction_node.hpp"

class degree_0_reduction : public data_reduction_node
{
public:

    explicit degree_0_reduction(graph_unit graph);

    std::string name() const override;

    bool apply(graph_operation::stack& op_stack) override;

    void accept(const graph_operation::operation_type& op) override;

    void reject(const graph_operation::operation_type& op) override;

private:

   bool reduce(graph_operation::stack& op_stack, vertex::id_type v);
};


#endif //ALGORITHM_ENGINEERING_DEGREE_0_REDUCTION_HPP
