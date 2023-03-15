//
// Created by Carsten Schubert on 2018-11-04.
//

#include <algorithm>
#include "dominate_reduction.hpp"

std::string dominate_reduction::name() const {
    return "dominate";
}

void dominate_reduction::reject(const graph_operation::operation_type& op) {

    if (std::holds_alternative<graph_operation::include>(op))
    {
        graph_operation::include include_op = std::get<graph_operation::include>(op);
        m_graph.controller().exclude_from_vertex_cover(include_op.vertex);
    }
    else
    {
        throw std::logic_error("Dominate reduction cannot undo operations it did not perform");
    }
}

bool dominate_reduction::apply(graph_operation::stack& op_stack)
{
    bool changed = false;
    // candidate for max heap
    for (auto it = m_graph.graph().cbegin(); it != m_graph.graph().cend(); ++it)
    {
        const auto& vertex = *it;
        if (m_graph.meta_data().vertex_cover_size() <= 0)
            return false;

        if (vertex.degree() < m_min_degree)
            continue;

        if (vertex.degree() > m_max_degree)
            continue;

        auto neighbors = vertex.neighbors();
        std::sort(std::begin(neighbors), std::end(neighbors));

        for (auto neighbor : neighbors)
        {
            if (m_graph.graph()[neighbor].degree() <= vertex.degree())
            {
                auto other_neighbors = m_graph.graph()[neighbor].neighbors();
                // removing 'vertex' itself from the neighbours of 'neighbor'
                other_neighbors.erase(std::remove(other_neighbors.begin(), other_neighbors.end(), vertex.id()), other_neighbors.end());
                std::sort(std::begin(other_neighbors), std::end(other_neighbors));

                if (std::includes(neighbors.begin(), neighbors.end(), other_neighbors.begin(), other_neighbors.end()))
                {
                    m_graph.controller().include_into_vertex_cover(vertex.id());
                    op_stack.push(*this, graph_operation::include { vertex.id(), nullptr });
                    changed = true;
                    break;
                }
            }
        }

    }
    return changed;
}

void dominate_reduction::accept(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::include>(op))
    {
        graph_operation::include inc = std::get<graph_operation::include>(op);
        m_graph.controller().revalidate(inc.vertex);
    }
    else
        throw std::logic_error("Cannot undo operation which the instance didn't perform");
}

dominate_reduction::dominate_reduction(graph_unit graph, unsigned min_degree, unsigned max_degree)
    : data_reduction_node(graph), m_min_degree(min_degree), m_max_degree(max_degree)
{ }
