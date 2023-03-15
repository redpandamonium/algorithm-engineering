//
// Created by Leon Suchy on 17.11.18.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>
#include <data_reduction_factory.hpp>
#include <data_reduction_options.hpp>

namespace
{
    using json = nlohmann::json;
    using namespace ::testing;

    class data_reduction_degree_0 : public Test
    {
    protected:

        static const data_reduction_options options;
    };

    const data_reduction_options data_reduction_degree_0::options
    { "exhaustive", 0, {{
       std::string("degree-0"),
       0,
       }}
    };

    /*
     * The graph consists of one isolated vertex of degree 0.
     * The degree 0 rule should delete it.
     * When accepting it is not necessary to put the vertex back.
     * When rejecting it should be put back.
     */
    TEST_F(data_reduction_degree_0, one_deg_0_vertex_accept)
    {
        graph g;
        g.add_vertex();

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory { options };
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_TRUE(g.empty());

        reductions->accept();

        EXPECT_EQ(1, g.size());
    }

    TEST_F(data_reduction_degree_0, one_deg_0_vertex_reject)
    {
        graph g;
        g.add_vertex();

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory { options };
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_TRUE(g.empty());

        reductions->reject();

        EXPECT_EQ(1, g.size());
    }

    TEST_F(data_reduction_degree_0, no_deg_0_vertex_accept)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory { options };
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(2, g.size());

        reductions->accept();

        EXPECT_EQ(2, g.size());
    }

    TEST_F(data_reduction_degree_0, no_deg_0_vertex_reject)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory { options };
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(2, g.size());

        reductions->reject();

        EXPECT_EQ(2, g.size());
    }

    TEST_F(data_reduction_degree_0, multiple_deg_0_reject)
    {
        graph g;
        g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        g.add_vertex();
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory { options };
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(2, g.size());

        reductions->reject();

        EXPECT_EQ(4, g.size());
    }

    TEST_F(data_reduction_degree_0, deg0_and_higher_degree)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v2);

        graph_meta_data meta(g, graph_meta_data::options { false, false, false });
        graph_controller controller(g, meta);
        graph_unit unit(g, meta, controller);
        data_reduction_factory factory(options);

        auto reductions = std::move(factory.build(unit));

        reductions->apply();

        EXPECT_EQ(2, g.size());
        EXPECT_TRUE(g[v0].valid());
        EXPECT_TRUE(g[v2].valid());
        EXPECT_FALSE(g[v1].valid());

        reductions->reject();

        EXPECT_EQ(3, g.size());
    }

    TEST_F(data_reduction_degree_0, no_metadata)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v2);

        graph_meta_data meta(g, graph_meta_data::options { false, false, false });
        graph_controller controller(g, meta);
        graph_unit unit(g, meta, controller);
        data_reduction_factory factory(options);

        auto reductions = std::move(factory.build(unit));

        reductions->apply();

        EXPECT_EQ(2, g.size());
        EXPECT_TRUE(g[v0].valid());
        EXPECT_TRUE(g[v2].valid());
        EXPECT_FALSE(g[v1].valid());

        reductions->reject();

        EXPECT_EQ(3, g.size());

        std::tuple<int, int> p(1,2);
        int first = std::get<0>(p);
    }
}

#pragma clang diagnostic pop