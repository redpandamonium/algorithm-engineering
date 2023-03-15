//
// Created by Leon Suchy on 10.11.18.
//

#ifndef ALGORITHM_ENGINEERING_BIPARTITE_GRAPH_HPP
#define ALGORITHM_ENGINEERING_BIPARTITE_GRAPH_HPP

#include "graph.hpp"

/**
 * @brief A graph that consists of two graphs. The only edges allowed are between those graphs.
 */
class bipartite_graph : public graph
{
public:

    /**
     * @brief Create an empty bipartite graph
     */
    bipartite_graph();

    /**
     * @brief Create an empty bipartite graph
     * @param capacity Capacity hint for the underlying storage
     */
    explicit bipartite_graph(size_type capacity);

    /**
     * @brief Create a bipartite graph from two graphs
     * @param left Left partition
     * @param right Right partition
     */
    bipartite_graph(const graph& left, const graph& right);

    /**
     * @brief Get the ID of a vertex inside a partition graph
     * @param id ID inside this graph
     * @return ID of the corresponding vertex inside the partition graph
     * @note Only meaningful if the bipartite graph actually represents two partition graphs
     */
    vertex::id_type in_original_graph(vertex::id_type id) const;

    /**
     * @brief Add a vertex on the left side
     * @return The ID of the created vertex
     */
    virtual vertex::id_type add_vertex_left();

    /**
     * @brief Add a vertex on the right side
     * @return The ID of the created vertex
     */
    virtual vertex::id_type add_vertex_right();

    /**
     * @brief Check if a vertex inside the graph is inside the left partition
     * @param id ID in the context of this graph
     * @return True if the vertex is part of the left partition
     */
    bool is_left(vertex::id_type id) const;

    /**
     * @brief Check if a vertex inside the graph is inside the right partition
     * @param id ID in the context of this graph
     * @return True if the vertex is part of the right partition
     */
    bool is_right(vertex::id_type id) const;

    /**
     * @brief Get all vertices in the left partition
     * @return All valid vertices on the left side
     */
    std::vector<vertex::id_type> get_left() const;

    /**
     **
     * @brief Get all vertices in the right partition
     * @return All valid vertices on the right side
     */
    std::vector<vertex::id_type> get_right() const;

    std::vector<vertex::id_type> get_left_including_invalid() const;

    std::vector<vertex::id_type> get_right_including_invalid() const;

    /**
     * @brief Add an undirected edge between a and b.
     * @param a Vertex
     * @param b Vertex
     */
    void add_edge(vertex& a, vertex& b) override;

    void add_edge(vertex::id_type a, vertex::id_type b) override;

    /**
     * @brief Group a set of vertices together into a new super vertex.
     * @param vertices Group members
     * @return The vertex representing the group
     */
    vertex::id_type group(const std::vector<vertex::id_type>& vertices) override;

    /**
     * @brief Getter
     * @param index Vertex index. Can be assumed to be equal to a vertex's ID
     * @return Vertex if exists nullptr otherwise. Returns even if the vertex is invalidated
     */
    vertex* get_vertex_including_inactive(vertex::id_type index) override;

    /**
     * @brief Getter
     * @param index Vertex index. Can be assumed to be equal to a vertex's ID
     * @return Vertex if exists nullptr otherwise. Returns even if the vertex is invalidated
     */
    const vertex* get_vertex_including_inactive(vertex::id_type index) const override;

    /**
     * @brief Get the ID a vertex at the i-th position in the left list has
     * @param i Index inside the left partition
     * @return ID of the vertex
     */
    vertex::id_type left_id_at_position(vertex::id_type i) const;

    /**
     * @brief Get the ID a vertex at the i-th position in the right list has
     * @param i Index inside the right partition
     * @return ID of the vertex
     */
    vertex::id_type right_id_at_position(vertex::id_type i) const;

private:

    size_t m_left_size;
    size_t m_right_size;

    /**
     * @brief Index for the underlying vector
     * @param i i-th element on the left
     * @return Index of this vertex in the vector
     */
    size_type index_left(size_type i) const;

    /**
     * @brief Index for the underlying vector
     * @param i i-th element on the right
     * @return Index of this vertex in the vector
     */
    size_type index_right(size_type i) const;

    vertex::id_type add_vertex_left(vertex&& v);

    vertex::id_type add_vertex_right(vertex&& v);
};

#ifndef NO_DEBUG
#define BIPARTITE_GRAPH_VERBOSE_ERRORS 1
#endif

/**
 * @brief Exception for when an edge between two vertices in the same partition is created
 */
struct edge_inside_bipartite_partition : public std::logic_error
{
    edge_inside_bipartite_partition(const graph& g, const vertex& a, const vertex& b);
};

/**
 * @brief Exception for when a group operation spans both partitions
 */
struct group_over_multiple_partitions : public std::runtime_error
{
    group_over_multiple_partitions(const graph& g, const std::vector<vertex::id_type>& vertices);

private:

    std::string list_vertices(const graph& g, const std::vector<vertex::id_type>& vertices);
};

#endif //ALGORITHM_ENGINEERING_BIPARTITE_GRAPH_HPP
