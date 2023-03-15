//
// Created by Leon Suchy on 17.11.18.
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

    TEST(graph_edge_counting, empty_graph)
    {
        graph g;
        EXPECT_EQ(0, g.num_edges());
    }

    TEST(graph_edge_counting, add)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();

        g.add_edge(v0, v1);
        EXPECT_EQ(1, g.num_edges());
    }

    TEST(graph_edge_counting, remove)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();

        g.add_edge(v0, v1);
        g.remove_edge(v0, v1);

        EXPECT_EQ(0, g.num_edges());
    }

    TEST(graph_edge_counting, add_invalid)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        g.invalidate(v0);
        g.add_edge(v0, v1);

        EXPECT_EQ(0, g.num_edges());
    }

    TEST(graph_edge_counting, invalidate)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        g.add_edge(v0, v1);
        g.invalidate(v0);

        EXPECT_EQ(0, g.num_edges());
    }

    TEST(graph_edge_counting, revalidate)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        g.add_edge(v0, v1);
        g.invalidate(v0);
        g.revalidate(v0);

        EXPECT_EQ(1, g.num_edges());
    }

    TEST(graph_edge_counting, group_simple)
    {
        // group vertices with no edges to the outside
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        g.add_edge(v0, v1);

        g.group({ v0, v1 });

        EXPECT_EQ(0, g.num_edges());
    }

    TEST(graph_edge_counting, group_complex)
    {
        // group vertices with edges to the outside
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v2, v0);
        g.add_edge(v3, v1);

        g.group({ v0, v1 });

        EXPECT_EQ(2, g.num_edges());
    }

    TEST(graph_edge_counting, ungroup_simple)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        g.add_edge(v0, v1);

        auto group = g.group({ v0, v1 });
        g.ungroup(group);

        EXPECT_EQ(1, g.num_edges());
    }

    TEST(graph_edge_counting, ungroup_complex)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v2, v0);
        g.add_edge(v3, v1);

        auto group = g.group({ v0, v1 });
        g.ungroup(group);

        EXPECT_EQ(3, g.num_edges());
    }

    TEST(graph_edge_counting, data_constructor)
    {
        graph_data data;
        data.vertices.emplace_back(0);
        data.vertices.emplace_back(1);
        data.vertices.emplace_back(2);

        data.vertices[0].add_edge_to(data.vertices[1]);
        data.vertices[1].add_edge_to(data.vertices[0]);
        data.vertices[0].add_edge_to(data.vertices[2]);
        data.vertices[2].add_edge_to(data.vertices[0]);
        data.vertices[1].add_edge_to(data.vertices[2]);
        data.vertices[2].add_edge_to(data.vertices[1]);

        graph g(std::move(data));

        EXPECT_EQ(3, g.num_edges());
    }

    TEST(graph_edge_counting, remove_non_existent_edge)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v1, v2);
        g.remove_edge(v0, v1);

        EXPECT_EQ(1, g.num_edges());
    }
}

#pragma clang diagnostic pop