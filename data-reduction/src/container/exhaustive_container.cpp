//
// Created by Leon Suchy on 23.01.19.
//

#include <string>
#include <graph_operation.hpp>
#include <container/exhaustive_container.hpp>

using std::literals::string_literals::operator""s;
const std::string exhaustive_container::NAME("exhaustive"s);

std::string exhaustive_container::name() const
{
    return NAME;
}

bool exhaustive_container::apply(graph_operation::stack& op_stack)
{
    bool done;
    bool modified = false;

    do
    {
        done = true;
        for (std::size_t i = 0; i < m_children.size(); ++i)
        {
            // break early if we are done
            if (m_graph.meta_data().vertex_cover_size() <= 0)
                return false;

            if (m_children[i]->apply(op_stack))
            {
                modified = true;
                done = false;
            }
        }
    } while (!done);

    return modified;
}

void exhaustive_container::accept(const graph_operation::operation_type& op)
{
    throw std::logic_error("A reduction container cannot undo anything");
}

void exhaustive_container::reject(const graph_operation::operation_type& op)
{
    throw std::logic_error("A reduction container cannot undo anything");
}

exhaustive_container::exhaustive_container(
    graph_unit unit,
    std::vector<std::unique_ptr<data_reduction_node>>&& children
)
    : data_reduction_container(unit, std::move(children))
{

}
