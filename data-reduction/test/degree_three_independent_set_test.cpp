//
// Created by Carsten Schubert on 2019-01-27.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <data_reduction_factory.hpp>
#include <util.hpp>

namespace {
    using namespace ::testing;

    class data_reduction_degree_3_independent_set : public Test {
    protected:

        static const data_reduction_options config;
    };

    const data_reduction_options data_reduction_degree_3_independent_set::config{"exhaustive", 0,
                                                                               {{"degree-3-independent-set", 0, {}}}};

    TEST_F(data_reduction_degree_3_independent_set, standard_accept) {
        graph g;
        // star
        auto vertex = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        // edges of base vertex
        g.add_edge(vertex, v1);
        g.add_edge(vertex, v2);
        g.add_edge(vertex, v3);

        graph_meta_data meta_data(g, {false, false, false});
        meta_data.vertex_cover_size(1);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory{config};
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(g.num_edges(), 2);

        // simulating solver
        controller.include_into_vertex_cover(v2);
        reductions->accept();

        EXPECT_TRUE(util::is_in_vertex_cover(g[vertex]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));

        EXPECT_EQ(meta_data.vertex_cover_size(), 0);
    }


    TEST_F(data_reduction_degree_3_independent_set, cube_accept) {
        graph g;
        // star
        auto vertex = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();
        auto v6 = g.add_vertex();


        // edges of base vertex
        g.add_edge(vertex, v1);
        g.add_edge(vertex, v2);
        g.add_edge(vertex, v3);
        g.add_edge(v4, v1);
        g.add_edge(v4, v3);
        g.add_edge(v5, v3);
        g.add_edge(v5, v2);
        g.add_edge(v6, v2);
        g.add_edge(v6, v1);


        graph_meta_data meta_data(g, {false, false, false});
        meta_data.vertex_cover_size(3);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory{config};
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        // simulating solver
        controller.include_into_vertex_cover(v1);
        controller.include_into_vertex_cover(v2);
        controller.include_into_vertex_cover(v3);

        reductions->accept();

        EXPECT_TRUE(util::is_in_vertex_cover(g[v1]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v2]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v3]));

        EXPECT_EQ(meta_data.vertex_cover_size(), 0);
    }

    TEST_F(data_reduction_degree_3_independent_set, some_graph_accept) {
        graph g;
        auto vertex = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();
        auto v6 = g.add_vertex();
        auto v7 = g.add_vertex();
        auto v8 = g.add_vertex();
        auto v9 = g.add_vertex();

        // edges of base vertex
        g.add_edge(vertex, v1);
        g.add_edge(vertex, v2);
        g.add_edge(vertex, v3);
        g.add_edge(v4, v1);
        g.add_edge(v4, v5);
        g.add_edge(v5, v3);
        g.add_edge(v6, v2);
        g.add_edge(v6, v3);
        g.add_edge(v7, v3);
        g.add_edge(v7, v8);
        g.add_edge(v8, v9);
        g.add_edge(v9, v3);

        graph_meta_data meta_data(g, {false, false, false});
        meta_data.vertex_cover_size(5);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory{config};
        auto reductions = std::move(factory.build(graph_u));

        EXPECT_EQ(g.num_edges(), 12);

        reductions->apply();

        EXPECT_EQ(g.num_edges(), 16);

        // simulating solver
        controller.include_into_vertex_cover(v2);
        controller.include_into_vertex_cover(v3);
        controller.include_into_vertex_cover(v4);
        controller.include_into_vertex_cover(v6);
        controller.include_into_vertex_cover(v8);

        reductions->accept();

        EXPECT_TRUE(util::is_in_vertex_cover(g[vertex]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v3]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v4]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v6]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v8]));

        EXPECT_EQ(meta_data.vertex_cover_size(), 0);

        for (auto v : g.data().vertices)
            controller.revalidate(v.id());
        EXPECT_EQ(g.num_edges(), 12);
    }

    TEST_F(data_reduction_degree_3_independent_set, standard_reject) {
        graph g;
        // star
        auto vertex = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        // edges of base vertex
        g.add_edge(vertex, v1);
        g.add_edge(vertex, v2);
        g.add_edge(vertex, v3);

        graph_meta_data meta_data(g, {false, false, false});
        meta_data.vertex_cover_size(1);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory{config};
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(g.num_edges(), 2);

        // simulating solver
        controller.include_into_vertex_cover(v2);
        reductions->reject();

        EXPECT_TRUE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[vertex]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));

        for (auto v : g)
            EXPECT_TRUE(v.valid());

        EXPECT_EQ(meta_data.vertex_cover_size(), 0);
    }

}