//
// Created by Leon Suchy on 18.11.18.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <graph.hpp>
#include <depth_first_search.hpp>

namespace
{
    using namespace ::testing;

    TEST(depth_first_search, path)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);

        auto search = dfs(g, v0);
        auto it = search.begin();

        EXPECT_EQ(v0, it->id());
        EXPECT_FALSE(search.end() == it);
        ++it;
        EXPECT_EQ(v1, it->id());
        EXPECT_FALSE(search.end() == it);
        ++it;
        EXPECT_EQ(v2, it->id());
        EXPECT_FALSE(search.end() == it);
        ++it;
        EXPECT_TRUE(search.end() == it);
    }

    TEST(depth_first_search, ranged_for)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);

        int num_iterations = 0;
        for (const vertex& v : dfs(g, v0))
        {
            num_iterations++;
        }

        EXPECT_EQ(3, num_iterations);
    }

    TEST(depth_first_search, order)
    {
        // construct binary tree
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();
        auto v6 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v0, v2);
        g.add_edge(v2, v3);
        g.add_edge(v2, v4);
        g.add_edge(v1, v5);
        g.add_edge(v1, v6);

        std::vector<std::vector<vertex::id_type>> possible_orderings =
        {
            {v0, v2, v3, v4, v1, v5, v6},
            {v0, v2, v3, v4, v1, v6, v5},
            {v0, v2, v4, v3, v1, v5, v6},
            {v0, v2, v4, v3, v1, v6, v5},
            {v0, v1, v5, v6, v2, v4, v3},
            {v0, v1, v5, v6, v2, v3, v4},
            {v0, v1, v6, v5, v2, v4, v3},
            {v0, v1, v6, v5, v2, v3, v4},
        };

        std::vector<vertex::id_type> actual_ordering;

        for (const vertex& v : dfs(g, v0))
        {
            actual_ordering.push_back(v.id());
        }

        ASSERT_EQ(g.size(), actual_ordering.size());

        // check if the ordering is contained in the set of all possible orderings
        bool found = false;
        for (const auto& ordering : possible_orderings)
        {
            bool equal = true;
            for (auto i = 0; i < g.size(); i++)
            {
                if (ordering[i] != actual_ordering[i])
                    equal = false;
            }
            if (equal)
            {
                found = true;
                break;
            }
        }

        ASSERT_TRUE(found);
    }
}

#pragma clang diagnostic pop