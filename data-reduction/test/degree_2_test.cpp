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

    class data_reduction_degree_2 : public Test
    {
    protected:

        static void SetUpTestCase()
        {
            config = { "exhaustive", 0, { { "triangle", 1, { } }, { "2-fold", 0, { } } } };
            factory = data_reduction_factory { config };
        }

        static data_reduction_options config;
        static data_reduction_factory factory_2_fold;
        static data_reduction_factory factory;
    };

    data_reduction_options data_reduction_degree_2::config { };
    data_reduction_factory data_reduction_degree_2::factory(data_reduction_options { });
    data_reduction_factory data_reduction_degree_2::factory_2_fold(data_reduction_options { "exhaustive", 0, { { "2-fold", 0, { } } } });

    TEST_F(data_reduction_degree_2, single_triangle_reject)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v0);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);

        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();
        reductions->reject();

        EXPECT_EQ(3, g.size());
        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
    }

    TEST_F(data_reduction_degree_2, single_triangle_accept)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v0);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);

        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();

        EXPECT_TRUE(g.empty());
        {
            int num_in_vc = 0;
            if (util::is_in_vertex_cover(g[v0]))
                num_in_vc++;
            if (util::is_in_vertex_cover(g[v1]))
                num_in_vc++;
            if (util::is_in_vertex_cover(g[v2]))
                num_in_vc++;

            // any two
            EXPECT_EQ(2, num_in_vc);
        }

        reductions->accept();

        EXPECT_EQ(3, g.size());
        {
            int num_in_vc = 0;
            if (util::is_in_vertex_cover(g[v0]))
                num_in_vc++;
            if (util::is_in_vertex_cover(g[v1]))
                num_in_vc++;
            if (util::is_in_vertex_cover(g[v2]))
                num_in_vc++;

            // any two
            EXPECT_EQ(2, num_in_vc);
        }

    }

    TEST_F(data_reduction_degree_2, fold_take_inner)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);

        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();
        reductions->accept();

        EXPECT_TRUE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
    }

    void add_k4(graph& g, vertex::id_type connection)
    {
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, connection);
        g.add_edge(v1, connection);
        g.add_edge(v2, connection);

        g.add_edge(v0, v1);
        g.add_edge(v0, v2);

        g.add_edge(v1, v2);
    }

    TEST_F(data_reduction_degree_2, fold_take_outer)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);

        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();

        EXPECT_EQ(1, g.size()); // all vertices are grouped

        // take group into VC
        controller.include_into_vertex_cover(g.begin()->id());

        reductions->accept();

        EXPECT_EQ(1, g.size()); // ungrouped

        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v0]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v2]));
    }

    TEST_F(data_reduction_degree_2, fold_reject)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);

        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();
        reductions->reject();

        EXPECT_EQ(3, g.size()); // ungrouped

        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
    }

    TEST_F(data_reduction_degree_2, triangle_reject)
    {
        graph g;
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v0);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);

        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();
        reductions->reject();

        EXPECT_EQ(3, g.size()); // ungrouped

        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
    }

    // ----------------------------------------------------------

    TEST_F(data_reduction_degree_2, simple_triangle)
    {
        graph g(5);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        // v0 and v1 connected to all others
        for (auto& vertex : g)
        {
            if (vertex.id() != v0)
            {
                g.add_edge(v0, vertex.id());
            }
        }

        for (auto& vertex : g)
        {
            if (vertex.id() != v0 && vertex.id() != v1)
            {
                g.add_edge(v1, vertex.id());
            }
        }

        g.add_edge(v2, v3);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();

        // v4 taken into VC
        EXPECT_TRUE ((g[v0].color() & vertex::color_true) == vertex::color_true);
        EXPECT_FALSE(g[v0].valid());
        EXPECT_TRUE ((g[v1].color() & vertex::color_true) == vertex::color_true);
        EXPECT_FALSE(g[v1].valid());
        EXPECT_FALSE(g[v4].valid());


        // others aren't
        for (auto& vertex : g)
        {
            if (vertex.id() == v2 || vertex.id() == v3)
            {
                EXPECT_FALSE((vertex.color() & vertex::color_true) == vertex::color_true);
                EXPECT_TRUE(vertex.valid());
            }
        }
    }

    TEST_F(data_reduction_degree_2, compl_fold)
    {
        graph g(5);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        // path of length 5
        g.add_edge(v0, v1);
        g.add_edge(v1, v2);
        g.add_edge(v2, v3);
        g.add_edge(v3, v4);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();
        reductions->accept();

        // none of the others taken
        EXPECT_FALSE(util::is_in_vertex_cover(g, v0));
        EXPECT_TRUE (util::is_in_vertex_cover(g, v1));
        EXPECT_FALSE(util::is_in_vertex_cover(g, v2));
        EXPECT_TRUE (util::is_in_vertex_cover(g, v3));
        EXPECT_FALSE(util::is_in_vertex_cover(g, v4));
    }

    TEST_F(data_reduction_degree_2, simple_fold)
    {
        graph g(3);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        auto reductions = factory.build(graph_unit(g, meta_data, controller));

        reductions->apply();

        EXPECT_EQ(1, g.size());

        reductions->accept();

        // none of the others taken
        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_TRUE (util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
    }

    TEST_F(data_reduction_degree_2, fold_none_initial_not_taken_accept)
    {
        graph g(3);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        auto reductions = factory_2_fold.build(graph_unit(g, meta_data, controller));

        reductions->apply();
        reductions->accept();

        EXPECT_TRUE(g[v0].valid());
        EXPECT_FALSE(g[v1].valid());
        EXPECT_TRUE(g[v2].valid());

        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
    }

    TEST_F(data_reduction_degree_2, fold_none_initial_not_taken_reject)
    {
        graph g(3);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        auto reductions = factory_2_fold.build(graph_unit(g, meta_data, controller));

        reductions->apply();
        reductions->reject();

        EXPECT_TRUE(g[v0].valid());
        EXPECT_TRUE(g[v1].valid());
        EXPECT_TRUE(g[v2].valid());

        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
    }

    TEST_F(data_reduction_degree_2, fold_none_initial_taken_accept)
    {
        graph g(3);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        auto reductions = factory_2_fold.build(graph_unit(g, meta_data, controller));

        reductions->apply();
        controller.include_into_vertex_cover(v1);
        reductions->accept();

        EXPECT_FALSE(g[v0].valid());
        EXPECT_TRUE(g[v1].valid());
        EXPECT_FALSE(g[v2].valid());

        EXPECT_TRUE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_TRUE(util::is_in_vertex_cover(g[v2]));
    }

    TEST_F(data_reduction_degree_2, fold_none_initial_taken_reject)
    {
        graph g(3);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v1, v2);

        graph_meta_data meta_data(g, graph_meta_data::options { false, false, false });
        meta_data.vertex_cover_size(10);
        graph_controller controller(g, meta_data);
        auto reductions = factory_2_fold.build(graph_unit(g, meta_data, controller));

        reductions->apply();
        controller.include_into_vertex_cover(v1);
        reductions->reject();

        EXPECT_TRUE(g[v0].valid());
        EXPECT_TRUE(g[v1].valid());
        EXPECT_TRUE(g[v2].valid());

        EXPECT_FALSE(util::is_in_vertex_cover(g[v0]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v1]));
        EXPECT_FALSE(util::is_in_vertex_cover(g[v2]));
    }
}

#pragma clang diagnostic pop