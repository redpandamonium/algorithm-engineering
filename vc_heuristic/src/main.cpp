//
// Created by iarch on 12/15/18.
//

#include <graph_data.hpp>
#include <graph.hpp>
#include <graph_io.hpp>
#include <iostream>
#include <util.hpp>
#include "maximal_matching.hpp"
#include "max_degree_heuristic.hpp"

void apply_datareductions(graph& G)
{
    for (auto& vertex : G)
    {
        auto v_id = vertex.id();

        while(G[v_id].degree() == 1)
        {
            G.invalidate(v_id);

            // deg1 has only 1 neighbor
            auto deg1_neighbor = G[v_id].neighbors()[0];

            // neighbour of deg 1 vertex could have degree 1 too
            if (G[deg1_neighbor].degree() > 0)
            {
                v_id = G[deg1_neighbor].neighbors()[0];
            }
            else if (G[deg1_neighbor].degree() == 0)// neighbour of deg 1 vertex is now isolated
            {
                G[deg1_neighbor].color(G[deg1_neighbor].color() | max_degree_heuristic::taken);
                G.invalidate(deg1_neighbor);
                break;
            }

            G[deg1_neighbor].color(G[deg1_neighbor].color() | max_degree_heuristic::taken);
            G.invalidate(deg1_neighbor);
        }
    }

    for (auto& vertex : G)
    {
        if (vertex.degree() == 2)
        {
            auto deg2_neighbors = vertex.neighbors();
            if (G.has_edge(deg2_neighbors[0], deg2_neighbors[1]))
            {
                G[deg2_neighbors[0]].color(G[deg2_neighbors[0]].color() | max_degree_heuristic::taken);
                G[deg2_neighbors[1]].color(G[deg2_neighbors[1]].color() | max_degree_heuristic::taken);
                G.invalidate(deg2_neighbors);
                G.invalidate(vertex);
            }
        }
    }

    for (auto &vertex :G)
    {
        auto neighbors = vertex.neighbors();
        std::sort(std::begin(neighbors), std::end(neighbors));

        for (auto neighbor : neighbors)
        {
            if (G[neighbor].degree() <= vertex.degree())
            {
                auto other_neighbors = G[neighbor].neighbors();
                other_neighbors.erase(std::remove(other_neighbors.begin(), other_neighbors.end(), vertex.id()), other_neighbors.end());     // removing 'vertex' itself from the neighbours of 'neighbor'
                std::sort(std::begin(other_neighbors), std::end(other_neighbors));

                if (std::includes(neighbors.begin(), neighbors.end(), other_neighbors.begin(), other_neighbors.end()))
                {
                    vertex.color(vertex.color() | max_degree_heuristic::taken);
                    G.invalidate(vertex);
                    break;
                }
            }
        }
    }
}

int main()
{
    graph_data input_data;
    std::cin >> input_data;
    graph G (std::move(input_data));

    apply_datareductions(G);

    graph_meta_data meta_data(G, {true,false,false});
    graph_controller controller(G, meta_data);
    graph_unit unit(G, meta_data, controller);

    max_degree_heuristic max_deg(unit);
    max_deg.compute_vertex_cover_heuristic();


    for (const auto& vertex : G.data().vertices)
    {
        if (util::is_marked(G, vertex.id(), max_degree_heuristic::taken))
        {
            G.revalidate(vertex.id());
        }
    }

    std::cout << G.data();
}

