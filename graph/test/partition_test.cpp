//
// Created by Leon Suchy on 23.11.18.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma ide diagnostic ignored "performance-inefficient-vector-operation"
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>

#include <graph.hpp>
#include <components.hpp>

namespace
{
    using namespace ::testing;

    TEST(components, singleton)
    {
        graph g;
        g.add_vertex();

        auto comps = std::move(components::subgraphs(g));
        ASSERT_EQ(1, comps.size());

        std::unique_ptr<subgraph>& s = comps[0];
        EXPECT_EQ(&s->supergraph(), &g); // supergraph address matches
        EXPECT_EQ(1, s->size());
    }

    TEST(components, empty)
    {
        graph g;
        auto comps = std::move(components::subgraphs(g));
        EXPECT_EQ(0, comps.size());
    }

    TEST(components, one)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v1, v2);

        auto comps = std::move(components::subgraphs(g));
        ASSERT_EQ(1, comps.size());

        std::unique_ptr<subgraph>& s = comps[0];
        EXPECT_EQ(&s->supergraph(), &g);
        EXPECT_EQ(2, s->size());
    }

    TEST(components, multiple)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();
        auto v6 = g.add_vertex();

        // two triangles
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);

        g.add_edge(v4, v5);
        g.add_edge(v5, v6);
        g.add_edge(v6, v4);

        auto comps = std::move(components::subgraphs(g));
        ASSERT_EQ(2, comps.size());

        EXPECT_EQ(3, comps[0]->size());
        EXPECT_EQ(3, comps[1]->size());
    }

    TEST(components, color)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();
        auto v6 = g.add_vertex();

        // two triangles
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);

        g.add_edge(v4, v5);
        g.add_edge(v5, v6);
        g.add_edge(v6, v4);

        auto comps = std::move(components::subgraphs(g));

        EXPECT_EQ(6, g.size());
        for (const vertex& v : g)
        {
            EXPECT_EQ(0, v.color());
        }
    }
}

#pragma clang diagnostic pop