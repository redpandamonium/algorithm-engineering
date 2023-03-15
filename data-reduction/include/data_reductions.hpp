//
// Created by Leon Suchy on 12.11.18.
//

#ifndef ALGORITHM_ENGINEERING_DATA_REDUCTIONS_HPP
#define ALGORITHM_ENGINEERING_DATA_REDUCTIONS_HPP

#include <vector>
#include <memory>

#include "data_reduction_node.hpp"
#include "graph_operation.hpp"

class data_reductions
{
public:

    data_reductions(data_reductions&& mv) = default;
    explicit data_reductions(std::unique_ptr<data_reduction_node>&& root);

    ~data_reductions();

    void apply();
    void reject();
    void accept();

    data_reduction_node& root() const;

private:

    friend class data_reduction_factory;

    std::unique_ptr<data_reduction_node> m_root;
    graph_operation::stack m_op_stack;
};

struct operation_stack_imbalance : public std::logic_error
{
    explicit operation_stack_imbalance(const graph_operation::stack& op_stack);
};


#endif //ALGORITHM_ENGINEERING_DATA_REDUCTIONS_HPP
