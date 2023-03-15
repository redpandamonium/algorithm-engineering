//
// Created by iarch on 12/15/18.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <graph.hpp>
#include <graph_unit.hpp>
#include <max_degree_heuristic.hpp>
#include <graph_writer.hpp>
#include <random_graph.hpp>

namespace
{
    using namespace ::testing;

    TEST(heuristic, simple_graph)
    {
        graph g(4);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v4);
        g.add_edge(v4, v0);

        random_graph_generator generator(20);

        for (auto i = 0; i < 1000; i++)
        {
            generated_graph g1 = std::move(generator.generate());
            graph_meta_data meta_data(g1.graph_);
            graph_controller controller(g1.graph_, meta_data);
            graph_unit graph(g1.graph_, meta_data, controller);

            std::cerr << "Solving " << g1.seed_ << " size " << graph.graph().size() << ", edges " << graph.graph().size() << std::endl;

            max_degree_heuristic max_deg(graph);

            max_deg.compute_vertex_cover_heuristic();
        }


        /*
        graph_writer writer(std::cerr);
        writer.write(g.data());
        std::cerr << "SIZE: " << g.size() << std::endl;
         */

    }

    TEST(heuristic, super_simple)
    {
        graph g(2);

        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();

        g.add_edge(v0, v1);

        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        graph_unit graph(g, meta_data, controller);

        max_degree_heuristic max_deg(graph);

        max_deg.compute_vertex_cover_heuristic();
    }
}