//
// Created by Leon Suchy on 16.12.18.
//

#ifndef ALGORITHM_ENGINEERING_CHANGE_LISTENER_HPP
#define ALGORITHM_ENGINEERING_CHANGE_LISTENER_HPP

#include <graph.hpp>
#include <vertex.hpp>

struct graph_listener
{
    virtual ~graph_listener() = default;

    /**
     * @brief Called when a vertex is taken into the vertex cover.
     * @param vertex
     */
    virtual void on_vertex_cover_include(vertex::id_type vertex) = 0;

    /**
     * @brief Called when a vertex is removed from the vertex cover.
     * @param vertex
     */
    virtual void on_vertex_cover_exclude(vertex::id_type vertex) = 0;

    /**
     * @brief Called when a vertex was valid and is now invalid.
     * @param vertex ID
     */
    virtual void on_add(vertex::id_type vertex) = 0;

    /**
     * @brief Called when a vertex was invalid and is now valid again.
     * @param vertex ID
     */
    virtual void on_remove(vertex::id_type vertex) = 0;

    /**
     * @brief Called when an edge was added or revalidated
     * @param e New edge
     */
    virtual void on_add(edge e) = 0;

    /**
     * @brief Called when an edge is removed or invalidated
     * @param e
     */
    virtual void on_remove(edge e) = 0;
};

#endif //ALGORITHM_ENGINEERING_CHANGE_LISTENER_HPP
