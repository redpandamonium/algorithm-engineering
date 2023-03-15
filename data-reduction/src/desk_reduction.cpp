//
// Created by synis on 2/28/19.
//

#include <util.hpp>
#include "desk_reduction.hpp"

desk_reduction::desk_reduction(graph_unit unit) :
    data_reduction_node(unit),
    m_set(unit.graph().size_including_inactive()),
    m_neighbor_vertex(unit.graph().size_including_inactive(), vertex::invalid_id)
{
}

std::string desk_reduction::name() const
{
    return "desk";
}

bool desk_reduction::apply(graph_operation::stack &op_stack)
{
    bool changed = false;
    std::fill(m_neighbor_vertex.begin(), m_neighbor_vertex.end(), vertex::invalid_id);
    for (auto& v : m_graph.graph())
    {
        if (v.degree() > 4 || v.degree() < 3)
            continue;

        auto current = v.id();
        std::vector<vertex::id_type> neighbors;
        unsigned d = 0;
        for (auto n : v.neighbors())
        {
            d = m_graph.graph()[n].degree();
            m_neighbor_vertex[n] = current;
            if (d == 3 || d == 4)
                neighbors.push_back(n);
        }

        if (find_desk(current, neighbors, op_stack))
            changed = true;
    }
    return changed;
}

bool desk_reduction::find_desk(const vertex::id_type current, const std::vector<vertex::id_type>& neighbors, graph_operation::stack& op_stack)
{
    for (std::size_t i = 0; i < neighbors.size(); i++)
    {
        m_set.clear();
        auto u1 = neighbors[i];
        unsigned size_b1 = 0;
        for (auto n : m_graph.graph()[u1].neighbors())
        {
            if (n == current)
                continue;

            m_set.add(n);
            size_b1++;
        }

        for (std::size_t j = i + 1; j < neighbors.size(); j++)
        {
            auto u2 = neighbors[j];

            if (m_set.is_member(u2))
                continue;

            unsigned size_b2 = 0;
            for (auto n : m_graph.graph()[u2].neighbors())
                if (n != current && !m_set.is_member(n))
                    size_b2++;

            if (size_b1 + size_b2 > 3)
                continue;

            for (auto w : m_graph.graph()[u2].neighbors())
            {
                if (!m_set.is_member(w) || m_neighbor_vertex[w] == current)
                    continue;

                auto d = m_graph.graph()[w].degree();
                if (d == 3 || d == 4)
                {
                    auto size_a = d - 2;
                    for (auto n_w : m_graph.graph()[w].neighbors())
                        if (n_w != u1 && n_w != u2 && m_neighbor_vertex[n_w] != current)
                            size_a++;

                    if (size_a <= 2)
                    {
                        // found desk
                        std::vector<vertex::id_type> A {current, w};
                        std::vector<vertex::id_type> B {u1, u2};

                        reduce_desk(A, B, op_stack);

                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void desk_reduction::reduce_desk(
        const std::vector<vertex::id_type> A,
        const std::vector<vertex::id_type> B,
        graph_operation::stack &op_stack)
{
    if (A.size() != B.size())
        throw std::logic_error("Alternative sets need to have the same size");
    m_set.clear();
    std::vector<vertex::id_type> A_B_diff;
    std::vector<vertex::id_type> B_A_diff;

    // set intersection of neighbours N(v) AND N(u)
    for (auto a : A)
        for (auto w : m_graph.graph()[a].neighbors())
            m_set.add(w);
    for (auto b : B)
    {
        for (auto w : m_graph.graph()[b].neighbors())
        {
            if (m_set.is_member(w))
            {
                m_graph.controller().include_into_vertex_cover(w);
                op_stack.push(*this, graph_operation::include { w, nullptr });
            }
        }
    }

    // set difference N(B) \ N[A]
    m_set.clear();
    for (auto a : A)
        m_set.add(a);
    for (auto b : B)
        for (auto w : m_graph.graph()[b].neighbors())
            if (m_set.add(w))
                B_A_diff.push_back(w);

    // set difference N(A) \ N[B]
    m_set.clear();
    for (auto b : B)
        m_set.add(b);
    for (auto a : A)
        for (auto w : m_graph.graph()[a].neighbors())
            if (m_set.add(w))
                A_B_diff.push_back(w);

    for (auto a : A)
        m_graph.controller().invalidate(a);
    for (auto b : B)
        m_graph.controller().invalidate(b);

    op_stack.push(*this, graph_operation::alternative{A, B, A_B_diff, B_A_diff});
    m_graph.controller().decrease_vertex_cover_size(2);

    m_set.clear();
    for (auto a_n : A_B_diff)
    {
        for (auto b_n : B_A_diff)
        {
            if (!m_graph.graph().has_edge(a_n, b_n))
            {
                // u_n and v_n dont have an edge
                op_stack.push(*this, graph_operation::edge_insert{ edge{a_n, b_n}, nullptr });
                m_graph.controller().add_edge(a_n, b_n);
            }
        }
    }
}

void desk_reduction::accept(const graph_operation::operation_type &op)
{
    if (std::holds_alternative<graph_operation::include>(op))
    {
        const auto& inc = std::get<graph_operation::include>(op);
        auto v = inc.vertex;
        m_graph.controller().revalidate(v);
    }
    else if (std::holds_alternative<graph_operation::alternative>(op))
    {
        const auto& alt = std::get<graph_operation::alternative>(op);

        auto A = alt.A;
        auto B = alt.B;
        const auto& a_b_diff = alt.A_B_difference;
        const auto& b_a_diff = alt.B_A_difference;

        for (auto a : A)
            m_graph.controller().revalidate(a);
        for (auto b : B)
            m_graph.controller().revalidate(b);

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
            for (auto b : B)
            {
                m_graph.controller().include_into_vertex_cover(b);
                m_graph.controller().revalidate(b);
            }
        }
        else if (b_a_vc)
        {
            for (auto a : A)
            {
                m_graph.controller().include_into_vertex_cover(a);
                m_graph.controller().revalidate(a);
            }
        }
        else
            throw std::logic_error("Should not happen");
    }
    else if (std::holds_alternative<graph_operation::edge_insert>(op))
    {
        const auto& insert = std::get<graph_operation::edge_insert>(op);
        m_graph.controller().remove_edge(insert.e.get_a(), insert.e.get_b());
    }
    else
        throw std::logic_error("Cannot accept operation that has not been done by desk");
}

void desk_reduction::reject(const graph_operation::operation_type &op)
{
    if (std::holds_alternative<graph_operation::include>(op))
    {
        const auto& inc = std::get<graph_operation::include>(op);
        auto v = inc.vertex;
        m_graph.controller().exclude_from_vertex_cover(v);
    }
    else if (std::holds_alternative<graph_operation::alternative>(op))
    {
        const auto& alt = std::get<graph_operation::alternative>(op);
        // do nothing
        for (auto a : alt.A)
            m_graph.controller().revalidate(a);
        for (auto b : alt.B)
            m_graph.controller().revalidate(b);
        m_graph.controller().increase_vertex_cover_size(2);
    }
    else if (std::holds_alternative<graph_operation::edge_insert>(op))
    {
        const auto& insert = std::get<graph_operation::edge_insert>(op);
        m_graph.controller().remove_edge(insert.e.get_a(), insert.e.get_b());
    }
    else
        throw std::logic_error("Cannot reject operation that has not been done by desk");
}
