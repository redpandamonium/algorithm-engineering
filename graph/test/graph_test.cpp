#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

//
// Created by Leon Suchy on 22.10.18.
//

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <graph.hpp>

namespace
{
    using namespace ::testing;

    TEST(graph_test, add_vertex)
    {
        graph g { };
        g.add_vertex();

        EXPECT_THAT(g.get_vertex(0), NotNull());
    }

    TEST(graph_test, get_invalidated_vertex)
    {
        graph g { };
        auto v = g.add_vertex();
        g.invalidate(v);

        EXPECT_THAT(g.get_vertex(v), IsNull());
        EXPECT_THAT(g.get_vertex_including_inactive(v), NotNull());
    }

    TEST(graph_test, size)
    {
        graph g { };

        EXPECT_EQ(0, g.size());
        EXPECT_TRUE(g.empty());

        auto v = g.add_vertex();

        EXPECT_EQ(1, g.size());
        EXPECT_FALSE(g.empty());

        g.invalidate(v);

        EXPECT_EQ(0, g.size());
        EXPECT_TRUE(g.empty());
    }

    TEST(graph_test, add_edge)
    {
        graph g { 2 };

        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v1, v2);

        EXPECT_THAT(g[v1].neighbors(), Contains(v2));
        EXPECT_THAT(g[v2].neighbors(), Contains(v1));
    }

    TEST(graph_test, remove_edge)
    {
        graph g { 2 };
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v1, v2);
        g.remove_edge(v1, v2);

        EXPECT_THAT(g[v1].neighbors(), Not(Contains(v2)));
        EXPECT_THAT(g[v2].neighbors(), Not(Contains(v1)));
    }

    TEST(graph_test, loop_free)
    {
        graph g { 2 };
        auto v1 = g.add_vertex();
        
        EXPECT_THROW(g.add_edge(v1, v1), invalid_graph_operation);
    }

    TEST(graph_test, remove_contingency)
    {
        graph g { 2 };
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.invalidate(v1);

        // second vertex has not moved nor changed ID
        EXPECT_THAT(g.get_vertex(v2), NotNull());
    }

    TEST(graph_test, group)
    {
        graph g { 4 };
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);

        auto group = g.group({ v1, v2, v3 });

        // the graph only has one vertex now
        EXPECT_EQ(1, g.size());

        // the vertex was marked correctly
        EXPECT_TRUE(g[group].is_group());

        // all its children were deactivated
        EXPECT_FALSE(g[v1].valid());
        EXPECT_FALSE(g[v2].valid());
        EXPECT_FALSE(g[v3].valid());

        // the group vertices can be obtained
        EXPECT_THAT(g[group].children(), Contains(v1));
        EXPECT_THAT(g[group].children(), Contains(v2));
        EXPECT_THAT(g[group].children(), Contains(v3));

        // no neighbors whatsoever
        EXPECT_EQ(0, g[group].degree());
        EXPECT_EQ(0, g[group].neighbors_including_inactive().size());
    }

    TEST(graph_test, group_with_outside_edges)
    {
        graph g { 10 };
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();

        // edges within the group
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);

        // edges outside the group
        g.add_edge(v1, v4);
        g.add_edge(v3, v5);

        vertex::id_type group = g.group({ v1, v2, v3 });

        // graph size reduced
        EXPECT_EQ(3, g.size());

        // the vertex was marked correctly
        EXPECT_TRUE(g[group].is_group());
        EXPECT_TRUE(g[group].valid());

        // all its children were deactivated
        EXPECT_FALSE(g[v1].valid());
        EXPECT_FALSE(g[v2].valid());
        EXPECT_FALSE(g[v3].valid());

        // the group vertices can be obtained
        EXPECT_THAT(g[group].children(), Contains(v1));
        EXPECT_THAT(g[group].children(), Contains(v2));
        EXPECT_THAT(g[group].children(), Contains(v3));

        // edges to the outside were preserved
        EXPECT_EQ(2, g[group].degree());
        EXPECT_THAT(g[group].neighbors(), Contains(v4));
        EXPECT_THAT(g[group].neighbors(), Contains(v5));
    }
    
    TEST(graph_test, ungroup)
    {
        graph g { 5 };
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);
        g.add_edge(v1, v4);

        auto group = g.group({ v1, v2, v3 });
        const auto& retrieved_vertices = g.ungroup(group);

        // graph size is back
        EXPECT_EQ(4, g.size());

        // v4 has its original neighbor back
        EXPECT_THAT(g[v4].neighbors(), Not(Contains(group)));

        // group is invalid, members are valid
        EXPECT_FALSE(g[group].valid());
        EXPECT_TRUE(g[v1].valid());
        EXPECT_TRUE(g[v2].valid());
        EXPECT_TRUE(g[v3].valid());
        
        // verify group member retrieval from return statement
        EXPECT_EQ(3, retrieved_vertices.size());
        EXPECT_THAT(retrieved_vertices, Contains(v1));
        EXPECT_THAT(retrieved_vertices, Contains(v2));
        EXPECT_THAT(retrieved_vertices, Contains(v3));
    }

    unsigned long ipow(unsigned long base, unsigned exponent) {
        unsigned long res = 1;

        while (exponent) {
            if(exponent & 1)
                res *= base;
            exponent >>= 1;
            base *= base;
        }

        return res;
    }

    std::vector<std::vector<vertex*>> generate_subsets(std::vector<vertex>& set, int start_index)
    {
        size_t size = set.size() - start_index;
        std::vector<std::vector<vertex*>> subsets { ipow(2, static_cast<unsigned int>(size - 1)) };

        unsigned bitset = 0x1;
        unsigned done_bits = static_cast<unsigned>(0x1) << set.size(); // counted 1 past the maximum valid index
        while ((bitset & done_bits) != done_bits)
        {
            std::vector<vertex*> subset;
            for (auto i = start_index; i < set.size(); i++)
            {
                bool contains_index = (bitset & (0x1 << i)) == (0x1 << i);
                if (contains_index)
                    subset.push_back(&set[i]);
            }

            subsets.push_back(std::move(subset));
            bitset++;
        }

        return std::move(subsets);
    }

    TEST(graph_test, remove_vertex)
    {
        // Test that if we delete a vertex all its neighbors know of it
        graph g { 5 };
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v1, v2);

        g.invalidate(v1);

        EXPECT_TRUE(g[v2].neighbors().empty());
    }

    TEST(graph_test, has_edge)
    {
        graph g {5};

        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        for (auto& vertex : g)
        {
            if (vertex.id() != v1)
            {
                std::cout << vertex << v1 << "\n";
                g.add_edge(v1, vertex.id());
            }
        }

        for (auto& vertex :g)
        {
            if (vertex.id() != v1)
            {
                EXPECT_TRUE(g.has_edge(vertex.id(), v1));
            }
        }

        EXPECT_FALSE(g.has_edge(v3, v4));
        g.add_edge(v3, v4);
        EXPECT_TRUE(g.has_edge(v4, v3));
    }

    TEST(graph_test, two_invalid_neighbors_delete_first)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v0, v2);

        g.invalidate(v1);
        g.invalidate(v2);

        g.remove_edge(v1, v0);
        g.remove_edge(v0, v1);

        EXPECT_THAT(g.num_edges(), 0);
    }
}

#pragma clang diagnostic pop