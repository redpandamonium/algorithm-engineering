//
// Created by Leon Suchy on 08.11.18.
//

#ifndef ALGORITHM_ENGINEERING_GRAPH_DATA_CONTROLLER_HPP
#define ALGORITHM_ENGINEERING_GRAPH_DATA_CONTROLLER_HPP

#include <vertex.hpp>
#include <subgraph.hpp>
#include "graph_meta_data.hpp"
#include "graph_listener.hpp"

/**
 * @brief Controller that keeps graph data handling objects consistent.
 */
class graph_controller
{
public:

    /**
     * @brief Listener interface to listen for data changes.
     */

    /**
     * @brief This exception is thrown when an operation which is not legal on invalid vertices is performed.
     */
    struct operation_on_invalid_vertex_error : public std::runtime_error
    {
        operation_on_invalid_vertex_error(const graph& g, vertex::id_type vertex, const std::string& operation);
    };

    /**
     * @brief Constructor
     * @param graph Graph
     * @param meta_data Meta data associated with the graph
     */
    graph_controller(graph& graph, graph_meta_data& meta_data);

    /**
     * @brief Include a vertex into the vertex cover. The vertex will be invalid after this.
     * @param vertex ID
     */
    void include_into_vertex_cover(vertex::id_type vertex);

    /**
     * @brief Take a set of vertices into the vertex cover
     * @param vertices Vertices to take
     */
    void include_into_vertex_cover(const std::vector<vertex::id_type>& vertices);

    /**
     * @brief Exclude a vertex from the vertex cover. The vertex will be valid after this.
     * @param vertex ID
     */
    void exclude_from_vertex_cover(vertex::id_type vertex);

    /**
     * @brief Remove a set of vertices from the vertex cover
     * @param vertices
     */
    void exclude_from_vertex_cover(const std::vector<vertex::id_type>& vertices);

    /**
     * @brief Invalidate a vertex. It may be removed when invalidated.
     * @param vertex ID
     */
    void invalidate(vertex::id_type vertex);

    /**
     * @brief Make an invalid vertex valid again
     * @param vertex ID
     */
    void revalidate(vertex::id_type vertex);

    /**
     * @brief Attach a listener
     * @param listener
     */
    void attach(graph_listener& listener);

    /**
     * @brief Detach a listener from the controller
     * @param listener Listener
     */
    void detach(graph_listener& listener);

    /**
     * @brief Add an edge to the graph
     * @param a Endpoint
     * @param b Endpoint
     */
    void add_edge(vertex::id_type a, vertex::id_type b);

    /**
     * @brief Remove an edge from the graph
     * @param a Endpoint
     * @param b Endpoint
     */
    void remove_edge(vertex::id_type a, vertex::id_type b);

    vertex::id_type group(const std::vector<vertex::id_type>& group);

    std::vector<vertex::id_type> ungroup(vertex::id_type vertex);

    void set_vertex_cover_size(int k);

    [[nodiscard]] std::vector<std::unique_ptr<subgraph>> components();

    /**
     * @brief Set color bit of a vertex
     * @param vertex Vertex
     * @param bit bit to set
     */
    void mark(vertex::id_type vertex, vertex::color_type bit);

    /**
     * @brief Remove color bit of a vertex
     * @param vertex Vertex
     * @param bit bit to set
     */
    void unmark(vertex::id_type vertex, vertex::color_type bit);

    /**
     * @brief Increase k by n
     * @param n Increase
     * @note Do not use this unless you know what you're doing
     */
    void increase_vertex_cover_size(unsigned n);

    /**
     * @brief Decrease k by n
     * @param n Decreae
     * @note Do not use this unless you know what you're doing
     */
    void decrease_vertex_cover_size(unsigned n);

    void increment_recursion_depth();
    void decrement_recursion_depth();

    void lp_update_matching();
    const std::vector<edge> lp_matching();

private:

    graph& m_graph;
    graph_meta_data& m_meta_data;

    std::vector<graph_listener*> m_listeners;

    inline bool in_vertex_cover(vertex::id_type vertex);
};

#ifndef NO_DEBUG
    #define GRAPH_DATA_CONTROLLER_EXTRA_ERROR_CHECKING
#endif

#endif //ALGORITHM_ENGINEERING_GRAPH_DATA_CONTROLLER_HPP
