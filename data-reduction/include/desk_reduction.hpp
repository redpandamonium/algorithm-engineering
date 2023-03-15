//
// Created by synis on 2/28/19.
//

#ifndef ALGORITHM_ENGINEERING_DESK_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_DESK_REDUCTION_HPP


#include "data_reduction_node.hpp"
#include <fast_set.hpp>

class desk_reduction : public data_reduction_node{

public:

    explicit desk_reduction(graph_unit unit);

    std::string name() const override;

    bool apply(graph_operation::stack &op_stack) override;

    void accept(const graph_operation::operation_type &op) override;

    void reject(const graph_operation::operation_type &op) override;

private:

    fast_set m_set;
    std::vector<vertex::id_type> m_neighbor_vertex;

    bool find_desk(const vertex::id_type current, const std::vector<vertex::id_type>& neighbors, graph_operation::stack& op_stack);
    void reduce_desk(const std::vector<vertex::id_type> A, const std::vector<vertex::id_type> B, graph_operation::stack& op_stack);
};


#endif //ALGORITHM_ENGINEERING_DESK_REDUCTION_HPP
