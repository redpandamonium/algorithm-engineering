//
// Created by Leon Suchy on 18.11.18.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <graph.hpp>

namespace
{
    using namespace ::testing;

    TEST(graph_delete_vertex, single_vertex)
    {
        graph g;
        auto v0 = g.add_vertex();
        g.remove(v0);

        EXPECT_TRUE(g.empty());
        EXPECT_EQ(0, g.data().vertices.size());
        EXPECT_EQ(0, g.data().names.size());
    }

    TEST(graph_delete_vertex, last_vertex)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();

        g.remove(v1);

        EXPECT_EQ(1, g.size());
        EXPECT_EQ(1, g.data().vertices.size());
        EXPECT_EQ(1, g.data().names.size());
    }

    TEST(graph_delete_vertex, middle_vertex_invalid_after)
    {
        std::string name = "test";

        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex(name);

        g[v2].color(1);
        g.invalidate(v2);
        g.remove(v1);

        EXPECT_EQ(1, g.size());

        // v2 should still exist
        bool v2_found = false;

        for (graph::size_type i = 0; i < g.size_including_inactive(); i++)
        {
            const vertex& vertex = g[i];
            if (vertex.color() == 1)
            {
                v2_found = true;
                EXPECT_EQ(name, g.name_of(vertex.id()));
            }
        }
        EXPECT_TRUE(v2_found);
        EXPECT_EQ(2, g.data().names.size());
    }

    TEST(graph_delete_vertex, middle_vertex_valid_after)
    {
        std::string name = "test";

        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex(name);

        g[v2].color(1);
        g.remove(v1);

        EXPECT_EQ(2, g.size());

        // v2 should still exist
        bool v2_found = false;
        for (const auto& vertex : g)
        {
            if (vertex.color() == 1)
            {
                v2_found = true;
                EXPECT_EQ(name, g.name_of(vertex.id()));
            }
        }
        EXPECT_TRUE(v2_found);
        EXPECT_EQ(2, g.data().names.size());
    }

    TEST(graph_delete_vertex, vertex_with_edges)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();

        g[v0].color(1);
        g.add_edge(v0, v1);
        g.remove(v1);

        auto v0_new = g.get_vertices_with_color(1)[0];

        EXPECT_EQ(0, g.num_edges());
        EXPECT_EQ(0, g[v0_new].neighbors_including_inactive().size());
        EXPECT_EQ(1, g.size());
    }

    TEST(graph_delete_vertex, swap_vertex_has_edges)
    {
        graph g;
        auto v0 = g.add_vertex("v0");
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex("v2");

        g[v0].color(1);
        g[v2].color(3);

        g.add_edge(v0, v2);
        g.remove(v1);

        auto v0_new = g.get_vertices_with_color(1)[0];
        auto v2_new = g.get_vertices_with_color(3)[0];

        EXPECT_TRUE(g.has_edge(v0_new, v2_new));
        EXPECT_EQ(1, g[v0_new].neighbors_including_inactive().size());
        EXPECT_EQ(1, g[v2_new].neighbors_including_inactive().size());
    }
}

#pragma clang diagnostic pop