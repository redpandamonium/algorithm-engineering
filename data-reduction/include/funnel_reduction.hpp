//
// Created by synis on 2/23/19.
//

#ifndef ALGORITHM_ENGINEERING_FUNNEL_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_FUNNEL_REDUCTION_HPP


#include "data_reduction_node.hpp"
#include <fast_set.hpp>

class funnel_reduction : public data_reduction_node{
public:

    explicit funnel_reduction(graph_unit graph);

    std::string name() const override;

    bool apply(graph_operation::stack &op_stack) override;

    void accept(const graph_operation::operation_type &op) override;

    void reject(const graph_operation::operation_type &op) override;

private:

    unsigned get_degree(vertex::id_type v);
    bool valid_funnel(vertex::id_type p1, vertex::id_type p2, const std::vector<vertex::id_type>& neighbors);
    void reduce_funnel(vertex::id_type v, vertex::id_type u, graph_operation::stack &op_stack);

    fast_set m_set;
    std::vector<bool> m_status;
};


#endif //ALGORITHM_ENGINEERING_FUNNEL_REDUCTION_HPP
