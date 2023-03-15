//
// Created by iarch on 11/10/18.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <bipartite_graph.hpp>
#include <hopcroft_karp_matching.hpp>

namespace
{
    using namespace ::testing;

    graph build_complete_graph(size_t size)
    {
        graph g{size+1};
        for (size_t i = 0; i < size; i++)
        {
            g.add_vertex();
        }

        for (auto& u : g)
        {
            for (auto& v : g)
            {
                if (u == v)
                    continue;
                g.add_edge(u, v);
            }
        }

        return g;
    }

    TEST(hopcroft_karp, triangle)
    {
        graph g(4);

        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0,v1);
        g.add_edge(v1,v2);
        g.add_edge(v2,v0);

        hopcroft_karp_matching m(g);
        m.init();
        auto matching = m.calculate_matching();
        EXPECT_EQ(matching.size(), 3);
    }

    TEST(hopcroft_karp, trivial)
    {
        graph g(4);

        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();

        g.add_edge(v0, v1);

        hopcroft_karp_matching m(g);
        m.init();
        auto matching = m.calculate_matching();
        EXPECT_EQ(matching.size(), 2);
    }

    TEST(hopcroft_karp, complete)
    {
        for(size_t i = 2; i <= 150; i++)
        {
            auto g = build_complete_graph(i);
            hopcroft_karp_matching m(g);
            m.init();
            auto matching = m.calculate_matching();
            EXPECT_EQ(matching.size(), i);
        }
    }

    TEST(hopcroft_karp, non_trivial)
    {
        graph g{10};

        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();

        // almost triangle
        g.add_edge(v0, v1);
        g.add_edge(v2, v1);
        g.add_edge(v1, v3);

        // triangle
        g.add_edge(v3, v4);
        g.add_edge(v3, v5);
        g.add_edge(v4, v5);

        hopcroft_karp_matching m(g);
        m.init();
        auto matching = m.calculate_matching();
        EXPECT_EQ(matching.size(), 5);
    }
}