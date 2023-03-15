//
// Created by Leon Suchy on 31.01.19.
//

#include <container/once_container.hpp>

using std::literals::string_literals::operator""s;
const std::string once_container::NAME("once"s);

std::string once_container::name() const
{
    return NAME;
}

bool once_container::apply(graph_operation::stack& op_stack)
{
    if (m_graph.meta_data().get_recursion_depth() % m_frequency != 0)
        return false;

    bool modified = false;

    for (auto& child : m_children)
    {
        // break early if we are done
        if (m_graph.meta_data().vertex_cover_size() <= 0)
            return false;

        if (child->apply(op_stack))
            modified = true;
    }

    return modified;
}

void once_container::accept(const graph_operation::operation_type& op)
{
    throw std::logic_error("A reduction container cannot undo anything");
}

void once_container::reject(const graph_operation::operation_type& op)
{
    throw std::logic_error("A reduction container cannot undo anything");
}

once_container::once_container(
        graph_unit unit,
        std::vector<std::unique_ptr<data_reduction_node>>&& children,
        unsigned frequency
)
        : data_reduction_container(unit, std::move(children)), m_frequency(frequency)
{

}
