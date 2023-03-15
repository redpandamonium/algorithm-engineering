//
// Created by Carsten Schubert on 2018-11-04.
//

#ifndef ALGORITHM_ENGINEERING_DOMINATE_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_DOMINATE_REDUCTION_HPP


#include "data_reduction_node.hpp"

class dominate_reduction : public data_reduction_node {
public:

    explicit dominate_reduction(graph_unit graph, unsigned min_degree, unsigned max_degree);

    std::string name() const override;

    bool apply(graph_operation::stack& op_stack) override;

    void accept(const graph_operation::operation_type& op) override;

    void reject(const graph_operation::operation_type& op) override;

private:

    unsigned m_min_degree;
    unsigned m_max_degree;
};

#endif //ALGORITHM_ENGINEERING_DOMINATE_REDUCTION_HPP
