//
// Created by Leon Suchy on 15.01.19.
//

#include <edge.hpp>

#include "edge.hpp"

using std::to_string;

namespace graphlib
{
    undirected_edge::undirected_edge(vertex_type a, vertex_type b)
        : m_end_a(a), m_end_b(b)
    { }

    typename undirected_edge::vertex_type undirected_edge::get_a() const
    {
        return m_end_a;
    }

    typename undirected_edge::vertex_type undirected_edge::get_b() const
    {
        return m_end_b;
    }

    bool undirected_edge::operator==(const undirected_edge& other) const
    {
        return (m_end_a == other.m_end_a || m_end_a == other.m_end_b) &&  (m_end_b == other.m_end_a || m_end_b == other.m_end_b);
    }

    bool undirected_edge::operator!=(const undirected_edge& other) const
    {
        return !operator==(other);
    }

    std::string undirected_edge::string() const
    {
        // put standard string conversion in the same namespace as user-provided ones to allow overloading
        // without std namespace pollution
        using std::to_string;

        return "[ v" + to_string(m_end_a) + ", v" + to_string(m_end_b) + "]";
    }

    directed_edge::directed_edge(directed_edge::vertex_type from, directed_edge::vertex_type to)
        : m_from(from), m_to(to), m_weight(0), m_flow(0), m_feature_enables(0)
{ }

    bool directed_edge::operator==(const directed_edge& other) const
    {
        return m_from == other.m_from && m_to == other.m_to;
    }

    bool directed_edge::operator!=(const directed_edge& other) const
    {
        return !operator==(other);
    }

    std::string directed_edge::string() const
    {
        // put standard string conversion in the same namespace as user-provided ones to allow overloading
        // without std namespace pollution
        using std::to_string;

        return "[ v" + to_string(m_from) + "->v" + to_string(m_to) + "]";
    }

    directed_edge::vertex_type directed_edge::get_from() const
    {
        return m_from;
    }

    directed_edge::vertex_type directed_edge::get_to() const
    {
        return m_to;
    }

    void directed_edge::add_weight(directed_edge::weight_type added)
    {
        if (!has_weight())
            throw std::logic_error("Cannot change weight if it is disabled");

        if (m_weight == infinite_weight)
            return;

        m_weight += added;
    }

    void directed_edge::sub_weight(directed_edge::weight_type subtracted)
    {
        if (!has_weight())
            throw std::logic_error("Cannot change weight if it is disabled");

        if (m_weight == infinite_weight)
            return;

        m_weight -= subtracted;
    }

    void directed_edge::set_weight(directed_edge::weight_type w)
    {
        m_feature_enables[feature_enable_index::weight] = true;
        m_weight = w;
    }

    void directed_edge::add_flow(directed_edge::flow_type added)
    {
        if (!has_flow())
            throw std::logic_error("Cannot change flow if it is disabled");

        // infinite flow cannot be changed
        if (m_flow == infinite_capacity)
            return;

        m_flow += added;
    }

    void directed_edge::sub_flow(directed_edge::flow_type subtracted)
    {
        if (!has_flow())
            throw std::logic_error("Cannot change flow if it is disabled");

        // infinite flow cannot be changed
        if (m_flow == infinite_capacity)
            return;

        if (m_flow < subtracted)
            throw flow_budget_exceeded_error(*this, subtracted);

        m_flow -= subtracted;
    }

    void directed_edge::set_flow(directed_edge::flow_type f)
    {
        m_feature_enables[feature_enable_index::flow] = true;
        m_flow = f;
    }

    bool directed_edge::has_weight() const
    {
        return m_feature_enables[feature_enable_index::weight];
    }

    bool directed_edge::has_flow() const
    {
        return m_feature_enables[feature_enable_index::flow];
    }

    directed_edge::directed_edge(directed_edge::vertex_type from, directed_edge::vertex_type to,
                                 bool weight_enabled, bool flow_enabled,
                                 weight_type weight, flow_type flow)
        : m_from(from), m_to(to), m_weight(weight), m_flow(flow)
    {
        m_feature_enables[feature_enable_index::flow] = flow_enabled;
        m_feature_enables[feature_enable_index::weight] = weight_enabled;
    }

    typename directed_edge::flow_type directed_edge::get_flow() const
    {
        if (!has_flow())
            throw std::logic_error("Cannot get flow if the feature is disabled");
        return m_flow;
    }

    typename directed_edge::weight_type directed_edge::get_weight() const
    {
        if (!has_weight())
            throw std::logic_error("Cannot get weight if the feature is disabled");
        return m_weight;
    }

    flow_budget_exceeded_error::flow_budget_exceeded_error(const directed_edge& edge, directed_edge::flow_type amount)
        : std::logic_error("Cannot remove " + to_string(amount) + " flow from edge " + edge.string() + ": Budget exceeded")
    { }
}