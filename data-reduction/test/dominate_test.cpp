//
// Created by Leon Suchy on 17.11.18.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <memory>
#include <utility>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <data_reduction_factory.hpp>
#include <util.hpp>

namespace
{
    using namespace ::testing;

    class data_reduction_dominate : public Test
    {
    protected:

        static void SetUpTestCase()
        {
            config = data_reduction_options { "exhaustive", 0, {{ "dominate", 0, { } }}};
            factory = data_reduction_factory(config);
        }

        static data_reduction_options config;
        static data_reduction_factory factory;
    };

    data_reduction_options data_reduction_dominate::config { };
    data_reduction_factory data_reduction_dominate::factory(data_reduction_options { });

    TEST_F(data_reduction_dominate, proper_subset)
    {
        graph g(5);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        // v0 connected to all others
        for (auto& vertex : g)
        {
            if (vertex.id() != v0)
            {
                g.add_edge(v0, vertex.id());
            }
        }

        // C_4
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v4);
        g.add_edge(v4, v1);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();

        // v0 taken into VC
        EXPECT_TRUE((g[v0].color() & vertex::color_true) == vertex::color_true);
        EXPECT_FALSE(g[v0].valid());

        // others aren't
        for (auto& vertex : g)
        {
            if (vertex.id() != v0)
            {
                EXPECT_FALSE((vertex.color() & vertex::color_true) == vertex::color_true);
                EXPECT_TRUE(vertex.valid());
            }
        }
    }

    TEST_F(data_reduction_dominate, same_neighbors_both_dominating)
    {
        graph g(4);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v0, v2);
        g.add_edge(v0, v3);
        g.add_edge(v1, v2);
        g.add_edge(v1, v3);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();

        bool v0_taken = (g[v0].color() & vertex::color_true) == vertex::color_true;
        bool v1_taken = (g[v1].color() & vertex::color_true) == vertex::color_true;

        // both dominate each other
        EXPECT_TRUE(v0_taken);
        EXPECT_TRUE(v1_taken);

        // none of the others taken
        EXPECT_FALSE((g[v2].color() & vertex::color_true) == vertex::color_true);
        EXPECT_FALSE((g[v3].color() & vertex::color_true) == vertex::color_true);
    }

    TEST_F(data_reduction_dominate, same_neighbors_only_one_dominating)
    {
        graph g(6);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();
        auto v6 = g.add_vertex();
        auto v7 = g.add_vertex();
        auto v8 = g.add_vertex();
        auto v9 = g.add_vertex();

        // 4-circle in v2-v5
        g.add_edge(v2, v3);
        g.add_edge(v3, v4);
        g.add_edge(v4, v5);
        g.add_edge(v5, v2);

        // v0 and v1 are connected to all vertices
        g.add_edge(v0, v1);
        g.add_edge(v0, v2);
        g.add_edge(v0, v3);
        g.add_edge(v0, v4);
        g.add_edge(v0, v5);
        g.add_edge(v1, v2);
        g.add_edge(v1, v3);
        g.add_edge(v1, v4);
        g.add_edge(v1, v5);

        // add support vertices
        g.add_edge(v2, v6);
        g.add_edge(v3, v7);
        g.add_edge(v4, v8);
        g.add_edge(v5, v9);

        // make support vertices cycles
        g.add_edge(v6, v7);
        g.add_edge(v8, v9);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();

        bool v0_taken = (g[v0].color() & vertex::color_true) == vertex::color_true;
        bool v1_taken = (g[v1].color() & vertex::color_true) == vertex::color_true;

        EXPECT_TRUE(v0_taken ^ v1_taken);

        EXPECT_FALSE((g[v2].color() & vertex::color_true) == vertex::color_true);
        EXPECT_FALSE((g[v3].color() & vertex::color_true) == vertex::color_true);
        EXPECT_FALSE((g[v4].color() & vertex::color_true) == vertex::color_true);
        EXPECT_FALSE((g[v5].color() & vertex::color_true) == vertex::color_true);
    }

    TEST_F(data_reduction_dominate, no_shared_neighbors)
    {
        graph g(4);
        auto v4 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v4);
        g.add_edge(v4, v1);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();

        // none dominate
        EXPECT_FALSE((g[v2].color() & vertex::color_true) == vertex::color_true);
        EXPECT_FALSE((g[v3].color() & vertex::color_true) == vertex::color_true);
        EXPECT_FALSE((g[v4].color() & vertex::color_true) == vertex::color_true);
        EXPECT_FALSE((g[v1].color() & vertex::color_true) == vertex::color_true);
    }

    TEST_F(data_reduction_dominate, graph_one_vertex)
    {
        graph g(1);
        g.add_vertex();

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        EXPECT_EQ(10, meta_data.vertex_cover_size());
    }
}