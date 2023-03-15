#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by Leon Suchy on 22.10.18.
//

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <vertex.hpp>

namespace
{
    using namespace ::testing;

    std::vector<vertex> generate_vertices(std::size_t n)
    {
        std::vector<vertex> res { n };
        unsigned current = 0;
        std::generate(res.begin(), res.end(), [&current] () { return vertex { current++ }; });
        return std::move(res);
    }

    TEST(vertex_test, degree_simple)
    {
        vertex v0(0);
        vertex v1(1);
        vertex v2(2);

        v0.add_edge_to(v1);
        v0.add_edge_to(v2);

        EXPECT_EQ(2, v0.degree());
    }

    TEST(vertex_test, degree_with_invalid)
    {
        vertex v0 {0};
        vertex v1 {1};
        vertex v2 {2};

        v0.add_edge_to(v1);
        v0.add_edge_to(v2);
        v1.add_edge_to(v0);
        v2.add_edge_to(v0);

        v1.invalidate();
        v0.invalidate(v1);

        EXPECT_EQ(1, v0.degree());
    }

    TEST(vertex_test, valid_on_construct)
    {
        vertex v0 {0};
        EXPECT_TRUE(v0.valid());
    }

    TEST(vertex_test, neighbors_without_invalid)
    {
        vertex v0 {0};
        vertex v1 {1};
        vertex v2 {2};

        v0.add_edge_to(v1);
        v0.add_edge_to(v2);
        v1.add_edge_to(v0);
        v2.add_edge_to(v0);

        auto neighbors = v0.neighbors();
        EXPECT_EQ(2, neighbors.size());

        EXPECT_THAT(neighbors, Contains(v1.id()));
        EXPECT_THAT(neighbors, Contains(v2.id()));
    }

    TEST(vertex_test, neighbors_with_invalid)
    {
        vertex v0 {0};
        vertex v1 {1};
        vertex v2 {2};

        v0.add_edge_to(v1);
        v0.add_edge_to(v2);
        v1.add_edge_to(v0);
        v2.add_edge_to(v0);

        v2.invalidate();
        v0.invalidate(v2);

        auto neighbors = v0.neighbors();
        EXPECT_EQ(1, neighbors.size());

        EXPECT_THAT(neighbors, Contains(v1.id()));
        EXPECT_THAT(neighbors, Not(Contains(v2.id())));
    }

    // This test asserts the invalidate behaviour on two vertices
    // It is expected that when a vertex is invalidated its neighbors do not change state
    // This means its neighbors are expected to be in an invalid state until updated to the change
    TEST(vertex_test, neighbors_force_invalid)
    {
        vertex v0 {0};
        vertex v1 {1};
        vertex v2 {2};

        v0.add_edge_to(v1);
        v0.add_edge_to(v2);
        v1.add_edge_to(v0);
        v2.add_edge_to(v0);

        v2.invalidate();

        auto neighbors = v0.neighbors_including_inactive();
        EXPECT_EQ(2, neighbors.size());

        EXPECT_THAT(neighbors, Contains(v1.id()));
        EXPECT_THAT(neighbors, Contains(v2.id()));
    }

    TEST(vertex_test, group)
    {
        std::vector<vertex> vertices(generate_vertices(4));
        std::vector<vertex::id_type> ids { 4 };
        for (auto& elem : vertices)
            ids.push_back(elem.id());

        vertex group { 4, ids };

        EXPECT_TRUE(group.is_group());
        EXPECT_THAT(group.children(), UnorderedElementsAreArray(ids));

        group.degree();
    }

    TEST(vertex_test, add_edge)
    {
        vertex v0 {0};
        vertex v1 {1};

        v0.add_edge_to(v1);

        EXPECT_THAT(v0.neighbors(), Contains(v1.id()));
        EXPECT_THAT(v1.neighbors(), Not(Contains(v0.id())));
    }

    TEST(vertex_test, remove_edge)
    {
        vertex v0 {0};
        vertex v1 {1};

        v0.add_edge_to(v1);
        v1.add_edge_to(v0);

        v0.remove_edge_to(v1);

        EXPECT_THAT(v0.neighbors(), Not(Contains(v1.id())));
        EXPECT_THAT(v1.neighbors(), Contains(v0.id()));
    }

    TEST(vertex_test, default_ctor)
    {
        vertex v0;
        EXPECT_FALSE(v0.valid());
    }

    void print(const std::vector<vertex*>& vertices)
    {
        std::cout << "{ ";
        for (auto vertex : vertices)
        {
            std::cout << *vertex << " ";
        }
        std::cout << "}\n";
    }

    TEST(vertex_test, invalidate)
    {
        vertex v { 0 };
        EXPECT_TRUE(v.invalidate());
        EXPECT_FALSE(v.valid());
    }

    TEST(vertex_test, revalidate)
    {
        vertex v { 0 };
        v.invalidate();
        EXPECT_TRUE(v.revalidate());
        EXPECT_TRUE(v.valid());
    }

    TEST(vertex_test, invalidate_wrong)
    {
        vertex v { 0 };
        v.invalidate();
        EXPECT_FALSE(v.invalidate());
        EXPECT_FALSE(v.valid());
    }

    TEST(vertex_test, revalidate_wrong)
    {
        vertex v { 0 };
        EXPECT_FALSE(v.revalidate());
        EXPECT_TRUE(v.valid());
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

    TEST(vertex_test, invalidate_neighbor)
    {
        const int num_vertices = 10;

        std::vector<vertex> vertices { generate_vertices(num_vertices) };
        // 10 vertices. all connected to the first
        for (auto i = 1; i < vertices.size(); i++)
        {
            vertices[i].add_edge_to(vertices.front());
            vertices.front().add_edge_to(vertices[i]);
        }

        // invalidate each subset of v0's neighbors and see if the data is still consistent
        auto subsets = generate_subsets(vertices, 1);
        for (auto& subset : subsets)
        {
            if (subset.empty())
                continue;

            for (auto vertex : subset)
            {
                vertex->invalidate();
                vertices.front().invalidate(*vertex);
            }

            // verify that all invalid edges are removed and all valid edges are still there
            for (auto neighbor : vertices.front().neighbors_including_inactive())
            {
                if (vertices[neighbor].valid())
                    EXPECT_THAT(vertices.front().neighbors(), Contains(neighbor));
                else
                    EXPECT_THAT(vertices.front().neighbors(), Not(Contains(neighbor)));
            }
        }
    }

    TEST(vertex_test, revalidate_neighbor)
    {
        const int num_vertices = 10;

        std::vector<vertex> vertices { generate_vertices(num_vertices) };
        // 10 vertices. all connected to the first
        for (auto i = 1; i < vertices.size(); i++)
        {
            vertices[i].add_edge_to(vertices.front());
            vertices.front().add_edge_to(vertices[i]);
        }

        // invalidate each subset of v0's neighbors and see if the data is still consistent
        auto subsets = generate_subsets(vertices, 1);
        for (auto& subset : subsets)
        {
            if (subset.empty())
                continue;

            for (auto vertex : subset)
            {
                vertex->invalidate();
                vertices.front().invalidate(*vertex);
            }

            for (auto vertex : subset)
            {
                vertex->revalidate();
                vertices.front().revalidate(*vertex);
            }
        }

        // vertices have been restored
        for (const auto& vertex : vertices)
        {
            EXPECT_TRUE(vertex.valid());
        }
        for (auto i = 1; i < vertices.size(); ++i)
        {
            EXPECT_THAT(vertices.front().neighbors(), Contains(vertices[i].id()));
        }
    }
}
#pragma clang diagnostic pop