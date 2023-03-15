//
// Created by synis on 2/21/19.
//

#include "container/exhaustive_iterative_container.hpp"

exhaustive_iterative_container::exhaustive_iterative_container(
    graph_unit unit,
    std::vector<std::unique_ptr<data_reduction_node>> &&children,
    unsigned frequency
) : data_reduction_container(unit, std::move(children)), m_frequency(frequency) {}

std::string exhaustive_iterative_container::name() const
{
    return "exhaustive-iterative";
}

bool exhaustive_iterative_container::apply(graph_operation::stack &op_stack)
{
    if (m_graph.meta_data().get_recursion_depth() % m_frequency != 0)
        return false;

    bool modified = false;

    std::size_t i = 0;
    while (i < m_children.size())
    {
        // break early if we are done
        if (m_graph.meta_data().vertex_cover_size() <= 0)
            return false;

        if (m_children[i]->apply(op_stack))
        {
            modified = true;
            i = 0;
            continue;
        }

        i++;
    }

    return modified;
}

void exhaustive_iterative_container::accept(const graph_operation::operation_type &op)
{
    throw std::logic_error("A reduction container cannot undo anything");
}

void exhaustive_iterative_container::reject(const graph_operation::operation_type &op)
{
    throw std::logic_error("A reduction container cannot undo anything");
}
