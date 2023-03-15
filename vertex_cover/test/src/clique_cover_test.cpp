//
// Created by Leon Suchy on 10.11.18.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <graph.hpp>
#include <bounds/clique_cover_lb.hpp>

namespace
{
    using namespace ::testing;

    TEST(clique_cover, trivial)
    {
        graph g(3);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v0);

        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        graph_unit graph_u(g, meta_data, controller);

        clique_cover_lb clique_cover(graph_u, clique_cover_lb::order_options { clique_cover_lb::order::descending, clique_cover_lb::order::none });
        
        int result = clique_cover.calculate();
        
        EXPECT_EQ(2, result);
    }
    
    TEST(clique_cover, path)
    {
        graph g(3);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        graph_unit graph_u(g, meta_data, controller);

        clique_cover_lb clique_cover(graph_u, clique_cover_lb::order_options { clique_cover_lb::order::descending, clique_cover_lb::order::none });

        int result = clique_cover.calculate();

        EXPECT_EQ(1, result);
    }
    
    TEST(clique_cover, intersecting_cliques)
    {
        graph g(4);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        // two triangles intersecting on v1-v2
        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v0);

        g.add_edge(v1, v3);
        g.add_edge(v2, v3);

        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        graph_unit graph_u(g, meta_data, controller);

        clique_cover_lb clique_cover(graph_u, clique_cover_lb::order_options { clique_cover_lb::order::descending, clique_cover_lb::order::none });

        int result = clique_cover.calculate();

        EXPECT_EQ(2, result);
    }

    TEST(clique_cover, ascending_trivial)
    {
        graph g(3);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v0);

        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        graph_unit graph_u(g, meta_data, controller);

        clique_cover_lb clique_cover(graph_u, clique_cover_lb::order_options { clique_cover_lb::order::ascending, clique_cover_lb::order::none });

        int result = clique_cover.calculate();

        EXPECT_EQ(2, result);
    }

    TEST(clique_cover, ascending_path)
    {
        graph g(3);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        graph_unit graph_u(g, meta_data, controller);

        clique_cover_lb clique_cover(graph_u, clique_cover_lb::order_options { clique_cover_lb::order::ascending, clique_cover_lb::order::none });

        int result = clique_cover.calculate();

        EXPECT_EQ(1, result);
    }

    TEST(clique_cover, ascending_intersecting_cliques)
    {
        graph g(4);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        // two triangles intersecting on v1-v2
        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v0);

        g.add_edge(v1, v3);
        g.add_edge(v2, v3);

        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        graph_unit graph_u(g, meta_data, controller);

        clique_cover_lb clique_cover(graph_u, clique_cover_lb::order_options { clique_cover_lb::order::ascending, clique_cover_lb::order::none });

        int result = clique_cover.calculate();

        EXPECT_EQ(2, result);
    }
}

#pragma clang diagnostic pop
