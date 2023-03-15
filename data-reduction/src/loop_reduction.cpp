//
// Created by Leon Suchy on 2019-02-09.
//

#include <string>
#include <graph_operation.hpp>
#include <loop_reduction.hpp>


std::string loop_reduction::name() const
{
    return "loop";
}

bool loop_reduction::apply(graph_operation::stack& op_stack)
{
    for (auto it = m_graph.graph().cbegin(); it != m_graph.graph().cend(); ++it)
    {
        if ((*it).get_incident_edge_to((*it)))
        {
            m_graph.controller().include_into_vertex_cover(it->id());
            op_stack.push(*this, graph_operation::include { it->id(), nullptr });
        }
    }
    return false;
}

void loop_reduction::accept(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::include>(op))
    {
        graph_operation::include include_op = std::get<graph_operation::include>(op);
        m_graph.controller().revalidate(include_op.vertex);
    }
    else
    {
        throw std::logic_error("Loop reduction can only undo include operations");
    }
}

void loop_reduction::reject(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::include>(op))
    {
        graph_operation::include include_op = std::get<graph_operation::include>(op);
        m_graph.controller().exclude_from_vertex_cover(include_op.vertex);
    }
    else
    {
        throw std::logic_error("Loop reduction can only undo include operations");
    }
}

loop_reduction::loop_reduction(graph_unit unit) : data_reduction_node(unit)
{ }
