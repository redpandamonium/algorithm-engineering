//
// Created by Leon Suchy on 22.11.18.
//

#include "components.hpp"
#include "graph_search.hpp"

std::vector<std::unique_ptr<subgraph>> components::subgraphs(graph& g)
{
    std::vector<std::unique_ptr<subgraph>> components;

    for (vertex& start_point : g)
    {
        // skip the vertices that are already taken
        if (is_marked(start_point))
            continue;

        std::vector<vertex::id_type> component;

        // start dfs from this vertex
        for (vertex& v : dfs(g, start_point.id()))
        {
            component.push_back(v.id());
            mark(v);
        }

        // create subgraph from component
        components.push_back(std::make_unique<subgraph>(g, component));
    }

    // revert coloring
    unmark(g);
    for (std::unique_ptr<subgraph>& sub: components)
        unmark(*sub);

    return components;
}

void components::mark(vertex& v)
{
    v.color(v.color() | marked);
}

bool components::is_marked(const vertex& v)
{
    return (v.color() & marked) == marked;
}

void components::unmark(graph& g)
{
    for (const vertex& v : g.data().vertices)
    {
        //v.color(v.color() & (~marked));
        g[v.id()].color(v.color() & (~marked));
    }
}
