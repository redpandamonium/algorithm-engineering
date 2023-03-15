//
// Created by Leon Suchy on 10.11.18.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <bipartite_graph.hpp>

namespace
{
    using namespace ::testing;


    TEST(bipartite_graph, empty_graph)
    {
        bipartite_graph g;
        EXPECT_EQ(0, g.size());
    }

    TEST(bipartite_graph, add_left)
    {
        bipartite_graph g(2);
        auto v0 = g.add_vertex_left();
        EXPECT_EQ(0, v0);
        EXPECT_EQ(1, g.size());
    }

    TEST(bipartite_graph, add_right)
    {
        bipartite_graph g(2);
        auto v0 = g.add_vertex_right();
        auto right = g.get_right();
        EXPECT_EQ(right[0], v0);
        EXPECT_EQ(1, g.size());
    }

    TEST(bipartite_graph, add_left_fill_empty_spot)
    {
        bipartite_graph g(2);
        auto v0 = g.add_vertex_right();
        auto v1 = g.add_vertex_left();

        EXPECT_EQ(0, v1);
        EXPECT_EQ(2, g.size());
    }

    TEST(bipartite_graph, add_right_fill_empty_spot)
    {
        bipartite_graph g(2);
        auto v0 = g.add_vertex_left();
        auto v1 = g.add_vertex_right();

        auto right = g.get_right();
        EXPECT_EQ(right[0], v1);
        EXPECT_EQ(2, g.size());
    }

    TEST(bipartite_graph, add_left_second)
    {
        bipartite_graph g(4);
        auto v0 = g.add_vertex_left();
        auto v1 = g.add_vertex_right();
        auto v2 = g.add_vertex_left();

        EXPECT_EQ(1, v2);
        EXPECT_EQ(3, g.size());
    }

    TEST(bipartite_graph, add_right_second)
    {
        bipartite_graph g(4);
        auto v0 = g.add_vertex_right();
        auto v1 = g.add_vertex_left();
        auto v2 = g.add_vertex_right();

        auto right = g.get_right();
        EXPECT_THAT(right, Contains(v2));
        EXPECT_EQ(3, g.size());
    }

    TEST(bipartite_graph, add_left_consecutive)
    {
        bipartite_graph g(4);
        auto v0 = g.add_vertex_left();
        auto v1 = g.add_vertex_left();

        EXPECT_EQ(1, v1);
        EXPECT_EQ(2, g.size());
    }

    TEST(bipartite_graph, add_right_consecutive)
    {
        bipartite_graph g(4);
        auto v0 = g.add_vertex_right();
        auto v1 = g.add_vertex_right();

        auto right = g.get_right();
        EXPECT_THAT(right, Contains(v1));
        EXPECT_EQ(2, g.size());
    }

    TEST(bipartite_graph, is_left)
    {
        bipartite_graph g(2);
        auto v0 = g.add_vertex_left();

        EXPECT_TRUE(g.is_left(v0));
        EXPECT_FALSE(g.is_right(v0));
    }

    TEST(bipartite_graph, is_right)
    {
        bipartite_graph g(2);
        auto v0 = g.add_vertex_right();

        EXPECT_TRUE(g.is_right(v0));
        EXPECT_FALSE(g.is_left(v0));
    }

    TEST(bipartite_graph, construct_from_graphs)
    {
        graph g1(3);
        auto g1v0 = g1.add_vertex();
        auto g1v1 = g1.add_vertex();
        auto g1v2 = g1.add_vertex();

        g1.add_edge(g1v0, g1v1);
        g1.add_edge(g1v1, g1v2);
        g1.add_edge(g1v2, g1v0);

        graph g2(3);
        auto g2v0 = g2.add_vertex();
        auto g2v1 = g2.add_vertex();
        auto g2v2 = g2.add_vertex();

        g2.add_edge(g2v0, g2v1);
        g2.add_edge(g2v1, g2v2);
        g2.add_edge(g2v2, g2v0);

        bipartite_graph g3 { g1, g2 };

        EXPECT_EQ(6, g3.size());
        EXPECT_EQ(3, g3.get_left().size());
        EXPECT_EQ(3, g3.get_right().size());
    }

    TEST(bipartite_graph, exception_on_edge_inside_component)
    {
        graph g1(3);
        auto g1v0 = g1.add_vertex();
        auto g1v1 = g1.add_vertex();
        auto g1v2 = g1.add_vertex();

        graph g2(3);
        auto g2v0 = g2.add_vertex();
        auto g2v1 = g2.add_vertex();
        auto g2v2 = g2.add_vertex();

        bipartite_graph g3 { g1, g2 };

        auto left = g3.get_left();
        EXPECT_THROW(g3.add_edge(left[0], left[1]), edge_inside_bipartite_partition);
    }

    TEST(bipartite_graph, exception_on_group_between_both_components)
    {
        graph g1(3);
        auto g1v0 = g1.add_vertex();
        auto g1v1 = g1.add_vertex();
        auto g1v2 = g1.add_vertex();

        graph g2(3);
        auto g2v0 = g2.add_vertex();
        auto g2v1 = g2.add_vertex();
        auto g2v2 = g2.add_vertex();

        bipartite_graph g3 { g1, g2 };

        auto left = g3.get_left();
        auto right = g3.get_right();
        EXPECT_THROW(g3.group({left[0], right[0]}), group_over_multiple_partitions);
    }

    TEST(bipartite_graph, group)
    {
        graph g1(3);
        auto g1v0 = g1.add_vertex();
        auto g1v1 = g1.add_vertex();
        auto g1v2 = g1.add_vertex();

        graph g2(3);
        auto g2v0 = g2.add_vertex();
        auto g2v1 = g2.add_vertex();
        auto g2v2 = g2.add_vertex();

        bipartite_graph g3 { g1, g2 };

        auto left = g3.get_left();
        auto right = g3.get_right();

        // group the left side into one
        auto group_vertex = g3.group(std::vector<vertex::id_type> {left[0], left[1], left[2]});

        EXPECT_EQ(4, g3.size());
        EXPECT_EQ(1, g3.get_left().size());
        EXPECT_TRUE(g3.is_left(group_vertex));
    }

    TEST(bipartite_graph, fynn_bug)
    {
        bipartite_graph g;
        auto v0 = g.add_vertex_left();
        auto v1 = g.add_vertex_left();
        auto v2 = g.add_vertex_right();
        auto v3 = g.add_vertex_right();

        EXPECT_NE(v0, v1);
        EXPECT_NE(v2, v3);
    }

    // a bipartite graph made of two graphs should map its vertices to the original ones
    TEST(bipartite_graph, bipartite_to_partition_graph_id)
    {
        graph g1(3);
        auto g1v0 = g1.add_vertex();
        auto g1v1 = g1.add_vertex();
        auto g1v2 = g1.add_vertex();

        graph g2(3);
        auto g2v0 = g2.add_vertex();
        auto g2v1 = g2.add_vertex();
        auto g2v2 = g2.add_vertex();

        bipartite_graph g3 { g1, g2 };

        auto right = g3.get_right();
        for (auto& v : right)
            v = g3.in_original_graph(v);

        EXPECT_THAT(right, Contains(g2v0));
        EXPECT_THAT(right, Contains(g2v1));
        EXPECT_THAT(right, Contains(g2v2));
    }

    TEST(bipartite_graph, id_to_index_map)
    {
        graph g1(3);
        auto g1v0 = g1.add_vertex();
        auto g1v1 = g1.add_vertex();
        auto g1v2 = g1.add_vertex();

        graph g2(3);
        auto g2v0 = g2.add_vertex();
        auto g2v1 = g2.add_vertex();
        auto g2v2 = g2.add_vertex();

        bipartite_graph g3 { g1, g2 };

        // for each vertex in the graph its ID to index mapping should result in the same vertex being retrieved
        for (const auto& it : g3)
        {
            EXPECT_EQ(it.id(), g3.get_vertex_including_inactive(it.id())->id());
        }
    }

    TEST(bipartite_graph, edge_create)
    {
        graph g1(3);
        auto g1v0 = g1.add_vertex();
        auto g1v1 = g1.add_vertex();
        auto g1v2 = g1.add_vertex();

        graph g2(3);
        auto g2v0 = g2.add_vertex();
        auto g2v1 = g2.add_vertex();
        auto g2v2 = g2.add_vertex();

        bipartite_graph g3 { g1, g2 };

        auto left = g3.get_left();
        auto right = g3.get_right();
        g3.add_edge(left[0], right[0]);

        EXPECT_TRUE(g3.has_edge(left[0], right[0]));
    }

}
#pragma clang diagnostic pop