//
// Created by toad on 10/29/18.
//

#include "degree_greater_k_reduction.hpp"
#include <util.hpp>
#include <degree_greater_k_reduction.hpp>


degree_greater_k_reduction::degree_greater_k_reduction(graph_unit graph)
    : data_reduction_node(graph)
{ };

std::string degree_greater_k_reduction::name() const {
    return "degree > k";
}

void degree_greater_k_reduction::reject(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::include>(op))
    {
        graph_operation::include include_op = std::get<graph_operation::include>(op);
        m_graph.controller().exclude_from_vertex_cover(include_op.vertex);
    }
    else
    {
        throw std::logic_error("Degree greater than k reduction cannot undo operations it did not perform");
    }
}

bool degree_greater_k_reduction::apply(graph_operation::stack& op_stack)
{
    bool changed = false;

    if (m_graph.meta_data().get_options().use_max_heap)
    {
        while (!m_graph.meta_data().max_heap().empty())
        {
            vertex::id_type vertex = m_graph.meta_data().max_heap().top();
            if (static_cast<int>(m_graph.graph()[vertex].degree()) <= m_graph.meta_data().vertex_cover_size())
                break;

            if (reduce(op_stack, vertex))
                changed = true;
            else
                break;
        }
    }
    else
    {
        for (const auto& vertex : m_graph.graph())
        {
            if (m_graph.meta_data().vertex_cover_size() <= 0)
                return false;

            // including more vertices into cover would not be possible
            if (reduce(op_stack, vertex.id()))
                changed = true;
        }
    }

    return changed;
}

void degree_greater_k_reduction::accept(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::include>(op))
    {
        graph_operation::include inc = std::get<graph_operation::include>(op);
        m_graph.controller().revalidate(inc.vertex);
    }
    else
        throw std::logic_error("Cannot undo operation which the instance didn't perform");
}

bool degree_greater_k_reduction::reduce(graph_operation::stack& op_stack, vertex::id_type v)
{
    if (m_graph.meta_data().vertex_cover_size() <= 0)
        return false;

    if (static_cast<int>(m_graph.graph()[v].degree()) > (m_graph.meta_data().vertex_cover_size()))
    {
        m_graph.controller().include_into_vertex_cover(v);
        op_stack.push(*this, graph_operation::include { v, nullptr });
        return true;
    }
    return false;
}
