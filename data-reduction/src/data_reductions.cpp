//
// Created by Leon Suchy on 12.11.18.
//

#include <data_reductions.hpp>

void data_reductions::apply()
{
    m_root->apply(m_op_stack);
}

void data_reductions::reject()
{
    while (!m_op_stack.empty())
    {
        graph_operation::stack::operation op = m_op_stack.pop();
        op.actor.reject(op.action);
    }
}

void data_reductions::accept()
{
    while (!m_op_stack.empty())
    {
        graph_operation::stack::operation op =m_op_stack.pop();
        op.actor.accept(op.action);
    }
}

data_reductions::~data_reductions()
{
    reject();
}

data_reductions::data_reductions(std::unique_ptr<data_reduction_node>&& root)
    : m_root(std::move(root))
{

}

data_reduction_node& data_reductions::root() const
{
    return *m_root;
}

operation_stack_imbalance::operation_stack_imbalance(const graph_operation::stack& op_stack)
    : std::logic_error(
            "A rejected path did not revert its data reductions correctly. " +
            std::to_string(op_stack.size()) + " operations left to be undone after cleanup.")
{ }
