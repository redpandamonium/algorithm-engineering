//
// Created by Leon Suchy on 11.11.18.
//

#include <degree_0_reduction.hpp>

#include "degree_0_reduction.hpp"

std::string degree_0_reduction::name() const
{
    return "degree-0";
}

bool degree_0_reduction::apply(graph_operation::stack& op_stack)
{
    if (m_graph.meta_data().get_options().use_min_heap)
    {
        while (!m_graph.meta_data().min_heap().empty())
        {
            vertex::id_type v = m_graph.meta_data().min_heap().top();
            if (!reduce(op_stack, v))
                break;
        }
    }
    else
    {
        for (const auto& vertex : m_graph.graph())
        {
            reduce(op_stack, vertex.id());
        }
    }

    return false;
}

void degree_0_reduction::accept(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::invalidate>(op))
    {
        graph_operation::invalidate inv = std::get<graph_operation::invalidate>(op);
        m_graph.controller().revalidate(inv.vertex);
    }
    else
        throw std::logic_error("Degree 0 reduction should only have to undo exclude operations");
}

void degree_0_reduction::reject(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::invalidate>(op))
    {
        graph_operation::invalidate ex = std::get<graph_operation::invalidate>(op);
        m_graph.controller().revalidate(ex.vertex);
    }
    else
    {
        throw std::logic_error("Degree 0 reduction should only have to undo invalidate operations");
    }
}

degree_0_reduction::degree_0_reduction(graph_unit graph)
        : data_reduction_node(graph)
{ }

bool degree_0_reduction::reduce(graph_operation::stack& op_stack, vertex::id_type v)
{
    if (m_graph.graph()[v].degree() == 0)
    {
        m_graph.controller().invalidate(v);
        graph_operation::stack::operation op { *this, graph_operation::operation_type { graph_operation::invalidate { v, nullptr } } };
        op_stack.push(std::move(op));
        return true;
    }
    return false;
}
