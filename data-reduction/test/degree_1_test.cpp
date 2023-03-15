//
// Created by Leon Suchy on 17.11.18.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <data_reduction_factory.hpp>
#include <util.hpp>

namespace
{
    using namespace ::testing;

    class data_reduction_degree_1 : public Test
    {
    protected:

        static const data_reduction_options config;
    };

    const data_reduction_options data_reduction_degree_1::config { "exhaustive", 0, {{ "degree-1", 0, { } }}};

    TEST_F(data_reduction_degree_1, one_degree_one_vertex_reject)
    {
        graph g;
        auto removable = g.add_vertex();
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v0);
        g.add_edge(v1, v3);

        g.add_edge(removable, v0);

        graph_meta_data meta_data(g, { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory { config };
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(3, g.size());

        // v0 was taken into the VC
        EXPECT_TRUE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[removable]));

        reductions->reject();

        EXPECT_EQ(5, g.size());

        // v0 was taken out of the VC
        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[removable]));
    }

    TEST_F(data_reduction_degree_1, one_degree_one_vertex_accpet)
    {
        graph g;
        auto removable = g.add_vertex();
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v0);
        g.add_edge(v1, v3);

        g.add_edge(removable, v0);

        graph_meta_data meta_data(g, { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory { config };
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(3, g.size());

        // v0 was taken into the VC
        EXPECT_TRUE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[removable]));

        reductions->accept();

        // nothing has to change to be in accepting state
        EXPECT_EQ(5, g.size());

        // v0 is still in the VC
        EXPECT_TRUE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[removable]));
    }

    TEST_F(data_reduction_degree_1, no_deg_1_accept)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v0);
        g.add_edge(v1, v3);

        graph_meta_data meta_data(g, { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory { config };
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(4, g.size());
        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));

        reductions->accept();

        EXPECT_EQ(4, g.size());
        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));
    }

    TEST_F(data_reduction_degree_1, no_deg_1_reject)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v0);
        g.add_edge(v1, v3);

        graph_meta_data meta_data(g, { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory { config };
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(4, g.size());
        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));

        reductions->reject();

        EXPECT_EQ(4, g.size());
        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));
    }

    TEST_F(data_reduction_degree_1, multiple_deg_1_accept)
    {
        graph g;
        auto removable1 = g.add_vertex();
        auto removable2 = g.add_vertex();
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v0);
        g.add_edge(v1, v3);
        g.add_edge(v4, v2);
        g.add_edge(v4, v3);

        g.add_edge(removable1, v0);
        g.add_edge(removable2, v1);

        graph_meta_data meta_data(g, { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory { config };
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(3, g.size());

        // v0 was taken into the VC
        EXPECT_TRUE(util::is_in_vertex_cover(g[v0]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v4]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[removable1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[removable2]));

        reductions->accept();

        // nothing has to change to be in accepting state
        EXPECT_EQ(7, g.size());

        // v0 is still in the VC
        EXPECT_TRUE(util::is_in_vertex_cover(g[v0]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v4]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[removable1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[removable2]));
    }

    TEST_F(data_reduction_degree_1, multiple_deg_1_reject)
    {
        graph g;
        auto removable1 = g.add_vertex();
        auto removable2 = g.add_vertex();
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v0);
        g.add_edge(v1, v3);
        g.add_edge(v4, v2);
        g.add_edge(v4, v3);

        g.add_edge(removable1, v0);
        g.add_edge(removable2, v1);

        graph_meta_data meta_data(g, { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);

        graph_unit graph_u(g, meta_data, controller);

        data_reduction_factory factory { config };
        auto reductions = std::move(factory.build(graph_u));

        reductions->apply();

        EXPECT_EQ(3, g.size());

        // v0 was taken into the VC
        EXPECT_TRUE(util::is_in_vertex_cover(g[v0]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v4]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[removable1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[removable2]));

        reductions->reject();

        // nothing has to change to be in accepting state
        EXPECT_EQ(7, g.size());

        // v0 is still in the VC
        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v3]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v4]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[removable1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[removable2]));
    }
}

#pragma clang diagnostic pop