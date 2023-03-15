//
// Created by Carsten Schubert on 2018-11-19.
//


#include "unconfined_reduction.hpp"
#include <util.hpp>
#include <algorithm>
#include <iostream>

std::string unconfined_reduction::name() const
{
    return "unconfined";
}

bool unconfined_reduction::apply(graph_operation::stack& op_stack)
{
    bool changed = false;
    std::vector<vertex::id_type> unconfined_neighbors;
    for (const auto& vertex : m_graph.graph())
    {

        if (m_graph.meta_data().vertex_cover_size() <= 0)
            return false;

        if (vertex.degree() > m_max_degree)
            continue;

        if (vertex.degree() < m_min_degree)
            continue;

        /*
         * Pseudocode:
         * 1. Let S = {v}.
         * 2. Find u ∈ N(S) such that |N(u)∩S|= 1 and |N(u)\N[S]| is minimized.
         * 3. If there is no such vertex, return no.
         * 4. If N(u)\N[S] = ∅, return yes.
         * 5. If N(u)\N[S] is a single vertex w, go back to line 2 by adding w to S.
         * 6. Return no.
         */

        auto added = 1;

        m_set.clear();
        unconfined_neighbors.clear();
        auto current = vertex.id();
        m_set.add(current);

        // get all neighbors of S (currently only the current vertex)
        for (auto u : vertex.neighbors())
        {
            m_set.add(u);
            unconfined_neighbors.push_back(u);
            m_unconfined_degrees[u] = 1; // degree is initially 1
        }

        bool finished = false;
        while (!finished)
        {
            finished = true;

            for (std::size_t i = 0; i < unconfined_neighbors.size(); ++i)
            {
                auto u = unconfined_neighbors[i];
                if (m_unconfined_degrees[u] != 1)
                    continue;

                // check for a single vertex OR if the neighbour difference is empty
                auto w = vertex::invalid_id;
                auto possible = neighbor_difference(u, w);

                if (!possible)
                    continue;

                // unconfined vertex
                if (w == vertex::invalid_id)
                {
                    // include into vc
                    m_graph.controller().include_into_vertex_cover(current);
                    op_stack.push(*this, graph_operation::include {current, nullptr });

                    auto n = vertex.neighbors();
                    // look if neighbours are degree 0,1 vertices now and reduced them
                    reduce_neighbors(n, op_stack);

                    changed = true;
                    finished = true;
                    break;
                }
                else
                {
                    finished = false;
                    m_set.add(w);
                    added++;
                    update_unconfined_set(w, unconfined_neighbors);
                }
            }
        }

        if (vertex.valid() && added >= 2)
        {
            m_set.clear();
            for (auto u_c : unconfined_neighbors)
                m_set.add(u_c);

            initialize_vertex_set(unconfined_neighbors);

            bool found = find_diamond(vertex.id(), unconfined_neighbors, op_stack);

            if (found)
            {
                auto n = vertex.neighbors();
                reduce_neighbors(n, op_stack);
                changed = true;
            }
        }
    }

    return changed;
}

inline bool unconfined_reduction::neighbor_difference(const vertex::id_type u, vertex::id_type& single)
{
    bool ok = true;
    for (auto w : m_graph.graph()[u].neighbors())
    {
        if (m_set.is_member(w))
            continue;

        if (single != vertex::invalid_id)
        {
            ok = false;
            break;
        }
        single = w;
    }
    return ok;
}

inline void unconfined_reduction::update_unconfined_set(
        vertex::id_type w,
        std::vector<vertex::id_type> &unconfined_neighbors)
{
    for (auto w_neighbor : m_graph.graph()[w].neighbors())
    {
        if (m_set.add(w_neighbor))
        {
            unconfined_neighbors.push_back(w_neighbor);
            m_unconfined_degrees[w_neighbor] = 1;
        }
        else
        {
            m_unconfined_degrees[w_neighbor]++;
        }
    }
}

void unconfined_reduction::reduce_neighbors(std::vector<vertex::id_type>& neighbors, graph_operation::stack& op_stack)
{
    std::queue<vertex::id_type> q;
    m_set.clear();

    for (auto n : neighbors)
    {
        if (m_graph.graph()[n].degree() <= 1)
        {
            q.push(n);
            m_set.add(n);
        }
    }

    while (!q.empty())
    {
        if (m_graph.meta_data().vertex_cover_size() <= 0)
            break;

        auto current = q.front();
        q.pop();

        if (!m_graph.graph()[current].valid())
            continue;

        auto current_neighbors = m_graph.graph()[current].neighbors();
        for (auto neighbor : current_neighbors)
        {
            for (auto neighbor_neighbor : m_graph.graph()[neighbor].neighbors())
            {
                if (m_graph.graph()[neighbor_neighbor].degree() <= 1 && m_set.add(neighbor_neighbor))
                {
                    q.push(neighbor_neighbor);
                }
            }
        }
        m_graph.controller().invalidate(current);
        op_stack.push(*this, graph_operation::invalidate { current, nullptr });
        if (!current_neighbors.empty())
        {
            m_graph.controller().include_into_vertex_cover(current_neighbors[0]);
            op_stack.push(*this, graph_operation::include {current_neighbors[0], nullptr });
        }
    }
}

void unconfined_reduction::initialize_vertex_set(std::vector<vertex::id_type> &unconfined_neighbors)
{
    for (std::size_t i = 0; i < unconfined_neighbors.size(); ++i)
    {
        m_temp[i] = vertex::invalid_id;
        m_temp[m_size + i] = vertex::invalid_id;

        auto u = unconfined_neighbors[i];

        if (m_unconfined_degrees[u] != 2)
            continue;

        auto v1 = vertex::invalid_id;
        auto v2 = vertex::invalid_id;

        for (auto u_neighbor : m_graph.graph()[u].neighbors())
        {
            if (m_set.is_member(u_neighbor))
                continue;

            if (v1 == vertex::invalid_id)
                v1 = u_neighbor;
            else if (v2 == vertex::invalid_id)
                v2 = u_neighbor;
            else
            {
                v1 = v2 = vertex::invalid_id;
                break;
            }
        }

        if (v1 < v2)
            std::swap(v1, v2);

        m_temp[i] = v1;
        m_temp[m_size + i] = v2;
    }
}

bool unconfined_reduction::find_diamond(
        vertex::id_type v,
        std::vector<vertex::id_type> &unconfined_neighbors,
        graph_operation::stack &op_stack)
{
    for (std::size_t i = 0; i < unconfined_neighbors.size(); ++i)
    {
        if (m_temp[i] == vertex::invalid_id || m_temp[m_size + i] == vertex::invalid_id)
            continue;

        auto u = unconfined_neighbors[i];
        m_set.clear();

        for (auto u_neighbor : m_graph.graph()[u].neighbors())
            m_set.add(u_neighbor);

        for (std::size_t j = i + 1; j < unconfined_neighbors.size(); ++j)
        {
            if (m_set.is_member(unconfined_neighbors[j]))
                continue;

            if (!(m_temp[i] == m_temp[j] && m_temp[m_size + i] == m_temp[m_size + j]))
                continue;

            auto n = m_graph.graph()[v].neighbors();

            // found diamond in the unconfined set of v
            m_graph.controller().include_into_vertex_cover(v);
            op_stack.push(*this, graph_operation::include {v, nullptr });

            return true;
        }
    }
    return false;
}

void unconfined_reduction::accept(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::include>(op))
    {
        graph_operation::include include_op = std::get<graph_operation::include>(op);
        m_graph.controller().revalidate(include_op.vertex);
    }
    else if (std::holds_alternative<graph_operation::invalidate>(op))
    {
        graph_operation::invalidate inv = std::get<graph_operation::invalidate>(op);
        m_graph.controller().revalidate(inv.vertex);
    }
    else
    {
        throw std::logic_error("Unconfined reduction cannot unfold operations it did not perform");
    }

}

void unconfined_reduction::reject(const graph_operation::operation_type& op)
{
    if (std::holds_alternative<graph_operation::include>(op))
    {
        graph_operation::include include_op = std::get<graph_operation::include>(op);
        m_graph.controller().exclude_from_vertex_cover(include_op.vertex);
    }
    else if (std::holds_alternative<graph_operation::invalidate>(op))
    {
        graph_operation::invalidate inv = std::get<graph_operation::invalidate>(op);
        m_graph.controller().revalidate(inv.vertex);
    }
    else
    {
        throw std::logic_error("Unconfined reduction cannot undo operations it did not perform");
    }
}

unconfined_reduction::unconfined_reduction(graph_unit graph, unsigned min_degree, unsigned max_degree) :
        data_reduction_node(graph),
        m_min_degree(min_degree),
        m_max_degree(max_degree),
        m_set(m_graph.graph().size_including_inactive()),
        m_unconfined_degrees(m_graph.graph().size_including_inactive(), 0),
        m_temp(m_graph.graph().size_including_inactive() * 2, 0)
{ }
