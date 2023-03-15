//
// Created by Leon Suchy on 13.12.18.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

// googletest
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// graph
#include <graph.hpp>

// vc
#include <graph_meta_data.hpp>
#include <graph_controller.hpp>

namespace
{
    using namespace ::testing;

    TEST(heap, min_heap_order)
    {
        graph g { 5 };
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v0, v2);
        g.add_edge(v0, v3);
        g.add_edge(v2, v3);

        graph_meta_data meta_data(g);

        EXPECT_EQ(v1, meta_data.min_heap().top());
        meta_data.min_heap().pop();

        auto second = meta_data.min_heap().top();
        meta_data.min_heap().pop();
        auto third = meta_data.min_heap().top();
        meta_data.min_heap().pop();
        EXPECT_NE(second, third);
        EXPECT_TRUE(second == v2 || third == v2);
        EXPECT_TRUE(second == v3 || third == v3);

        EXPECT_EQ(v0, meta_data.min_heap().top());
        meta_data.min_heap().pop();

        EXPECT_TRUE(meta_data.min_heap().empty());
    }

    TEST(heap, min_comparator)
    {
        graph g { 5 };
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v0, v2);
        g.add_edge(v0, v3);
        g.add_edge(v2, v3);

        min_comparator comparator(g);
        EXPECT_TRUE(comparator(v0, v1));   // v1 comes before v0
        EXPECT_FALSE(comparator(v1, v0));    // v1 comes before v0
        EXPECT_FALSE(comparator(v2, v3));   // don't swap equal values
    }

    TEST(heap, max_heap_order)
    {
        graph g { 5 };
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v0, v2);
        g.add_edge(v0, v3);
        g.add_edge(v2, v3);

        graph_meta_data meta_data(g);

        EXPECT_EQ(v0, meta_data.max_heap().top());
        meta_data.max_heap().pop();

        auto second = meta_data.max_heap().top();
        meta_data.max_heap().pop();
        auto third = meta_data.max_heap().top();
        meta_data.max_heap().pop();
        EXPECT_NE(second, third);
        EXPECT_TRUE(second == v2 || third == v2);
        EXPECT_TRUE(second == v3 || third == v3);

        EXPECT_EQ(v1, meta_data.max_heap().top());
        meta_data.max_heap().pop();

        EXPECT_TRUE(meta_data.max_heap().empty());
    }

    TEST(heap, max_comparator)
    {
        graph g { 5 };
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v0, v2);
        g.add_edge(v0, v3);
        g.add_edge(v2, v3);

        max_comparator comparator(g);
        EXPECT_TRUE(comparator(v1, v0));   // v1 comes before v0
        EXPECT_FALSE(comparator(v0, v1));    // v1 comes before v0
        EXPECT_FALSE(comparator(v2, v3));   // don't swap equal values
    }

    TEST(heap, increase)
    {
        graph g { 6 };
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v0, v2);
        g.add_edge(v0, v3);
        g.add_edge(v2, v3);

        graph_meta_data meta_data { g };
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();
        g.add_edge(v2, v4);
        g.add_edge(v2, v5);

        meta_data.heap_element_degree_increased(v2);

        g.remove(edge { v0, v1 });
        meta_data.heap_element_degree_decreased(v0);
        meta_data.heap_element_degree_decreased(v1);

        EXPECT_EQ(v2, meta_data.max_heap().top());
        EXPECT_EQ(v1, meta_data.min_heap().top());
    }

    TEST(heap, decrease)
    {
        graph g { 6 };
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v0, v2);
        g.add_edge(v0, v3);
        g.add_edge(v2, v3);

        graph_meta_data meta_data { g };

        g.remove(edge {v0, v3});
        g.remove(edge {v2, v3});

        meta_data.heap_element_degree_increased(v0);
        meta_data.heap_element_degree_increased(v2);
        meta_data.heap_element_degree_increased(v3);

        EXPECT_TRUE(meta_data.max_heap().top() == v2 || meta_data.max_heap().top() == v0);
        EXPECT_EQ(v3, meta_data.min_heap().top());
    }

    TEST(heap, include_into_vertex_cover)
    {
        graph g { 5 };
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        graph_meta_data meta_data { g };
        graph_controller controller { g, meta_data };

        controller.add_edge(v0, v1);
        controller.add_edge(v0, v2);
        controller.add_edge(v1, v2);
        controller.add_edge(v2, v3);

        controller.include_into_vertex_cover(v0);

        EXPECT_EQ(v2, meta_data.max_heap().top());
        EXPECT_TRUE(v3 == meta_data.min_heap().top() || v1 == meta_data.min_heap().top());
    }
}

#pragma clang diagnostic pop