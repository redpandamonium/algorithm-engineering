//
// Created by Carsten Schubert on 2019-01-28.
//

#ifndef ALGORITHM_ENGINEERING_DEGREE_4_CROSSBOW_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_DEGREE_4_CROSSBOW_REDUCTION_HPP

#include "data_reduction_node.hpp"

class degree_4_crossbow_reduction : public data_reduction_node {
public:
    explicit degree_4_crossbow_reduction (graph_unit graph);

    std::string name() const override;

    bool apply(graph_operation::stack &op_stack) override;

    void accept(const graph_operation::operation_type &op) override;

    void reject(const graph_operation::operation_type &op) override;
private:
    static constexpr const vertex::color_type MARK = 0x2;


    void connect_to_them(graph_operation::stack &op_stack, vertex::id_type vertex,
                         const std::vector<vertex::id_type> &set);

    void delete_edges_except(graph_operation::stack &op_stack, vertex::id_type vertex,
                         const std::vector<vertex::id_type> &set, vertex::id_type except);

    bool in_vc (vertex v);

    unsigned count_edges_in_subgraph(const vertex::id_type v, const std::vector<vertex::id_type>& subgraph);

};

#endif //ALGORITHM_ENGINEERING_DEGREE_4_CROSSBOW_REDUCTION_HPP
