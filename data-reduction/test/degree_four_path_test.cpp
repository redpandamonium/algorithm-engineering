//
// Created by Carsten Schubert on 2019-01-30.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <data_reduction_factory.hpp>
#include <util.hpp>

namespace {
    using namespace ::testing;

    class data_reduction_degree_4_path : public Test {
    protected:

        static const data_reduction_options config;
    };

    const data_reduction_options data_reduction_degree_4_path::config{"exhaustive", 0,
                                                                                 {{"degree-4-path", 0, {}}}};

    TEST_F(data_reduction_degree_4_path, standard_accept) {
        graph g;
        auto vertex = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v1_n = g.add_vertex();
        auto v2_n = g.add_vertex();
        auto v3_n = g.add_vertex();
        auto v4_n = g.add_vertex();


        // edges of base vertex
        g.add_edge(vertex, v1);
        g.add_edge(vertex, v2);
        g.add_edge(vertex, v3);
        g.add_edge(vertex, v4);

        // path
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v4, v3);

        // neighbours
        g.add_edge(v1_n, v1);
        g.add_edge(v2_n, v2);
        g.add_edge(v3_n, v3);
        g.add_edge(v4_n, v4);

        graph_meta_data meta_data(g, {false, false, false});
        meta_data.vertex_cover_size(4);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory{config};
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(g.num_edges(), 14);
        EXPECT_FALSE(g[vertex].valid());

        // simulating solver
        controller.include_into_vertex_cover(v1);
        controller.include_into_vertex_cover(v2);
        controller.include_into_vertex_cover(v3);
        controller.include_into_vertex_cover(v4);

        reductions->accept();

        EXPECT_FALSE(util::is_in_vertex_cover(g[vertex]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v1]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v2]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v3]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v4]));

        EXPECT_EQ(meta_data.vertex_cover_size(), 0);
    }


    TEST_F(data_reduction_degree_4_path, neighbor_clique_accept) {
        graph g;
        auto vertex = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v1_n = g.add_vertex();
        auto v1_n2 = g.add_vertex();
        auto v2_n = g.add_vertex();
        auto v3_n = g.add_vertex();
        auto v4_n = g.add_vertex();


        // edges of base vertex
        g.add_edge(vertex, v1);
        g.add_edge(vertex, v2);
        g.add_edge(vertex, v3);
        g.add_edge(vertex, v4);

        // path
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v4, v3);

        // neighbours
        g.add_edge(v1_n, v1);
        g.add_edge(v1_n2, v1);
        g.add_edge(v2_n, v2);
        g.add_edge(v3_n, v3);
        g.add_edge(v4_n, v4);
        g.add_edge(v1_n, v1_n2);
        g.add_edge(v1_n, v2_n);
        g.add_edge(v1_n, v3_n);
        g.add_edge(v1_n, v4_n);
        g.add_edge(v1_n2, v2_n);
        g.add_edge(v1_n2, v3_n);
        g.add_edge(v1_n2, v4_n);
        g.add_edge(v2_n, v3_n);
        g.add_edge(v2_n, v4_n);
        g.add_edge(v3_n, v4_n);

        graph_meta_data meta_data(g, {false, false, false});
        meta_data.vertex_cover_size(1 + 2 + 4); // vertex, path, clique
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory{config};
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(g.num_edges(), 6 + 10 + 5 + 4 + 2);
        EXPECT_FALSE(g[vertex].valid());

        // simulating solver
        controller.include_into_vertex_cover(v1);
        controller.include_into_vertex_cover(v2);
        controller.include_into_vertex_cover(v3);
        controller.include_into_vertex_cover(v1_n);
        controller.include_into_vertex_cover(v1_n2);
        controller.include_into_vertex_cover(v2_n);
        controller.include_into_vertex_cover(v4_n);

        reductions->accept();

        EXPECT_TRUE(util::is_in_vertex_cover(g[vertex]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v2]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v3]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v4]));

        EXPECT_EQ(meta_data.vertex_cover_size(), 0);
    }

    TEST_F(data_reduction_degree_4_path, standard_reject) {
        graph g;
        auto vertex = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v1_n = g.add_vertex();
        auto v2_n = g.add_vertex();
        auto v3_n = g.add_vertex();
        auto v4_n = g.add_vertex();


        // edges of base vertex
        g.add_edge(vertex, v1);
        g.add_edge(vertex, v2);
        g.add_edge(vertex, v3);
        g.add_edge(vertex, v4);

        // path
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v4, v3);

        // neighbours
        g.add_edge(v1_n, v1);
        g.add_edge(v2_n, v2);
        g.add_edge(v3_n, v3);
        g.add_edge(v4_n, v4);

        graph_meta_data meta_data(g, {false, false, false});
        meta_data.vertex_cover_size(4);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory{config};
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(g.num_edges(), 14);
        EXPECT_FALSE(g[vertex].valid());

        // simulating solver
        controller.include_into_vertex_cover(v1);
        controller.include_into_vertex_cover(v2);
        controller.include_into_vertex_cover(v3);
        controller.include_into_vertex_cover(v4);

        reductions->reject();
        EXPECT_TRUE(g[vertex].valid());

        EXPECT_FALSE(util::is_in_vertex_cover(g[vertex]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v1]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v2]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v3]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v4]));

        controller.exclude_from_vertex_cover(v1);
        controller.exclude_from_vertex_cover(v2);
        controller.exclude_from_vertex_cover(v3);
        controller.exclude_from_vertex_cover(v4);

        EXPECT_EQ(g.num_edges(), 4+3+4);
        EXPECT_EQ(g.size(), 9);
        EXPECT_EQ(meta_data.vertex_cover_size(), 4);
    }

}