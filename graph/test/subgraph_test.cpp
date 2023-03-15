//
// Created by Leon Suchy on 23.11.18.
//

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <graph.hpp>
#include <subgraph.hpp>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma ide diagnostic ignored "performance-inefficient-vector-operation"
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"

namespace
{
    using namespace ::testing;

    TEST(subgraph, vertex_insert)
    {
        graph g { 10 };
        std::vector<vertex::id_type> vertices;

        for (auto i = 0; i < 10; i++)
            vertices.push_back(g.add_vertex());

        subgraph sub { g, { vertices[0], vertices[1], vertices[2] } };
        EXPECT_EQ(3, sub.size());
    }

    TEST(subgraph, empty)
    {
        graph g { 5 };
        for (auto i = 0; i < 5; i++)
            g.add_vertex();

        subgraph sub { g, { } };
        EXPECT_EQ(0, sub.size());
    }

    TEST(subgraph, vertices_not_in_supergraph)
    {
        graph g { 10 };
        std::vector<vertex::id_type> vertices;

        for (auto i = 0; i < 10; i++)
            vertices.push_back(g.add_vertex());

        std::vector<vertex::id_type> not_in_subgraph;

        // select three vertices which are not in the supergraph
        vertex::id_type current = 0;
        while (not_in_subgraph.empty())
        {
            bool contained = std::find(vertices.begin(), vertices.end(), current) != vertices.end();
            if (!contained)
                not_in_subgraph.push_back(current);

            current++;
        }

        EXPECT_THROW(subgraph(g, not_in_subgraph), invalid_graph_operation);
    }

    TEST(subgraph, ignore_invalid)
    {
        graph g { 2 };
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.invalidate(v2);

        subgraph sub(g, { v1, v2 });
        EXPECT_EQ(1, sub.size());
    }

    TEST(subgraph, ignore_invalid_edge)
    {
        graph g { 2 };
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v1, v2);
        g.invalidate(v2);

        subgraph sub(g, { v1, v2 });
        EXPECT_EQ(0, sub.num_edges());
    }

    TEST(subgraph, edges)
    {
        graph g { 2 };
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v1, v2);

        subgraph sub(g, { v1, v2 });
        EXPECT_EQ(1, sub.num_edges());
    }

    TEST(subgraph, color)
    {
        graph g { 1 };
        auto v1 = g.add_vertex();
        g[v1].color(10);

        subgraph sub(g, { v1 });
        auto colored = sub.get_vertices_with_color(10);
        EXPECT_EQ(1, colored.size());
    }

    TEST(subgraph, backmap)
    {
        graph g { 3 };
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.invalidate(v1);

        subgraph sub(g, { v2 });
        for (auto& vertex : sub)
        {
            vertex::id_type backmap = sub.translate_to_super(vertex.id());
            EXPECT_NE(nullptr, g.get_vertex(backmap));
        }
    }

    TEST(subgraph, backmap_non_existent_id)
    {
        graph g { 1 };
        auto v1 = g.add_vertex();

        subgraph sub(g, { v1 });

        // find an ID which is not in the subgraph
        vertex::id_type invalid_id = 0;
        while (sub.get_vertex(invalid_id) != nullptr)
        {
            invalid_id++;
        }

        EXPECT_EQ(vertex::invalid_id, sub.translate_to_super(invalid_id));
    }

    TEST(subgraph, backmap_vertex_only_in_subgraph)
    {
        graph g { 1 };
        auto v1 = g.add_vertex();

        subgraph sub(g, { v1 });
        auto u1 = sub.add_vertex();

        EXPECT_EQ(vertex::invalid_id, sub.translate_to_super(u1));
    }

    TEST(subgraph, mapped_element_moved)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g[v1].color(0);
        g[v2].color(1);
        g[v3].color(2);

        subgraph sub { g, { v1, v2, v3 } };
        sub.remove(sub.get_vertices_with_color(1)[0]);

        EXPECT_EQ(v1, sub.translate_to_super(sub.get_vertices_with_color(0)[0]));
        EXPECT_EQ(v3, sub.translate_to_super(sub.get_vertices_with_color(2)[0]));
    }
}

#pragma clang diagnostic pop