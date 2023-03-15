//
// Created by toad on 11/23/18.
//

// googletest
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// graph
#include <graph.hpp>
#include <graph_io.hpp>
#include <graph_controller.hpp>
#include <graph_meta_data.hpp>

namespace
{
    using namespace ::testing;

    TEST(graph_controller_test, invalidate)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v1, v2);

        graph_meta_data meta_data(g);
        meta_data.vertex_cover_size(2);
        graph_controller controller(g, meta_data);

        // valid before invalidating
        EXPECT_TRUE(g[v1].valid());
        controller.invalidate(v1);
        // invalid in g after invalidating
        EXPECT_FALSE(g[v1].valid());

        controller.invalidate(v1);

        // invalidating again does not change anything
        EXPECT_FALSE(g[v1].valid());
    }

    TEST(graph_controller_test, revalidate)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v1, v2);

        graph_meta_data meta_data(g);
        meta_data.vertex_cover_size(2);
        graph_controller controller(g, meta_data);

        controller.invalidate(v1);

        controller.revalidate(v1);
        EXPECT_TRUE(g[v1].valid());
        controller.revalidate(v1);
        EXPECT_TRUE(g[v1].valid());
    }

    TEST(graph_controller_test, include)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.add_edge(v1, v2);
        g.add_edge(v1, v3);
        g.add_edge(v3, v4);

        auto cover_size = 2;
        graph_meta_data meta_data(g);
        meta_data.vertex_cover_size(cover_size);
        graph_controller controller(g, meta_data);

        controller.include_into_vertex_cover(v3);

        EXPECT_FALSE(g[v3].valid());
        EXPECT_EQ(meta_data.vertex_cover_size(), 1);
        EXPECT_EQ(g[v3].color(), vertex::color_true);

        controller.include_into_vertex_cover(v3);

        EXPECT_FALSE(g[v3].valid());
        EXPECT_EQ(meta_data.vertex_cover_size(), 1);
        EXPECT_EQ(g[v3].color(), vertex::color_true);

        controller.include_into_vertex_cover(v1);

        EXPECT_EQ(meta_data.vertex_cover_size(), 0);
    }

    TEST(graph_controller_test, exclude)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.add_edge(v1, v2);
        g.add_edge(v1, v3);
        g.add_edge(v3, v4);

        auto cover_size = 2;
        graph_meta_data meta_data(g);
        meta_data.vertex_cover_size(cover_size);
        graph_controller controller(g, meta_data);

        controller.exclude_from_vertex_cover(v3);
        EXPECT_TRUE(g[v3].valid());

        controller.include_into_vertex_cover(v3);

        controller.exclude_from_vertex_cover(v3);
        EXPECT_TRUE(g[v3].valid());
        EXPECT_EQ(meta_data.vertex_cover_size(), 2);
        EXPECT_EQ(g[v3].color(), vertex::color_false);
    }

    TEST(graph_controller_test, grouping)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.add_edge(v1, v2);
        g.add_edge(v1, v3);
        g.add_edge(v3, v4);

        auto cover_size = 2;
        graph_meta_data meta_data(g);
        meta_data.vertex_cover_size(cover_size);
        graph_controller controller(g, meta_data);

        std::vector<vertex::id_type> group;
        group.emplace_back(v1);
        group.emplace_back(v2);
        auto group_v = controller.group(group);

        EXPECT_TRUE(g[group_v].is_group());

        controller.ungroup(group_v);

        EXPECT_FALSE(g[group_v].valid());

        EXPECT_EQ(g.size(), 4);
    }
}