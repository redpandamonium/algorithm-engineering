//
// Created by Leon Suchy on 12.11.18.
//

#include <graph_operation.hpp>

#include "graph_operation.hpp"

namespace graph_operation
{
    void stack::push(stack::operation&& op)
    {
        m_operation_stack.push(std::move(op));
    }

    stack::operation stack::pop()
    {
        operation op(m_operation_stack.top());
        m_operation_stack.pop();
        return op;
    }

    stack::operation& stack::top()
    {
        return m_operation_stack.top();
    }

    const stack::operation& stack::top() const
    {
        return m_operation_stack.top();
    }

    size_t stack::size() const
    {
        return m_operation_stack.size();
    }

    bool stack::empty() const
    {
        return m_operation_stack.empty();
    }

    void stack::push(data_reduction_node& actor, group&& op)
    {
        push(operation { actor, operation_type { op }});
    }

    void stack::push(data_reduction_node& actor, ungroup&& op)
    {
        push(operation { actor, operation_type { op }});
    }

    void stack::push(data_reduction_node& actor, include&& op)
    {
        push(operation { actor, operation_type { op }});
    }

    void stack::push(data_reduction_node& actor, exclude&& op)
    {
        push(operation { actor, operation_type { op }});
    }

    void stack::push(data_reduction_node& actor, invalidate&& op)
    {
        push(operation { actor, operation_type { op }});
    }

    void stack::push(data_reduction_node& actor, revalidate&& op)
    {
        push(operation { actor, operation_type { op }});
    }

    void stack::push(data_reduction_node& actor, edge_insert&& op)
    {
        push(operation { actor, operation_type { op }});
    }

    void stack::push(data_reduction_node& actor, edge_remove&& op)
    {
        push(operation { actor, operation_type { op }});
    }

    void stack::push(data_reduction_node& actor, neighborhood_clique_partition&& op)
    {
        push(operation { actor, operation_type { op }});
    }

    void stack::push(data_reduction_node &actor, data_reduction_vertices &&op)
    {
        push(operation { actor, operation_type { op }});
    }

    void stack::push(data_reduction_node& actor, folding&& op)
    {
        push(operation { actor, operation_type { op }});
    }

    void stack::push(data_reduction_node &actor, graph_operation::alternative &&op)
    {
        push(operation {actor, operation_type {op}});
    }
}
