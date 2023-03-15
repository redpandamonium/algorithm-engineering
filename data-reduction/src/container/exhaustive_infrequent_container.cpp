//
// Created by Leon Suchy on 23.01.19.
//


#include <string>
#include <graph_operation.hpp>
#include <container/exhaustive_infrequent_container.hpp>


std::string exhaustive_infrequent_container::name() const
{
    return "exhaustive-infrequent";
}

bool exhaustive_infrequent_container::apply(graph_operation::stack& op_stack)
{
    if (m_graph.meta_data().get_recursion_depth() % m_frequency != 0)
        return false;

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

void exhaustive_infrequent_container::accept(const graph_operation::operation_type& op)
{
    throw std::logic_error("A reduction container cannot undo anything");
}

void exhaustive_infrequent_container::reject(const graph_operation::operation_type& op)
{
    throw std::logic_error("A reduction container cannot undo anything");
}

exhaustive_infrequent_container::exhaustive_infrequent_container(
    graph_unit unit,
    std::vector<std::unique_ptr<data_reduction_node>>&& children,
    unsigned frequency
)
    : data_reduction_container(unit, std::move(children)), m_frequency(frequency)
{

}

unsigned exhaustive_infrequent_container::get_frequency() const noexcept
{
    return m_frequency;
}
