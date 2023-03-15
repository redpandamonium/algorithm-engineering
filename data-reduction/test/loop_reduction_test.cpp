//
// Created by Leon Suchy on 2019-02-09.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <data_reduction_factory.hpp>
#include <loop_reduction.hpp>

namespace
{
    using namespace ::testing;

    TEST(loop_reduction, no_loops)
    {
        graph_data data;
        data.vertices.emplace_back(0);
        data.vertices.emplace_back(1);
        data.vertices.emplace_back(2);

        data.vertices[0].add_edge_to(data.vertices[1]);
        data.vertices[1].add_edge_to(data.vertices[2]);
        data.vertices[2].add_edge_to(data.vertices[0]);
        data.vertices[1].add_edge_to(data.vertices[0]);
        data.vertices[2].add_edge_to(data.vertices[1]);
        data.vertices[0].add_edge_to(data.vertices[2]);

        graph g(std::move(data));
        graph_meta_data meta_data(g);
        meta_data.vertex_cover_size(3);
        graph_controller controller(g, meta_data);
        graph_unit unit(g, meta_data, controller);

        data_reduction_options options { "once", 0, { { "loop", 0, { }, { }, "" } }, { }, "" };
        data_reduction_factory factory(options);

        auto reductions = factory.build(unit);

        EXPECT_EQ(3, g.size());
        EXPECT_EQ(3, meta_data.vertex_cover_size());

        reductions->apply();

        EXPECT_EQ(3, g.size());
        EXPECT_EQ(3, meta_data.vertex_cover_size());

        reductions->reject();

        EXPECT_EQ(3, g.size());
        EXPECT_EQ(3, meta_data.vertex_cover_size());

        reductions->apply();

        EXPECT_EQ(3, g.size());
        EXPECT_EQ(3, meta_data.vertex_cover_size());

        reductions->accept();

        EXPECT_EQ(3, g.size());
        EXPECT_EQ(3, meta_data.vertex_cover_size());
    }

    TEST(loop_redcution, loops)
    {
        graph_data data;
        data.vertices.emplace_back(0);
        data.vertices.emplace_back(1);
        data.vertices.emplace_back(2);

        data.vertices[0].add_edge_to(data.vertices[1]);
        data.vertices[1].add_edge_to(data.vertices[2]);
        data.vertices[2].add_edge_to(data.vertices[0]);
        data.vertices[1].add_edge_to(data.vertices[0]);
        data.vertices[2].add_edge_to(data.vertices[1]);
        data.vertices[0].add_edge_to(data.vertices[2]);
        data.vertices[0].add_edge_to(data.vertices[0]);

        graph g(std::move(data));
        graph_meta_data meta_data(g);
        meta_data.vertex_cover_size(3);
        graph_controller controller(g, meta_data);
        graph_unit unit(g, meta_data, controller);

        data_reduction_options options { "once", 0, { { "loop", 0, { }, { }, "" } }, { }, "" };
        data_reduction_factory factory(options);

        auto reductions = factory.build(unit);

        EXPECT_EQ(3, g.size());
        EXPECT_EQ(3, meta_data.vertex_cover_size());

        reductions->apply();

        EXPECT_EQ(2, g.size());
        EXPECT_EQ(2, meta_data.vertex_cover_size());

        reductions->reject();

        EXPECT_EQ(3, g.size());
        EXPECT_EQ(3, meta_data.vertex_cover_size());

        reductions->apply();

        EXPECT_EQ(2, g.size());
        EXPECT_EQ(2, meta_data.vertex_cover_size());

        reductions->accept();

        EXPECT_EQ(3, g.size());
        EXPECT_EQ(2, meta_data.vertex_cover_size());
    }
}