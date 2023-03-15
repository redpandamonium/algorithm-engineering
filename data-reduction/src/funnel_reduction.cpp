//
// Created by synis on 2/23/19.
//

#include <util.hpp>
#include <iostream>
#include "funnel_reduction.hpp"

funnel_reduction::funnel_reduction(graph_unit graph) :
    data_reduction_node(graph),
    m_set(m_graph.graph().size_including_inactive()),
    m_status(m_graph.graph().size_including_inactive(), false)
{

}

std::string funnel_reduction::name() const {
    return "funnel";
}

bool funnel_reduction::apply(graph_operation::stack &op_stack) {
    bool changed = false;


    for (auto& v : m_graph.graph())
    {
        if (m_graph.meta_data().vertex_cover_size() < 0)
            return false;

        auto current = v.id();
        m_set.clear();

        const auto neighbors = m_graph.graph()[current].neighbors();
        const auto size = neighbors.size();

        // isolated or deg 1
        if (size <= 1)
        {
            m_graph.controller().invalidate(current);
            op_stack.push(*this, graph_operation::invalidate {current, nullptr });
            if (v.degree() == 1)
            {
                m_graph.controller().include_into_vertex_cover(neighbors[0]);
                op_stack.push(*this, graph_operation::include {neighbors[0], nullptr });
            }
            continue;
        }

        // iterate over neighbours to set them used
        for (auto u : neighbors)
            m_set.add(u);

        vertex::id_type potential = vertex::invalid_id;

        // find a vertex in the neighbourhood that destroys clique property
        for (auto u : neighbors)
        {
            // get degree to each other neighbour of the current vertex
            unsigned degree = get_degree(u);

            // clique
            if (degree + 1 < size)
            {
                potential = u;
                break;
            }
        }

        // clique trap
        if (potential == vertex::invalid_id)
        {
            m_graph.controller().invalidate(current);
            op_stack.push(*this, graph_operation::invalidate {current, nullptr });

            for (auto n : neighbors)
            {
                m_graph.controller().include_into_vertex_cover(n);
                op_stack.push(*this, graph_operation::include {n, nullptr });
            }
            continue;
        }

        for (auto u : neighbors)
            m_status[u] = false;

        for (auto u : m_graph.graph()[potential].neighbors())
            m_status[u] = true;

        vertex::id_type potential2 = vertex::invalid_id;

        for (auto u : neighbors)
        {
            if (u == potential)
                continue;
            if (m_status[u])
                continue;

            // vertex is not connected to other potential vertex
            potential2 = u;
            break;
        }

        if (potential2 == vertex::invalid_id)
            throw std::logic_error("(funnel) current is isolated clique, but didn't get reduced");

        m_set.remove(potential);
        m_set.remove(potential2);

        // get degree of both potential vertices
        unsigned p_degree = get_degree(potential);
        unsigned p2_degree = get_degree(potential2);

        // one vertex should be connected to every other neighbour of current
        if (p_degree < size - 2 && p2_degree < size - 2)
            continue;

        // check if there exists a funnel with v and one of the potential vertices
        if (!valid_funnel(potential, potential2, neighbors))
            continue;

        vertex::id_type alternative;

        // this is the case when potential is almost a clique with N[v] but isnt connected to potential2
        // just take potential 2 as alternative vertex
        if (p_degree == size - 2)
            alternative = potential2;
        else
            alternative = potential;

        reduce_funnel(current, alternative, op_stack);
        changed = true;
    }

    return changed;
}

inline bool funnel_reduction::valid_funnel(
        vertex::id_type p1,
        vertex::id_type p2,
        const std::vector<vertex::id_type> &neighbors)
{
    bool valid = true;
    for (auto u : neighbors)
    {
        if (u == p1 || u == p2)
            continue;

        unsigned degree = get_degree(u);

        // vertex in the neighbors was connected to both vertices that would make funnel impossible
        if (degree < neighbors.size() - 3)
        {
            valid = false;
            break;
        }
    }
    return valid;
}

unsigned funnel_reduction::get_degree(vertex::id_type v)
{
    unsigned degree = 0;
    for (auto n : m_graph.graph()[v].neighbors())
        if (m_set.is_member(n))
            degree++;

    return degree;
}

void funnel_reduction::reduce_funnel(vertex::id_type v, vertex::id_type u, graph_operation::stack &op_stack)
{
    m_set.clear();
    std::vector<vertex::id_type> v_u_diff;
    std::vector<vertex::id_type> u_v_diff;

    const auto v_neighbors = m_graph.graph()[v].neighbors();
    const auto u_neighbors = m_graph.graph()[u].neighbors();

    // set intersection of neighbours N(v) AND N(u)
    for (auto w : v_neighbors)
        m_set.add(w);
    for (auto w : u_neighbors)
    {
        if (m_set.is_member(w))
        {
            m_graph.controller().include_into_vertex_cover(w);
            op_stack.push(*this, graph_operation::include { w, nullptr });
        }
    }

    // set difference N(u) \ N[v]
    m_set.clear();
    m_set.add(v);
    for (auto w : u_neighbors)
        if (m_set.add(w) && m_graph.graph()[w].valid())
            u_v_diff.push_back(w);

    // set difference N(v) \ N[u]
    m_set.clear();
    m_set.add(u);
    for (auto w : v_neighbors)
        if (m_set.add(w) && m_graph.graph()[w].valid())
            v_u_diff.push_back(w);

    m_graph.controller().invalidate(v);
    m_graph.controller().invalidate(u);
    op_stack.push(*this, graph_operation::alternative{{v}, {u}, v_u_diff, u_v_diff});
    m_graph.controller().decrease_vertex_cover_size(1);

    m_set.clear();
    for (auto v_n : v_u_diff)
    {
        for (auto u_n : u_v_diff)
        {
            if (!m_graph.graph().has_edge(v_n, u_n))
            {
                // u_n and v_n dont have an edge
                op_stack.push(*this, graph_operation::edge_insert{ edge{v_n, u_n}, nullptr });
                m_graph.controller().add_edge(v_n, u_n);
            }
        }
    }
}

void funnel_reduction::accept(const graph_operation::operation_type &op) {
    if (std::holds_alternative<graph_operation::invalidate>(op))
    {
        const auto& inv = std::get<graph_operation::invalidate>(op);
        auto v = inv.vertex;
        m_graph.controller().revalidate(v);
    }
    else if (std::holds_alternative<graph_operation::include>(op))
    {
        const auto& inc = std::get<graph_operation::include>(op);
        auto v = inc.vertex;
        m_graph.controller().revalidate(v);
    }
    else if (std::holds_alternative<graph_operation::alternative>(op))
    {
        const auto& alt = std::get<graph_operation::alternative>(op);

        auto a = alt.A;
        auto b = alt.B;
        const auto& a_b_diff = alt.A_B_difference;
        const auto& b_a_diff = alt.B_A_difference;

        m_graph.controller().revalidate(a[0]);
        m_graph.controller().revalidate(b[0]);

        bool a_b_vc = true;
        bool b_a_vc = true;
        for (auto v : a_b_diff)
        {
            if (!util::is_in_vertex_cover(m_graph.graph(), v))
            {
                a_b_vc = false;
                break;
            }
        }

        for (auto v : b_a_diff)
        {
            if (!util::is_in_vertex_cover(m_graph.graph(), v))
            {
                b_a_vc = false;
                break;
            }
        }

        if (a_b_vc)
        {
            m_graph.controller().include_into_vertex_cover(b[0]);
            m_graph.controller().revalidate(b[0]);
        }
        else if (b_a_vc)
        {
            m_graph.controller().include_into_vertex_cover(a);
            m_graph.controller().revalidate(a[0]);
        }
        else
            throw std::logic_error("shouldnt happen");
    }
    else if (std::holds_alternative<graph_operation::edge_insert>(op))
    {
        const auto& insert = std::get<graph_operation::edge_insert>(op);
        m_graph.controller().remove_edge(insert.e.get_a(), insert.e.get_b());
    }
    else
        throw std::logic_error("Cant accept operation that hasnt been done by funnel");
}

void funnel_reduction::reject(const graph_operation::operation_type &op) {
    if (std::holds_alternative<graph_operation::invalidate>(op))
    {
        const auto& inv = std::get<graph_operation::invalidate>(op);
        auto v = inv.vertex;
        m_graph.controller().revalidate(v);
    }
    else if (std::holds_alternative<graph_operation::include>(op))
    {
        const auto& inc = std::get<graph_operation::include>(op);
        auto v = inc.vertex;
        m_graph.controller().exclude_from_vertex_cover(v);
    }
    else if (std::holds_alternative<graph_operation::alternative>(op))
    {
        const auto& alt = std::get<graph_operation::alternative>(op);
        // do nothing
        m_graph.controller().revalidate(alt.A[0]);
        m_graph.controller().revalidate(alt.B[0]);
        m_graph.controller().increase_vertex_cover_size(1);
    }
    else if (std::holds_alternative<graph_operation::edge_insert>(op))
    {
        const auto& insert = std::get<graph_operation::edge_insert>(op);
        m_graph.controller().remove_edge(insert.e.get_a(), insert.e.get_b());
    }
    else
        throw std::logic_error("Cant reject operation that hasnt been done by funnel");
}
