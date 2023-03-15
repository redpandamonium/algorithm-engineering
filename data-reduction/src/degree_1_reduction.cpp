//
// Created by toad on 10/29/18.
//

#include <iostream>
#include "degree_1_reduction.hpp"

std::string degree_1_reduction::name() const {
    return "degree-1";
}

void degree_1_reduction::reject(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::include>(op))
    {
        graph_operation::include include = std::get<graph_operation::include>(op);
        m_graph.controller().exclude_from_vertex_cover(include.vertex);
    }
    else if (std::holds_alternative<graph_operation::invalidate>(op))
    {
        graph_operation::invalidate inv = std::get<graph_operation::invalidate>(op);
        m_graph.controller().revalidate(inv.vertex);
    }
    else
    {
        throw std::logic_error("Degree 1 reductions should not have to undo any operation other than include and invalidate");
    }
}

bool degree_1_reduction::apply(graph_operation::stack& op_stack)
{
    bool modified = false;
    if (m_graph.meta_data().get_options().use_min_heap)
    {
        while (!m_graph.meta_data().min_heap().empty())
        {
            vertex::id_type vertex = m_graph.meta_data().min_heap().top();
            if (m_graph.graph()[vertex].degree() != 1)
                break;

            if (reduce(op_stack, vertex))
                modified = true;
            else
                break;
        }
    }
    else
    {
        return searching_reduce(op_stack);
    }

    return modified;
}

void degree_1_reduction::accept(const graph_operation::operation_type& op)
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

degree_1_reduction::degree_1_reduction(graph_unit graph)
    : data_reduction_node(graph), m_set(m_graph.graph().size_including_inactive())
{ }

bool degree_1_reduction::reduce(graph_operation::stack& op_stack, vertex::id_type v)
{
    if (m_graph.meta_data().vertex_cover_size() <= 0)
        return false;

    if (m_graph.graph()[v].degree() == 1)
    {
        auto neighbor = m_graph.graph()[v].neighbors()[0];
        m_graph.controller().include_into_vertex_cover(neighbor);
        op_stack.push(*this, graph_operation::include { neighbor, nullptr });

        m_graph.controller().invalidate(v);
        op_stack.push(*this, graph_operation::invalidate { v, nullptr });

        return true;
    }
    return false;
}

bool degree_1_reduction::searching_reduce(graph_operation::stack &op_stack)
{

    m_set.clear();

    bool modified = false;
    std::queue<vertex::id_type> q;

    for (auto& vertex : m_graph.graph())
    {
        if (vertex.degree() <= 1) {
            q.push(vertex.id());
            m_set.add(vertex.id());
        }
    }

    while (!q.empty())
    {
        if (m_graph.meta_data().vertex_cover_size() <= 0)
            break;

        auto v = q.front();
        q.pop();

        if (!m_graph.graph()[v].valid())
            continue;

        auto v_neighbors = m_graph.graph()[v].neighbors();
        for (auto neighbor : v_neighbors)
        {
            for (auto neighbor_neighbor : m_graph.graph()[neighbor].neighbors())
            {
                if (m_graph.graph()[neighbor_neighbor].degree() <= 1 && m_set.add(neighbor_neighbor))
                {
                    q.push(neighbor_neighbor);
                }
            }
        }

        modified = true;
        m_graph.controller().invalidate(v);
        op_stack.push(*this, graph_operation::invalidate { v, nullptr });
        if (!v_neighbors.empty())
        {
            m_graph.controller().include_into_vertex_cover(v_neighbors[0]);
            op_stack.push(*this, graph_operation::include {v_neighbors[0], nullptr });
        }
    }
    return modified;
}
