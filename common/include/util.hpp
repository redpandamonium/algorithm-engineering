//
// Created by Leon Suchy on 28.10.18.
//

#ifndef ALGORITHM_ENGINEERING_UTIL_HPP
#define ALGORITHM_ENGINEERING_UTIL_HPP

#include <type_traits>

#include <vertex.hpp>
#include <graph.hpp>

struct util
{
    static bool is_in_vertex_cover(const graph& g, vertex::id_type vertex);
    static bool is_in_vertex_cover(const vertex& vertex);

    static bool is_marked(const vertex& vertex, vertex::color_type bit);
    static bool is_marked(const graph& g, vertex::id_type vertex, vertex::color_type bit);
    [[deprecated("Use graph::num_edges instead")]] static uint32_t amount_edges(const graph& g);

    [[nodiscard]] static bool equals_ignore_case(const std::string& a, const std::string& b);

    /**
     * @brief Get the total degree of all neighbors of v
     * @param g Graph
     * @param v Vertex
     * @return Sum of all degrees in the neighborhood of v
     */
    static vertex::size_type neighbor_degree(const graph& g, vertex::id_type v);
};


#endif //ALGORITHM_ENGINEERING_UTIL_HPP
