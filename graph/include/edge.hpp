//
// Created by Leon Suchy on 15.01.19.
//

#ifndef ALGORITHM_ENGINEERING_EDGE_HPP
#define ALGORITHM_ENGINEERING_EDGE_HPP

#include <bitset>
#include <stdexcept>
#include "types.hpp"

namespace graphlib
{
    class undirected_edge
    {
    public:

        typedef typename detail::id_type vertex_type;

        explicit undirected_edge(vertex_type a, vertex_type b);

        vertex_type get_a() const;
        vertex_type get_b() const;

        bool operator==(const undirected_edge& other) const;
        bool operator!=(const undirected_edge& other) const;

        std::string string() const;

    private:

        vertex_type m_end_a;
        vertex_type m_end_b;
    };

    class directed_edge
    {
    public:

        // Types ----------------------------------------------------------------------------

        typedef typename detail::id_type vertex_type;
        typedef uint32_t flow_type;
        typedef uint32_t weight_type;

        // Constants ------------------------------------------------------------------------

        static constexpr const flow_type infinite_capacity = std::numeric_limits<flow_type>::max();
        static constexpr const weight_type infinite_weight = std::numeric_limits<weight_type>::max();

        // Operations -----------------------------------------------------------------------

        explicit directed_edge(vertex_type from, vertex_type to);
        explicit directed_edge(vertex_type from, vertex_type to, bool weight_enabled, bool flow_enabled, weight_type weight = 0, flow_type flow = 0);

        virtual ~directed_edge() = default;

        bool operator==(const directed_edge& other) const;
        bool operator!=(const directed_edge& other) const;

        std::string string() const;

        // Functions ------------------------------------------------------------------------

        vertex_type get_from() const;
        vertex_type get_to() const;

        void add_weight(weight_type added);
        void sub_weight(weight_type subtracted);
        void set_weight(weight_type w);

        void add_flow(flow_type added);
        void sub_flow(flow_type subtracted);
        void set_flow(flow_type f);

        bool has_weight() const;
        bool has_flow() const;

        flow_type get_flow() const;
        weight_type get_weight() const;

    private:

        enum feature_enable_index
        {
            weight = 0,
            flow = 1,
        };

        vertex_type m_from;
        vertex_type m_to;

        weight_type m_weight;
        flow_type m_flow;

        std::bitset<2> m_feature_enables;
    };

    struct flow_budget_exceeded_error : public std::logic_error
    {
        flow_budget_exceeded_error(const directed_edge& edge, directed_edge::flow_type amount);
    };
}

#endif //ALGORITHM_ENGINEERING_EDGE_HPP
