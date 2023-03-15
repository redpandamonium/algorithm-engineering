//
// Created by synis on 1/26/19.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <memory>
#include <utility>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <data_reduction_factory.hpp>
#include <util.hpp>
#include <lp_reduction.hpp>
#include <nlohmann/json.hpp>

namespace
{
    using json = nlohmann::json;
    using namespace ::testing;

    class data_reduction_lp : public Test
    {
    protected:

        static const data_reduction_options options;
    };

    const data_reduction_options data_reduction_lp::options
            { "once", 0, {{std::string("lp"),0,}}};

    TEST_F(data_reduction_lp, no_solution)
    {
        graph g{3};

        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v1);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, true });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        graph_unit unit(g, meta_data, controller);

        data_reduction_factory factory { options };
        auto reductions = std::move(factory.build(unit));

        reductions->apply();

        EXPECT_TRUE(g.size() == 3); // did not reduce graph;
    }

    TEST_F(data_reduction_lp, find_optimal_solution)
    {
        graph g{5};

        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v0, v2);
        g.add_edge(v1, v3);
        g.add_edge(v1, v4);
        g.add_edge(v2, v3);
        g.add_edge(v2, v4);

        auto edges = g.num_edges();

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, true });
        meta_data.vertex_cover_size(2); // this is true, example graph by me
        graph_controller controller(g, meta_data);
        graph_unit unit(g, meta_data, controller);

        data_reduction_factory factory { options };
        auto reductions = std::move(factory.build(unit));

        reductions->apply();

        EXPECT_TRUE(g.empty());
        EXPECT_TRUE(g.num_edges() == 0);
        EXPECT_TRUE(meta_data.vertex_cover_size() == 0);
        EXPECT_TRUE(util::is_in_vertex_cover(g, v1));
        EXPECT_TRUE(util::is_in_vertex_cover(g, v2));

        reductions->reject();

        EXPECT_TRUE(g.num_edges() == edges);
        EXPECT_FALSE(util::is_in_vertex_cover(g, v1));
        EXPECT_FALSE(util::is_in_vertex_cover(g, v2));
        for (auto& vertex : g)
            EXPECT_TRUE(vertex.valid());
    }

    TEST_F(data_reduction_lp, rocket_graph) {

        graph g{7};

        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();
        auto v6 = g.add_vertex();

        g.add_edge(v0, v2);
        g.add_edge(v0, v3);
        g.add_edge(v1, v2);
        g.add_edge(v1, v3);
        g.add_edge(v2, v4);
        g.add_edge(v2, v5);
        g.add_edge(v3, v4);
        g.add_edge(v3, v5);
        g.add_edge(v4, v5);
        g.add_edge(v4, v6);
        g.add_edge(v5, v6);

        auto edges = g.num_edges();

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, true });
        meta_data.vertex_cover_size(2); // this is true, example graph by me
        graph_controller controller(g, meta_data);
        graph_unit unit(g, meta_data, controller);

        data_reduction_factory factory { options };
        auto reductions = std::move(factory.build(unit));

        reductions->apply();

        EXPECT_TRUE(g.num_edges() == 3);
        EXPECT_TRUE(meta_data.vertex_cover_size() == 0);
        EXPECT_TRUE(util::is_in_vertex_cover(g, v2));
        EXPECT_TRUE(util::is_in_vertex_cover(g, v3));
    }
}