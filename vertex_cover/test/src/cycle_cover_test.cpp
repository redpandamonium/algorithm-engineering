//
// Created by synis on 1/31/19.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <graph.hpp>
#include <bounds/cycle_cover_lb.hpp>

namespace
{
    using namespace ::testing;

    TEST(cycle_cover_test, triangle)
    {
        graph g;

        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v0, v2);
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        graph_unit graph_u(g, meta_data, controller);

        cycle_cover_lb lb(graph_u);

        auto solution = lb.calculate();

        EXPECT_TRUE(solution == 2);

    }
    TEST(cycle_cover_test, trivial)
    {
        graph g;

        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v0, v5);
        g.add_edge(v1, v2);
        g.add_edge(v1, v5);
        g.add_edge(v2, v3);
        g.add_edge(v2, v4);
        g.add_edge(v2, v5);
        g.add_edge(v3, v4);

        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        graph_unit graph_u(g, meta_data, controller);

        cycle_cover_lb lb(graph_u);

        auto solution = lb.calculate();
        EXPECT_TRUE(solution == 4); // 2 - 3 length circles with (v0, v1, v5) (v2, v3, v4)
    }

    TEST(cycle_cover_test, almost_cycle)
    {
        graph g;

        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();

        // make an almost cycle (dont connect v0 and v5)
        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v4);
        g.add_edge(v4, v5);

        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        graph_unit graph_u(g, meta_data, controller);

        cycle_cover_lb lb(graph_u);

        auto solution = lb.calculate();

        EXPECT_TRUE(solution == 3); // 3 2-length cycles
    }
}