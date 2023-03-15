#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by Leon Suchy on 27.10.18.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <graph.hpp>

namespace
{
    using namespace ::testing;

    TEST(graph_iterator, test_no_invalid)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        auto it = g.begin();
        EXPECT_EQ(v1, it->id());
        ++it;
        EXPECT_EQ(v2, it->id());
        ++it;
        EXPECT_TRUE(g.end() == it);
    }

    TEST(const_graph_iterator, test_no_invalid)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        auto it = g.cbegin();
        EXPECT_EQ(v1, it->id());
        ++it;
        EXPECT_EQ(v2, it->id());
        ++it;
        EXPECT_TRUE(g.cend() == it);
    }

    TEST(graph_iterator, test_with_invalid)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.invalidate(v2);
        g.invalidate(v4);

        auto it = g.begin();
        EXPECT_EQ(v1, it->id());
        ++it;
        EXPECT_EQ(v3, it->id());
        ++it;
        EXPECT_TRUE(g.end() == it);
    }

    TEST(const_graph_iterator, test_with_invalid)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.invalidate(v2);
        g.invalidate(v4);

        auto it = g.cbegin();
        EXPECT_EQ(v1, it->id());
        ++it;
        EXPECT_EQ(v3, it->id());
        ++it;
        EXPECT_TRUE(g.cend() == it);
    }

    TEST(graph_iterator, iterations_match_size)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();

        g.invalidate(v2);
        g.invalidate(v5);

        unsigned num = 0;
        for (graph_iterator it = g.begin(); it != g.end(); ++it)
        {
            num++;
            if (num > g.size())
                break;
        }

        EXPECT_EQ(g.size(), num);
    }

    TEST(const_graph_iterator, iterations_match_size)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();

        g.invalidate(v2);
        g.invalidate(v5);

        unsigned num = 0;
        for (const_graph_iterator it = g.cbegin(); it != g.cend(); ++it)
        {
            num++;
            if (num > g.size())
                break;
        }

        EXPECT_EQ(g.size(), num);
    }

    TEST(graph_iterator, iteration_terminates_after_insert)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        graph_iterator it = g.begin();
        auto v5 = g.add_vertex();
        ++it;

        unsigned num = 1;
        for (; it != g.end(); ++it)
        {
            num++;
            if (num > g.size())
                break;
        }

        EXPECT_EQ(g.size(), num);
    }

    TEST(const_graph_iterator, iteration_terminates_after_insert)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        const_graph_iterator it = g.cbegin();
        auto v5 = g.add_vertex();
        ++it;

        unsigned num = 1;
        for (; it != g.cend(); ++it)
        {
            num++;
            if (num > g.size())
                break;
        }

        EXPECT_EQ(g.size(), num);
    }

    TEST(graph_iterator, first_element_is_at_index_greater_than_size)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.invalidate(v1);
        g.invalidate(v2);

        bool found = false;
        for (graph_iterator it = g.begin(); it != g.end(); ++it)
        {
            // v3 is the only valid vertex so only it should show up
            EXPECT_EQ(v3, it->id());
            found = true;
        }
        // v3 is a valid vertex, so it should have shown up
        EXPECT_TRUE(found);
    }

    TEST(const_graph_iterator, first_element_is_at_index_greater_than_size)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.invalidate(v1);
        g.invalidate(v2);

        bool found = false;
        for (const_graph_iterator it = g.cbegin(); it != g.cend(); ++it)
        {
            // v3 is the only valid vertex so only it should show up
            EXPECT_EQ(v3, it->id());
            found = true;
        }
        // v3 is a valid vertex, so it should have shown up
        EXPECT_TRUE(found);
    }

    TEST(graph_iterator, foreach_loop)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();

        g.invalidate(v1);
        g.invalidate(v3);
        g.invalidate(v5);

        bool v2_seen = false;
        bool v4_seen = false;
        int num_iterations = 0;

        for (vertex& v : g)
        {
            if (v.id() == v2)
                v2_seen = true;
            if (v.id() == v4)
                v4_seen = true;
            num_iterations++;
        }

        EXPECT_TRUE(v2_seen);
        EXPECT_TRUE(v4_seen);
        EXPECT_EQ(2, num_iterations);
    }

    TEST(const_graph_iterator, foreach_loop)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();

        g.invalidate(v1);
        g.invalidate(v3);
        g.invalidate(v5);

        bool v2_seen = false;
        bool v4_seen = false;
        int num_iterations = 0;

        const graph& g_const = g;

        for (const vertex& v : g_const)
        {
            if (v.id() == v2)
                v2_seen = true;
            if (v.id() == v4)
                v4_seen = true;
            num_iterations++;
        }

        EXPECT_TRUE(v2_seen);
        EXPECT_TRUE(v4_seen);
        EXPECT_EQ(2, num_iterations);
    }
}

#pragma clang diagnostic pop