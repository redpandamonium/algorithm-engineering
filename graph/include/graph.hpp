//
// Created by Leon Suchy on 21.10.18.
//

#ifndef ALGORITHM_ENGINEERING_GRAPH_HPP
#define ALGORITHM_ENGINEERING_GRAPH_HPP

#include <string>
#include <cstddef>  // size_t
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "vertex.hpp"
#include "graph_data.hpp"

#include "../lib/graph_iterator.hpp"

#ifndef GRAPH_DISABLE_DEBUG
#define GRAPH_DO_DOUBLE_EDGE_CHECK
#define GRAPH_DO_GROUP_EXCEPTION
#endif

struct invalid_graph_operation : std::runtime_error
{
    explicit invalid_graph_operation(const std::string& error);
};

class edge
{
public:

    edge(vertex::id_type a, vertex::id_type b);
    edge(const vertex& v1, const vertex& v2);

    vertex::id_type get_a() const;
    vertex::id_type get_b() const;

    bool operator==(const edge& e) const;
    bool operator!=(const edge& e) const;
    bool operator<(const edge& e) const;


private:

    vertex::id_type m_a, m_b;
};

/**
 * @brief Loop-free, undirected, and finite graph implementation using adjacency lists.
 * @note References may be invalidated when a new vertex is introduced. However, ID's will always stay valid.
 */
class graph
{
public:

    typedef std::size_t size_type;
    typedef graph_iterator iterator_type;
    typedef const_graph_iterator const_iterator_type;

    /**
     * @brief Construct an empty graph
     */
    graph();

    /**
     * @brief Virtual destructor
     */
    virtual ~graph() = default;

    /**
     * @brief Construct a graph with a given capacity for vertices
     * @param capacity Capacity
     */
    explicit graph(size_type capacity);

    explicit graph(graph_data&& data);

    /**
     * @brief Introduce a new vertex to the graph.
     * @return Reference to the new vertex
     */
    vertex::id_type add_vertex();

    /**
     * @brief Add a vertex with a name
     * @param name Unique name
     * @return ID of the new vertex
     */
    vertex::id_type add_vertex(const std::string& name);

    /**
     * @brief Add an undirected edge between a and b.
     * @param a Vertex
     * @param b Vertex
     */
    virtual void add_edge(vertex& a, vertex& b);

     /**
     * @brief Add an undirected edge between a and b.
     * @param a Vertex
     * @param b Vertex
     */
     virtual void add_edge(vertex::id_type a, vertex::id_type b);

    /**
     * @brief Remove a vertex from this graph.
     * @param v Vertex
     */
    void invalidate(vertex& v);

    /**
     * @brief Remove a vertex from this graph.
     * @param v Vertex
     */
    void invalidate(vertex::id_type v);

    /**
     * @brief Make an invalid vertex valid again
     * @param v Vertex
     */
    void revalidate(vertex& v);

    /**
     * @brief Make an invalid vertex valid again
     * @param v Vertex
     */
    void revalidate(vertex::id_type v);

    /**
     * @brief Remove a set of vertices
     * @param vertices Vertices
     */
    void invalidate(const std::vector<vertex::id_type>& vertices);

    /**
     * @brief Remove a vertex permanently.
     * @param v Vertex
     * @note This will invalidate all IDs in the graph
     * @complexity O(n)
     */
    void remove(vertex& v);

    /**
     * @brief Remove a vertex permanently.
     * @param v Vertex
     * @note This will invalidate all IDs in the graph
     * @complexity O(n)
     */
    void remove(vertex::id_type v_id);

    /**
     * @brief Remove an edge from this graph.
     * @param a End point a
     * @param b End point b
     */
    void remove_edge(vertex& a, vertex& b);

    /**
     * @brief Remove an edge from this graph.
     * @param a End point a
     * @param b End point b
     */
    void remove_edge(vertex::id_type a, vertex::id_type b);

    /**
     * @brief Remove an edge from the graph
     * @param e Edge
     */
    void remove(edge e);

    /**
     * @brief Check if there is an edge between 2 vertices
     * @param a vertex
     * @param b vertex
     * @return True if there is an edge between a and b
     */
    bool has_edge(const vertex& a, const vertex& b) const;

    /**
     * @brief Check if there is an edge between 2 vertices
     * @param a vertex
     * @param b vertex
     * @return True if there is an edge between a and b
     */
    bool has_edge(vertex::id_type a, vertex::id_type b) const;

    /**
     * @brief Group a set of vertices together into a new super vertex.
     * @param vertices Group members
     * @return The vertex representing the group
     */
    virtual vertex::id_type group(const std::vector<vertex::id_type>& vertices);

    /**
     * @brief Undo a previous group operation.
     * @param v Group vertex
     * @return All members of the group
     */
    const std::vector<vertex::id_type>& ungroup(vertex& v);

    /**
     * @brief Undo a previous group operation.
     * @param v Group vertex
     * @return All members of the group
     */
    const std::vector<vertex::id_type>& ungroup(vertex::id_type v);

    /**
     * @brief Getter
     * @param index Vertex index. Can be assumed to be equal to a vertex's ID
     * @return Vertex if exists nullptr otherwise
     */
    virtual vertex* get_vertex(vertex::id_type index);

    /**
     * @brief Getter
     * @param index Vertex index. Can be assumed to be equal to a vertex's ID
     * @return Vertex if exists nullptr otherwise. Returns even if the vertex is invalidated
     */
    virtual vertex* get_vertex_including_inactive(vertex::id_type index);

    /**
     * @brief Getter
     * @param index Vertex index. Can be assumed to be equal to a vertex's ID
     * @return Vertex if exists nullptr otherwise. Returns even if the vertex is invalidated
     */
    virtual const vertex* get_vertex_including_inactive(vertex::id_type index) const;

    /**
     * @brief Get all vertices of a certain color
     * @param color Color
     * @return All vertices with that color
     */
    std::vector<vertex::id_type> get_vertices_with_color(vertex::color_type color) const;

    /**
     * @brief Get the name of a vertex.
     * @param vertex ID
     * @return The name associated with the vertex. Empty if no name.
     */
    std::string name_of(vertex::id_type vertex) const;

    /**
     * @brief Getter
     * @return Number of valid vertices in the graph.
     */
    size_type size() const;

    /**
     * @brief Empty
     * @return True if no active vertices exist
     */
    bool empty() const;

    /**
     * @brief Begin iterator
     * @return Input forward iterator
     */
    iterator_type begin();

    /**
     * @brief End iterator
     * @return Input forward iterator
     */
    iterator_type end();

    /**
     * @brief Begin iterator
     * @return Input forward iterator
     */
    const_iterator_type begin() const;

    /**
     * @brief End iterator
     * @return Input forward iterator
     */
    const_iterator_type end() const;

    /**
     * @brief Constant begin iterator
     * @return Const input forward iterator
     */
    const_iterator_type cbegin() const;

    /**
     * @brief Constant end iterator
     * @return Const input forward iterator
     */
    const_iterator_type cend() const;

    /**
     * @brief Vertex getter. No validity checks.
     * @param id Vertex ID
     * @return Vertex
     */
    vertex& operator[](vertex::id_type id);

    /**
     * @brief Vertex getter. No validity checks.
     * @param id Vertex ID
     * @return Vertex
     */
    const vertex& operator[](vertex::id_type id) const;

    /**
     * @brief Internal data getter
     * @return The internal data inside the graph
     */
    const graph_data& data() const;

    /**
     * @brief Getter
     * @return Number of vertices, valid and invalid
     */
    size_type size_including_inactive() const;

    /**
     * @brief Getter
     * @return Current number of valid edges
     * @complexity Constant
     */
    size_type num_edges() const;

protected:

    graph_data m_data;

    size_type m_size;
    size_type m_num_edges;

    void add_vertex(vertex&& v);

    void group_into(vertex& parent, const std::vector<vertex::id_type>& vertices);

    virtual void on_element_replace(vertex& replaced, vertex& replacer);
    virtual void on_element_remove(vertex& removed);

private:

    friend class graph_iterator;
    friend class const_graph_iterator;

    bool verify_groupable(const std::vector<vertex::id_type>& group) const;
};

#endif //ALGORITHM_ENGINEERING_GRAPH_HPP
