//
// Created by Leon Suchy on 27.10.18.
//

#ifndef ALGORITHM_ENGINEERING_DATA_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_DATA_REDUCTION_HPP

#include <string>
#include <graph.hpp>
#include <variant>
#include <graph_unit.hpp>

namespace graph_operation
{
    struct group;
    struct ungroup;
    struct include;
    struct exclude;
    struct invalidate;
    struct revalidate;
    struct edge_insert;
    struct edge_remove;
    struct neighborhood_clique_partition;
    struct data_reduction_vertices;
    struct folding;
    struct alternative;
    class stack;
    typedef std::variant<group, ungroup, include, exclude, invalidate, revalidate, edge_insert, edge_remove, neighborhood_clique_partition, data_reduction_vertices, folding, alternative> operation_type;
}
#include "graph_operation.hpp"

class data_reduction_node
{
public:

    /**
     * @brief Constructor
     * @param graph Graph unit
     */
    explicit data_reduction_node(graph_unit graph);

    /**
     * @brief Destructor
     */
    virtual ~data_reduction_node() = default;

    /**
     * @brief Getter.
     * @return Name of the data reduction
     */
    virtual std::string name() const = 0;

    /**
     * @brief Applies this data reduction once
     * @return True if the graph was modified
     */
    virtual bool apply(graph_operation::stack& op_stack) = 0;

    /**
     * @brief Resolve all groupings
     */
    virtual void accept(const graph_operation::operation_type& op) = 0;

    /**
     * @brief Undos all changes made by this reduction
     */
    virtual void reject(const graph_operation::operation_type& op) = 0;

protected:

    graph_unit m_graph;
};

#endif //ALGORITHM_ENGINEERING_DATA_REDUCTION_HPP
