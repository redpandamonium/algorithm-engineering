//
// Created by synis on 1/24/19.
//

#ifndef ALGORITHM_ENGINEERING_LP_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_LP_REDUCTION_HPP


#include <variant>
#include "data_reductions.hpp"

class lp_reduction : public data_reduction_node {
public:

    explicit lp_reduction(graph_unit graph);

    std::string name() const override;

    bool apply(graph_operation::stack &op_stack) override;

    void accept(const graph_operation::operation_type& op) override;

    void reject(const graph_operation::operation_type &op) override;

private:

    static constexpr const vertex::color_type VISITED_LEFT = 0x4;
    static constexpr const vertex::color_type VISITED_RIGHT = 0x8;

    static constexpr const vertex::id_type nil = vertex::invalid_id;

    std::vector<vertex::id_type> m_in;
    std::vector<vertex::id_type> m_out;
};



#endif //ALGORITHM_ENGINEERING_LP_REDUCTION_HPP
