//
// Created by toad on 10/29/18.
//

#ifndef ALGORITHM_ENGINEERING_2_FOLD_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_2_FOLD_REDUCTION_HPP

#include "data_reduction_node.hpp"

class degree_2_folding : public data_reduction_node {
public:

    explicit degree_2_folding(graph_unit graph);

    std::string name() const override;

    bool apply(graph_operation::stack& op_stack) override;

    void accept(const graph_operation::operation_type& op) override;

    void reject(const graph_operation::operation_type& op) override;

private:

    bool reduce(graph_operation::stack& op_stack, vertex::id_type v);

    void remove_extra_edges(vertex::id_type v, vertex::id_type middle);
};


#endif //ALGORITHM_ENGINEERING_2_FOLD_REDUCTION_HPP
