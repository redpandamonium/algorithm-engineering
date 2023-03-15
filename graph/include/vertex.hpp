//
// Created by Leon Suchy on 25.10.18.
//

#ifndef ALGORITHM_ENGINEERING_VERTEX_HPP
#define ALGORITHM_ENGINEERING_VERTEX_HPP

#include <limits>
#include <ostream>
#include <vector>

#include <boost/optional.hpp>

#include "types.hpp"
#include "edge.hpp"

/**
 * @brief Represents a vertex in a graph.
 * Each vertex has a unique ID.
 * A vertex has neighbors which it has an edge to.
 * If a neighbor is invalidated the edge becomes invalid as well.
 * A vertex can also represent a group of other vertices.
 */
class vertex
{
public:

    // Types ----------------------------------------------------------------------------

    typedef std::size_t size_type;
    typedef typename ::graphlib::detail::id_type id_type;
    typedef unsigned char color_type;

    // Constants ------------------------------------------------------------------------

    static constexpr const color_type color_false = 0;
    static constexpr const color_type color_true = 1;

    static constexpr const id_type invalid_id = std::numeric_limits<id_type>::max();

    // Operations -----------------------------------------------------------------------

    /**
     * @brief Construct an invalid vertex.
     */
    vertex();

    /**
     * @brief Construct a new vertex with a given ID.
     * @param id Unique ID
     */
    explicit vertex(id_type id);

    /**
     * @brief Default copy ctor
     * @param cpy Vertex to copy
     */
    vertex(const vertex& cpy) = default;

    /**
     * @brief Copy a vertex but assign a new ID
     * @param cpy Vertex to copy
     * @param new_id ID
     */
    vertex(const vertex& cpy, id_type new_id);

    /**
     * @brief Construct a group vertex.
     * @param id Unique vertex ID
     * @param children All the members of this group
     */
    vertex(id_type id, std::vector<id_type> children);

    /**
     * @brief Vertex degree
     * @return Degree of this vertex. Only counts active neighbors.
     */
    size_type degree() const;

    /**
     * @brief A vertex is valid if it wasn't invalidated by a delete etc.
     * @return True if the vertex is valid in the graph
     */
    bool valid() const;

    /**
     * @brief Getter
     * @return Unique vertex ID
     */
    id_type id() const;

    /**
     * @brief Getter
     * @return All active neighbors
     */
    std::vector<id_type> neighbors() const;

    /**
     * @brief Getter
     * @return Color of this vertex
     */
    color_type color() const;

    /**
     * @brief Color this vertex
     * @param c Color
     */
    void color(color_type c);

    /**
     * @brief If this vertex groups other vertices
     * @return True if this vertex represents a group
     */
    bool is_group() const;

    /**
     * @brief Getter
     * @return If this vertex groups other vertices its members will be returned
     */
    const std::vector<id_type>& children() const;

    // Operators ------------------------------------------------------------------------

    /**
     * @brief Equality
     * @param n Other
     * @return True if IDs match
     */
    bool operator==(const vertex& n) const;

    /**
     * @brief Inequality
     * @param n Other
     * @return False if IDs match
     */
    bool operator!=(const vertex& n) const;

    /**
     * @brief Order on IDs
     * @param n Other
     * @return True if this comes before n in the ordering
     */
    bool operator<(const vertex& n) const;

    /**
     * @brief Convert to string
     * @return String representation
     */
    std::string string() const;

    /**
     * @brief Output stream operator
     * @param os Output stream
     * @param n Vertex
     * @return The same output stream with the vertex inserted into it as a string
     */
    friend std::ostream& operator<<(std::ostream& os, const vertex& n);

    // Internal -------------------------------------------------------------------------

    /**
     * @brief Invalidate this vertex. Alternative to outright deleting it and producing holes in storage.
     * @note Do not use this unless implementing a vertex-based data structure.
     * @return true if successfully invalidated
     */
    bool invalidate();

    /**
     * @brief Undo a previous invalidation.
     * @note Do not use this unless implementing a vertex-based data structure.
     * @return true if successfully revalidated
     */
    bool revalidate();

    /**
     * @brief Invalidate a neighbor
     * @param neighbor Any neighbor of this vertex
     * @note Do not use this unless implementing a vertex-based data structure.
     */
    void invalidate(const vertex& neighbor);

    /**
     * @brief Revalidate a neighbor
     * @param neighbor Any invalidated neighbor of this vertex
     * @note Do not use this unless implementing a vertex-based data structure.
     */
    void revalidate(const vertex& neighbor);

    /**
     * @brief Add a directed edge from this to v.
     * @param v Neighbor
     * @return True on success
     * @note Do not use this unless implementing a vertex-based data structure.
     */
    bool add_edge_to(const vertex& v);

    bool add_edge_to(const vertex& v, ::graphlib::directed_edge::flow_type flow);

    /**
     * @brief Remove a directed edge from this to v.
     * @param v Neighbor
     * @return True on success
     * @note Do not use this unless implementing a vertex-based data structure.
     */
    bool remove_edge_to(const vertex& vertex);

    /**
     * @brief Getter
     * @return All neighbors. Even inactive ones. Use @see[degree] to determine how many are valid. Valid neighbors comes first and are contingent.
     * @note Do not use this unless implementing a vertex-based data structure.
     */
    std::vector<id_type> neighbors_including_inactive() const;

    std::vector<::graphlib::directed_edge> get_incident_edges() const;

    const std::vector<::graphlib::directed_edge>& get_incident_edges_including_inactive() const;

    std::vector<::graphlib::directed_edge>& get_incident_edges_including_inactive();

    boost::optional<const ::graphlib::directed_edge&> get_incident_edge_to(const vertex& v) const;

    boost::optional<::graphlib::directed_edge&> get_incident_edge_to(const vertex& v);

private:

    id_type m_id;
    std::vector<::graphlib::directed_edge> m_incident_edges;
    std::vector<id_type> m_children;
    color_type m_color;

    bool m_valid;
    size_type m_num_valid_neighbors;

    size_type get_neighbor_index(const vertex& v) const;

    bool add_edge_to(const vertex& v, const ::graphlib::directed_edge& e);
};

#endif //ALGORITHM_ENGINEERING_VERTEX_HPP
