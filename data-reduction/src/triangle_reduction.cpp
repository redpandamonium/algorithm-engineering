//
// Created by Leon Suchy on 25.11.18.
//

#include <triangle_reduction.hpp>

#include "triangle_reduction.hpp"

triangle_reduction::triangle_reduction(graph_unit graph) : data_reduction_node(graph)
{ }

std::string triangle_reduction::name() const
{
    return "triangle";
}

bool triangle_reduction::apply(graph_operation::stack& op_stack)
{
    bool changed = false;
    if (m_graph.meta_data().get_options().use_min_heap)
    {
        while (!m_graph.meta_data().min_heap().empty())
        {
            vertex::id_type vertex = m_graph.meta_data().min_heap().top();
            if (m_graph.graph()[vertex].degree() != 2)
                break;

            if (reduce(op_stack, vertex))
                changed = true;
            else
                break;
        }
    }
    else
    {
        for (const vertex& v : m_graph.graph())
        {
            if (m_graph.meta_data().vertex_cover_size() <= 0)
                return false;

            if (reduce(op_stack, v.id()))
                changed = true;
        }
    }

    return changed;
}

void triangle_reduction::accept(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::invalidate>(op))
    {
        graph_operation::invalidate inv = std::get<graph_operation::invalidate>(op);
        m_graph.controller().revalidate(inv.vertex);
    }
    else if (std::holds_alternative<graph_operation::include>(op))
    {
        graph_operation::include inc = std::get<graph_operation::include>(op);
        m_graph.controller().revalidate(inc.vertex);
    }
    else
        throw std::logic_error("Cannot undo operation which the instance didn't perform");
}

void triangle_reduction::reject(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::include>(op))
    {
        graph_operation::include include_op = std::get<graph_operation::include>(op);
        m_graph.controller().exclude_from_vertex_cover(include_op.vertex);
    }
    else if (std::holds_alternative<graph_operation::invalidate>(op))
    {
        graph_operation::invalidate invalidate_op = std::get<graph_operation::invalidate>(op);
        m_graph.controller().revalidate(invalidate_op.vertex);
    }
    else
    {
        throw std::logic_error("degree 2 reduction can only cause invalidate, include and group and thus cannot undo anything else");
    }
}

bool triangle_reduction::reduce(graph_operation::stack& op_stack, vertex::id_type v)
{
    if (m_graph.graph()[v].degree() == 2)
    {
        auto neighbors = m_graph.graph()[v].neighbors();

        // Triangle
        if (m_graph.graph().has_edge(neighbors[0], neighbors[1]))
        {
            // take the neighbors, discard the vertex
            m_graph.controller().include_into_vertex_cover(neighbors);
            m_graph.controller().invalidate(v);

            op_stack.push(*this, graph_operation::include{ neighbors[0], nullptr });
            op_stack.push(*this, graph_operation::include{ neighbors[1], nullptr });
            op_stack.push(*this, graph_operation::invalidate{ v, nullptr });

            return true;
        }
    }
    return false;
}
