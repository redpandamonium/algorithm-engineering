//
// Created by iarch on 12/15/18.
//

#include "maximal_matching.hpp"

void maximal_matching::compute_maximal_matching(graph& graph)
{
    constexpr const vertex::color_type MATCHED = 0x2;

    for (auto& vertex : graph)
    {
        if ((vertex.color() & MATCHED) != MATCHED)
        {
            for (auto neighbor : vertex.neighbors())
            {
                if ((graph[neighbor].color() & MATCHED) != MATCHED)
                {
                    vertex.color(vertex.color() | MATCHED);
                    graph[neighbor].color(graph[neighbor].color() | MATCHED);
                    break;
                }
            }
        }
    }

    for (auto& vertex : graph)
        if ((vertex.color() & MATCHED) != MATCHED)
            vertex.invalidate();
}