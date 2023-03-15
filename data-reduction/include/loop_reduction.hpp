//
// Created by Leon Suchy on 2019-02-09.
//

#ifndef ALGORITHM_ENGINEERING_LOOP_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_LOOP_REDUCTION_HPP

#include "data_reduction_node.hpp"

class loop_reduction : public data_reduction_node
{
public:

    explicit loop_reduction(graph_unit unit);

    std::string name() const override;

    bool apply(graph_operation::stack& op_stack) override;

    void accept(const graph_operation::operation_type& op) override;

    void reject(const graph_operation::operation_type& op) override;

private:

};

#endif //ALGORITHM_ENGINEERING_LOOP_REDUCTION_HPP
