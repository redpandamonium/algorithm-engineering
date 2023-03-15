//
// Created by Carsten Schubert on 2019-01-24.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <data_reduction_factory.hpp>
#include <util.hpp>

namespace {
    using namespace ::testing;

    class data_reduction_neighborhood_partition : public Test {
    protected:

        static const data_reduction_options config;
    };

    const data_reduction_options data_reduction_neighborhood_partition::config{"exhaustive", 0, {{"neighbor-clique-partition", 0, {}}}};

    TEST_F(data_reduction_neighborhood_partition, standard_accept) {
        graph g;
        auto vertex = g.add_vertex();
        // big clique
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        // small clique
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();

        // edges of base vertex
        g.add_edge(vertex, v1);
        g.add_edge(vertex, v2);
        g.add_edge(vertex, v3);
        g.add_edge(vertex, v4);
        g.add_edge(vertex, v5);

        // big clique
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);

        // small clique
        g.add_edge(v4, v5);

        // edges between cliques
        g.add_edge(v1, v5);
        g.add_edge(v2, v5);

        g.add_edge(v3, v4);

        graph_meta_data meta_data(g, {false, false, false});
        meta_data.vertex_cover_size(4);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory{config};
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(3, g.size());
        EXPECT_FALSE(g[vertex].valid());

        EXPECT_TRUE(!g[v3].valid() || !g[v5].valid());
        EXPECT_FALSE(g[v4].valid());

        // reduction needs to check if bigger clique members are in VC --> obviously doesn't work if 0 of 3 clique vertices got chosen
        controller.include_into_vertex_cover(v1);
        controller.include_into_vertex_cover(v2);


        reductions->accept();

        EXPECT_TRUE(util::is_in_vertex_cover(g[v1]) && util::is_in_vertex_cover(g[v2]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v4]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[vertex]));


        EXPECT_TRUE(!util::is_in_vertex_cover(g[v3]) && !util::is_in_vertex_cover(g[v5]));

        EXPECT_EQ(meta_data.vertex_cover_size(), 0);

    }

    TEST_F(data_reduction_neighborhood_partition, weird_accept) {
        graph g;
        auto vertex = g.add_vertex();
        // big clique
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        // small clique
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();

        // edges of base vertex
        g.add_edge(vertex, v1);
        g.add_edge(vertex, v2);
        g.add_edge(vertex, v3);
        g.add_edge(vertex, v4);
        g.add_edge(vertex, v5);

        // big clique
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);

        // small clique
        g.add_edge(v4, v5);

        // edges between cliques
        g.add_edge(v1, v5);
        g.add_edge(v2, v5);

        g.add_edge(v3, v4);

        graph_meta_data meta_data(g, {false, false, false});
        meta_data.vertex_cover_size(4);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory{config};
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(3, g.size());
        EXPECT_FALSE(g[vertex].valid());

        EXPECT_TRUE(!g[v3].valid() || !g[v5].valid());
        EXPECT_FALSE(g[v4].valid());

        // reduction needs to check if bigger clique members are in VC --> obviously doesn't work if 0 of 3 clique vertices got chosen
        controller.include_into_vertex_cover(v1);
        controller.include_into_vertex_cover(v5);

        reductions->accept();

        EXPECT_TRUE(util::is_in_vertex_cover(g[v1]) && util::is_in_vertex_cover(g[v5]));

        EXPECT_TRUE(!util::is_in_vertex_cover(g[v4]));
        EXPECT_TRUE(!util::is_in_vertex_cover(g[v2]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v3]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[vertex]));

        for (auto v : g){
            EXPECT_FALSE(util::is_marked(v, 0x2));
        }

        EXPECT_EQ(meta_data.vertex_cover_size(), 0);

    }


TEST_F(data_reduction_neighborhood_partition, reject) {
        graph g;
        auto vertex = g.add_vertex();
        // big clique
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        // small clique
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();

        // neighbors of v4
        auto v4_n1 = g.add_vertex();
        auto v4_n2 = g.add_vertex();
        g.add_edge(v4_n1, v4);
        g.add_edge(v4_n2, v4);

        // neighbors of v1
        auto v1_n1 = g.add_vertex();
        auto v1_n2 = g.add_vertex();
        g.add_edge(v1_n1, v1);
        g.add_edge(v1_n2, v1);

        // edges of base vertex
        g.add_edge(vertex, v1);
        g.add_edge(vertex, v2);
        g.add_edge(vertex, v3);
        g.add_edge(vertex, v4);
        g.add_edge(vertex, v5);

        // big clique
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);

        // small clique
        g.add_edge(v4, v5);

        // edges between cliques
        g.add_edge(v1, v5);
        g.add_edge(v2, v5);

        g.add_edge(v3, v4);

        graph_meta_data meta_data(g, {false, false, false});
        meta_data.vertex_cover_size(4);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory{config};
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_FALSE(g[vertex].valid());

        EXPECT_TRUE(!g[v3].valid() || !g[v5].valid());
        EXPECT_FALSE(g[v4].valid());

        EXPECT_TRUE(g[v1].get_incident_edge_to(g[v1_n1]));
        EXPECT_TRUE(g[v1].get_incident_edge_to(g[v1_n2]));
        EXPECT_TRUE(g[v1].get_incident_edge_to(g[v4_n1]));
        EXPECT_TRUE(g[v1].get_incident_edge_to(g[v4_n2]));

        EXPECT_TRUE(g[v2].get_incident_edge_to(g[v4_n1]));
        EXPECT_TRUE(g[v2].get_incident_edge_to(g[v4_n2]));

        reductions->reject();

        for (auto v : g) {
            EXPECT_FALSE(util::is_in_vertex_cover(v));
            EXPECT_FALSE(util::is_marked(v, 0x2));
            EXPECT_TRUE(v.valid());

        }

        EXPECT_EQ(g[v2].neighbors().size(), 4);
        EXPECT_EQ(g[v1].neighbors().size(), 6);
        EXPECT_EQ(g[v4].neighbors().size(), 5);

        EXPECT_TRUE(g[v1].get_incident_edge_to(g[v1_n1]));
        EXPECT_TRUE(g[v1].get_incident_edge_to(g[v1_n2]));
        EXPECT_FALSE(g[v1].get_incident_edge_to(g[v4_n1]));
        EXPECT_FALSE(g[v1].get_incident_edge_to(g[v4_n2]));

        EXPECT_FALSE(g[v2].get_incident_edge_to(g[v4_n1]));
        EXPECT_FALSE(g[v2].get_incident_edge_to(g[v4_n2]));

        EXPECT_EQ(meta_data.vertex_cover_size(), 4);
    }
}
