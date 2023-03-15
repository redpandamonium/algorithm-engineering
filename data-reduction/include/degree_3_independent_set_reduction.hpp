//
// Created by Carsten Schubert on 2019-01-24.
//

#ifndef ALGORITHM_ENGINEERING_DEGREE_3_INDEPENDENT_SET_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_DEGREE_3_INDEPENDENT_SET_REDUCTION_HPP


#include "data_reduction_node.hpp"

class degree_3_independent_set_reduction : public data_reduction_node {
public:
    explicit degree_3_independent_set_reduction (graph_unit graph);

    std::string name() const override;

    bool apply(graph_operation::stack &op_stack) override;

    void accept(const graph_operation::operation_type &op) override;

    void reject(const graph_operation::operation_type &op) override;
private:
    static constexpr const vertex::color_type MARK = 0x2;


    void connect_to_them(graph_operation::stack &op_stack, vertex::id_type vertex,
                         const std::vector<vertex::id_type> &set);

};


#endif //ALGORITHM_ENGINEERING_DEGREE_3_INDEPENDENT_SET_REDUCTION_HPP
