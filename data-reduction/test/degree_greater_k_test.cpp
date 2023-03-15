//
// Created by Leon Suchy on 17.11.18.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>
#include <data_reduction_factory.hpp>
#include <util.hpp>

namespace
{
    using json = nlohmann::json;
    using namespace ::testing;

    class data_reduction_degree_greater_k : public Test
    {
    protected:

        static void SetUpTestCase()
        {
            config = data_reduction_options { "exhaustive", 0, {{ "degree > k", 0, { } }}};
            factory = data_reduction_factory(config);
        }

        static data_reduction_options config;
        static data_reduction_factory factory;
    };

    data_reduction_options data_reduction_degree_greater_k::config { };
    data_reduction_factory data_reduction_degree_greater_k::factory(data_reduction_options { });

    TEST_F(data_reduction_degree_greater_k, all_below_k)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(2);
        graph_controller controller(g, meta_data);
        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();

        EXPECT_EQ(3, g.size());
    }

    TEST_F(data_reduction_degree_greater_k, one_above_k)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);
        g.add_edge(v1, v4);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(2);
        graph_controller controller(g, meta_data);
        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();

        EXPECT_EQ(3, g.size());
        EXPECT_TRUE(util::is_in_vertex_cover(g[v1]));
    }

    TEST_F(data_reduction_degree_greater_k, reject)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);
        g.add_edge(v1, v4);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(2);
        graph_controller controller(g, meta_data);
        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();
        EXPECT_EQ(3, g.size());
        reductions->reject();

        EXPECT_EQ(4, g.size());
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
    }

    TEST_F(data_reduction_degree_greater_k, accept)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);
        g.add_edge(v1, v4);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(2);
        graph_controller controller(g, meta_data);
        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();
        EXPECT_EQ(3, g.size());
        reductions->accept();
        EXPECT_EQ(4, g.size());
        EXPECT_TRUE(util::is_in_vertex_cover(g[v1]));
    }
}