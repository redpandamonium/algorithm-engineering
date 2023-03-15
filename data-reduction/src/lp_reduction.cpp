//
// Created by synis on 1/24/19.
//

#include <hopcroft_karp_matching.hpp>
#include <util.hpp>
#include <stack>
#include <iostream>
#include "lp_reduction.hpp"

lp_reduction::lp_reduction(graph_unit graph) : data_reduction_node(graph) {}

std::string lp_reduction::name() const {
    return "lp";
}

bool lp_reduction::apply(graph_operation::stack &op_stack)
{
    auto v_before = m_graph.graph().size();
    auto vc_before = m_graph.meta_data().vertex_cover_size();

    m_graph.controller().lp_update_matching();
    m_graph.controller().lp_matching();

    m_in = m_graph.meta_data().left_right();
    m_out = m_graph.meta_data().right_left();

    // this was calculated by hopcroft karp. use the results
    for (auto& v : m_graph.graph())
    {
        if (util::is_marked(v, VISITED_LEFT) && !util::is_marked(v, VISITED_RIGHT))
        {
            auto id = v.id();
            m_graph.controller().unmark(id, VISITED_LEFT);
            m_graph.controller().invalidate(id);
            graph_operation::stack::operation op {*this, graph_operation::operation_type {graph_operation::invalidate {id, nullptr}}};
            op_stack.push(std::move(op));

            auto neighbors = v.neighbors();

            for (auto neighbor : neighbors)
            {
                m_graph.controller().unmark(neighbor, VISITED_LEFT);
                m_graph.controller().unmark(neighbor, VISITED_RIGHT);
                m_graph.controller().include_into_vertex_cover(neighbor);
                graph_operation::stack::operation op_ {*this, graph_operation::operation_type {graph_operation::include {neighbor, nullptr}}};
                op_stack.push(std::move(op_));
            }
        }
        else
        {
            m_graph.controller().unmark(v.id(), VISITED_LEFT);
            m_graph.controller().unmark(v.id(), VISITED_RIGHT);
        }
    }
    // find strongly connected components

    auto g_size = m_graph.graph().size_including_inactive();
    std::vector<bool> visited;
    visited.resize(2 * g_size, false);

    std::queue<vertex::id_type> queue;
    std::vector<vertex::id_type> starting_points;
    for (auto& start : m_graph.graph())
    {
        auto start_id = start.id();
        if (visited[start_id])
            continue;
        visited[start_id] = true;
        queue.push(start_id);

        while (!queue.empty())
        {
            auto current = queue.front();
            queue.pop();

            auto next = nil;

            // left vertex
            if (current < g_size)
            {
                for (auto n : m_graph.graph()[current].neighbors())
                {
                    auto right = g_size + n;
                    if (visited[right])
                        continue;

                    visited[right] = true;
                    next = right;
                    break;
                }
            } else if (!visited[m_in[current - g_size]])
            {
                visited[m_in[current - g_size]] = true;
                next = m_in[current - g_size];
            }

            if (next != nil)
            {
                queue.push(next);
            } else
            {
                starting_points.push_back(current);
            }
        }
    }

    std::fill(visited.begin(), visited.end(), false);

    // remove vertices from strongly connected components
    for (auto it = starting_points.rbegin(); it != starting_points.rend(); it++)
    {
        auto v = *it;

        if (visited[v])
            continue;

        visited[v] = true;

        std::queue<vertex::id_type> q;
        std::vector<vertex::id_type> c;

        q.push(v);
        c.push_back(v);

        bool found = true;
        while (!q.empty())
        {
            auto current = q.front();
            q.pop();

            if (visited[(current >= g_size ? (current - g_size) : (current + g_size))])
                found = false;

            if (current >= g_size)
            {
                for (auto n : m_graph.graph()[current - g_size].neighbors())
                {
                    if (visited[n])
                        continue;

                    visited[n] = true;
                    q.push(n);
                    c.push_back(n);
                }
            } else if (!visited[m_out[current] + g_size])
            {
                visited[m_out[current] + g_size] = true;
                q.push(m_out[current] + g_size);
                c.push_back(m_out[current] + g_size);
            }
        }

        if (found)
        {
            for (auto ver : c)
            {
                // right vertex
                if (ver >= g_size)
                {
                    auto original = ver - g_size;
                    m_graph.controller().invalidate(original);
                    graph_operation::stack::operation inv_op {*this, graph_operation::operation_type {graph_operation::invalidate {original, nullptr}}};
                    op_stack.push(std::move(inv_op));

                    for (auto n : m_graph.graph()[original].neighbors())
                    {
                        m_graph.controller().include_into_vertex_cover(n);
                        graph_operation::stack::operation inc_op {*this, graph_operation::operation_type {graph_operation::include {n, nullptr}}};
                        op_stack.push(std::move(inc_op));
                    }
                }
            }
        }
    }

    if (v_before != m_graph.graph().size())
    {
        std::cout << "# (" << name() << ") Total Removed: " << v_before - m_graph.graph().size();
        std::cout << ", Taken: " << vc_before - m_graph.meta_data().vertex_cover_size() << std::endl;
    }

    return false;
}

void lp_reduction::accept(const graph_operation::operation_type& op)
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

void lp_reduction::reject(const graph_operation::operation_type &op)
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
        throw std::logic_error("LP-Reduction should not have to undo any operation other than include and invalidate");
    }
}