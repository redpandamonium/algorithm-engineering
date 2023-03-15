//
// Created by Leon Suchy on 28.10.18.
//

#include <util.hpp>

#include "util.hpp"

bool util::is_in_vertex_cover(const graph& g, vertex::id_type vertex)
{
    return is_in_vertex_cover(g[vertex]);
}

bool util::is_in_vertex_cover(const vertex& vertex)
{
    return (vertex.color() & vertex::color_true) == vertex::color_true;
}

bool util::is_marked(const vertex &vertex, vertex::color_type bit)
{
    return (vertex.color() & bit) == bit;
}

bool util::is_marked(const graph &g, vertex::id_type vertex, vertex::color_type bit)
{
    return is_marked(g[vertex], bit);
}

uint32_t util::amount_edges(const graph &g)
{
    uint32_t edges = 0;
    for (auto& vertex : g)
    {
        edges += vertex.degree();
    }

    return (edges / 2);
}

vertex::size_type util::neighbor_degree(const graph& g, vertex::id_type v)
{
    vertex::size_type result = 0;

    auto neighbors = g[v].neighbors();
    for (auto neighbor : neighbors)
        result += g[neighbor].degree() - 1;

    return result;
}

bool util::equals_ignore_case(const std::string& a, const std::string& b)
{
    if (a.size() != b.size())
        return false;

    for (char i : a)
    {
        for (char j : b)
        {
            if (std::tolower(i) != std::tolower(j))
                return false;
        }
    }

    return true;
}
