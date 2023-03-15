//
// Created by Leon Suchy on 23.01.19.
//

#ifndef ALGORITHM_ENGINEERING_EXHAUSTIVE_CONTAINER_HPP
#define ALGORITHM_ENGINEERING_EXHAUSTIVE_CONTAINER_HPP

#include <string>
#include "data_reduction_container.hpp"

class exhaustive_container : public data_reduction_container
{
public:

    static const std::string NAME;

    exhaustive_container(graph_unit unit, std::vector<std::unique_ptr<data_reduction_node>>&& children);

    std::string name() const override;

    bool apply(graph_operation::stack& op_stack) override;

    void accept(const graph_operation::operation_type& op) override;

    void reject(const graph_operation::operation_type& op) override;

};

#endif //ALGORITHM_ENGINEERING_EXHAUSTIVE_CONTAINER_HPP
