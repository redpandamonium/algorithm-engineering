//
// Created by toad on 10/29/18.
//

#include <algorithm>
#include <iostream>
#include "degree_2_folding.hpp"
#include <util.hpp>
#include <degree_2_folding.hpp>


// m_removed_vertices is for triangles
// m_folded_vertices for 2-Fold

std::string degree_2_folding::name() const {
    return "2-fold";
}

bool degree_2_folding::apply(graph_operation::stack& op_stack)
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
        for (graph::const_iterator_type it = m_graph.graph().cbegin(); it != m_graph.graph().cend(); ++it)
        {
            vertex::id_type v = it->id();
            // including more vertices into cover would not be possible
            if (m_graph.meta_data().vertex_cover_size() <= 0)
                return false;

            if (reduce(op_stack, v))
                changed = true;
        }
    }

    return changed;
}

void degree_2_folding::accept(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::folding>(op))
    {
        graph_operation::folding fold_op = std::get<graph_operation::folding>(op);
        m_graph.controller().increase_vertex_cover_size(1);

        if (util::is_in_vertex_cover(m_graph.graph(), fold_op.middle))
        {
            // take v's neighbors instead
            m_graph.controller().revalidate(fold_op.left);
            m_graph.controller().revalidate(fold_op.right);
            m_graph.controller().invalidate(fold_op.middle);
            m_graph.controller().exclude_from_vertex_cover(fold_op.middle);
            m_graph.controller().include_into_vertex_cover(fold_op.left);
            m_graph.controller().include_into_vertex_cover(fold_op.right);
        }
        else
        {
            // take v
            m_graph.controller().revalidate(fold_op.middle);
            m_graph.controller().revalidate(fold_op.left);
            m_graph.controller().revalidate(fold_op.right);
            m_graph.controller().include_into_vertex_cover(fold_op.middle);
        }
    }
    else if (std::holds_alternative<graph_operation::edge_insert>(op))
    {
        graph_operation::edge_insert insert_op = std::get<graph_operation::edge_insert>(op);
        m_graph.controller().remove_edge(insert_op.e.get_a(), insert_op.e.get_b());
    }
    else
        throw std::logic_error("Cannot undo operation which the instance didn't perform");
}

void degree_2_folding::reject(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::folding>(op))
    {
        graph_operation::folding fold_op = std::get<graph_operation::folding>(op);
        m_graph.controller().increase_vertex_cover_size(1);

        // revalidate neighbors
        m_graph.controller().revalidate(fold_op.left);
        m_graph.controller().revalidate(fold_op.right);

        // fix middle
        if (util::is_in_vertex_cover(m_graph.graph(), fold_op.middle))
            m_graph.controller().exclude_from_vertex_cover(fold_op.middle);
    }
    else if (std::holds_alternative<graph_operation::edge_insert>(op))
    {
        graph_operation::edge_insert insert_op = std::get<graph_operation::edge_insert>(op);
        m_graph.controller().remove_edge(insert_op.e.get_a(), insert_op.e.get_b());
    }
    else
    {
        throw std::logic_error("degree 2 reduction can only cause invalidate, include and group and thus cannot undo anything else");
    }
}

degree_2_folding::degree_2_folding(graph_unit graph)
    : data_reduction_node(graph)
{ }

bool degree_2_folding::reduce(graph_operation::stack& op_stack, vertex::id_type v)
{
    if (m_graph.meta_data().vertex_cover_size() <= 0)
        return false;

    if (m_graph.graph()[v].degree() == 2)
    {
        auto neighbors = m_graph.graph()[v].neighbors();

        // 2-Fold
        if (!m_graph.graph().has_edge(neighbors[0], neighbors[1]))
        {
            // distance 2 neighbors from v
            std::vector<vertex::id_type> left_neighbors = m_graph.graph()[neighbors[0]].neighbors();
            std::vector<vertex::id_type> right_neighbors = m_graph.graph()[neighbors[1]].neighbors();

            // invalidate neighbors of v
            m_graph.controller().invalidate(neighbors[0]);
            m_graph.controller().invalidate(neighbors[1]);

            // insert additional edges
            for (auto left : left_neighbors)
            {
                if (left == v)
                    continue;

                if (!m_graph.graph().has_edge(left, v))
                {
                    m_graph.controller().add_edge(left, v);
                    op_stack.push(*this, graph_operation::edge_insert { edge { left, v }, nullptr });
                }
            }
            for (auto right : right_neighbors)
            {
                if (right == v)
                    continue;

                if (!m_graph.graph().has_edge(right, v))
                {
                    m_graph.controller().add_edge(right, v);
                    op_stack.push(*this, graph_operation::edge_insert { edge { right, v }, nullptr });
                }
            }

            op_stack.push(*this, graph_operation::folding { v, neighbors[0], neighbors[1] });

            m_graph.controller().decrease_vertex_cover_size(1); // we can assume one was taken
            return true;
        }
    }
    return false;
}

void degree_2_folding::remove_extra_edges(vertex::id_type v, vertex::id_type middle)
{
    auto v_neighbors  = m_graph.graph()[v].neighbors();
    for (auto neighbor : v_neighbors)
    {
        if (neighbor == middle)
            continue;

        m_graph.controller().remove_edge(neighbor, middle);
    }
}
