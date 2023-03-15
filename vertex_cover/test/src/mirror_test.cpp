//
// Created by Carsten Schubert on 2018-11-24.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <graph.hpp>
#include <graph_meta_data.hpp>
#include <graph_controller.hpp>
#include <mirrors.hpp>

namespace
{
    using namespace ::testing;

    TEST(mirrors, no_mirror)
    {
        graph g(2);
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        g.add_edge(v0, v1);

        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        mirrors mirror(graph_unit(g, meta_data, controller));


        auto mirrors = mirror.find_mirrors(v0);

        EXPECT_EQ(mirrors.size(), 1);

    }


    TEST(mirrors, mirror_ex_1)
    {
        graph g(6);
        auto u = g.add_vertex();
        auto v = g.add_vertex();
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v, v0);
        g.add_edge(v, v1);
        g.add_edge(v, v2);
        g.add_edge(v, v3);

        g.add_edge(u, v0);
        g.add_edge(u, v1);
        g.add_edge(u, v2);
        g.add_edge(u, v3);



        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        mirrors mirror(graph_unit(g, meta_data, controller));


        auto mirrors = mirror.find_mirrors(v);
        std::vector<vertex::id_type> u_vec = {u, v};
        std::sort(mirrors.begin(), mirrors.end());
        std::sort(u_vec.begin(), u_vec.end());

        EXPECT_EQ(mirrors, u_vec);

    }

    TEST(mirrors, mirror_ex_2)
    {
        graph g(6);
        auto u = g.add_vertex();
        auto v = g.add_vertex();
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v, v0);
        g.add_edge(v, v1);
        g.add_edge(v, v2);
        g.add_edge(v, v3);

        g.add_edge(u, v0);
        g.add_edge(u, v1);
        g.add_edge(u, v2);



        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        mirrors mirror(graph_unit(g, meta_data, controller));


        auto mirrors = mirror.find_mirrors(v);
        std::vector<vertex::id_type> u_vec = {u, v};
        std::sort(mirrors.begin(), mirrors.end());
        std::sort(u_vec.begin(), u_vec.end());

        EXPECT_EQ(mirrors, u_vec);

    }

    TEST(mirrors, mirror_ex_3)
    {
        graph g(6);
        auto u = g.add_vertex();
        auto v = g.add_vertex();
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v, v0);
        g.add_edge(v, v1);
        g.add_edge(v, v2);
        g.add_edge(v, v3);

        g.add_edge(u, v0);
        g.add_edge(u, v1);

        g.add_edge(v2, v3);


        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        mirrors mirror(graph_unit(g, meta_data, controller));


        auto mirrors = mirror.find_mirrors(v);
        std::vector<vertex::id_type> u_vec = {u, v};
        std::sort(mirrors.begin(), mirrors.end());
        std::sort(u_vec.begin(), u_vec.end());

        EXPECT_EQ(mirrors, u_vec);

    }

    TEST(mirrors, mirror_ex_4)
    {
        graph g(6);
        auto u = g.add_vertex();
        auto v = g.add_vertex();
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();

        g.add_edge(v0, v1);
        g.add_edge(v, v0);
        g.add_edge(v, v1);
        g.add_edge(v, v2);
        g.add_edge(v, v3);

        g.add_edge(u, v0);

        g.add_edge(v2, v3);
        g.add_edge(v1, v3);
        g.add_edge(v1, v2);



        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        mirrors mirror(graph_unit(g, meta_data, controller));


        auto mirrors = mirror.find_mirrors(v);
        std::vector<vertex::id_type> u_vec = {u, v};
        std::sort(mirrors.begin(), mirrors.end());
        std::sort(u_vec.begin(), u_vec.end());

        EXPECT_EQ(mirrors, u_vec);

    }

    TEST(mirrors, more_no_mirror)
    {
        graph g(7);
        auto v = g.add_vertex();
        auto u = g.add_vertex();
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();


        g.add_edge(v, v0);
        g.add_edge(v, v1);
        g.add_edge(v, v2);

        g.add_edge(v, v3);
        g.add_edge(v, v4);


        g.add_edge(v1, v0);     // Clique of size 3
        g.add_edge(v1, v2);
        g.add_edge(v0, v2);

        g.add_edge(u, v3);
        g.add_edge(u, v0);



        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        mirrors mirror(graph_unit(g, meta_data, controller));


        auto mirrors = mirror.find_mirrors(v);
        std::vector<vertex::id_type> v_vec = {v};
        std::sort(mirrors.begin(), mirrors.end());
        std::sort(v_vec.begin(), v_vec.end());

        EXPECT_EQ(mirrors, v_vec);

    }

    TEST(mirrors, complex_no_mirror)
    {
        graph g(11);
        auto v = g.add_vertex();
        auto v0 = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();
        auto u = g.add_vertex();
        auto w = g.add_vertex();



        g.add_edge(v, v0);      // v connected to clique
        g.add_edge(v, v1);
        g.add_edge(v, v2);
        g.add_edge(v, v3);



        g.add_edge(v1, v0);     // Clique of size 4
        g.add_edge(v1, v2);
        g.add_edge(v0, v2);
        g.add_edge(v3, v0);
        g.add_edge(v3, v1);
        g.add_edge(v3, v2);



        g.add_edge(v, v4);
        g.add_edge(v, v5);



        g.add_edge(v5, v4);
        g.add_edge(v0, v4);
        g.add_edge(v1, v4);
        g.add_edge(v2, v4);


        g.add_edge(u, v3);
        g.add_edge(u, v5);

        g.add_edge(w, v4);
        g.add_edge(w, v3);





        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        mirrors mirror(graph_unit(g, meta_data, controller));


        auto mirrors = mirror.find_mirrors(v);
        std::vector<vertex::id_type> v_vec = {v, u};
        std::sort(mirrors.begin(), mirrors.end());
        std::sort(v_vec.begin(), v_vec.end());

        EXPECT_EQ(mirrors, v_vec);

    }

    TEST(mirrors, many_mirrors)
    {
        graph g(11);
        auto v = g.add_vertex();
        auto c0 = g.add_vertex();
        auto c1 = g.add_vertex();

        auto l0 = g.add_vertex();
        auto l1 = g.add_vertex();
        auto l2 = g.add_vertex();
        auto l3 = g.add_vertex();


        auto r0 = g.add_vertex();
        auto r1 = g.add_vertex();
        auto r2 = g.add_vertex();
        auto r3 = g.add_vertex();
        auto r4 = g.add_vertex();






        g.add_edge(v, c0);      // v in triangle
        g.add_edge(v, c1);
        g.add_edge(c0, c1);

        for (int j = 3; j < 7; ++j) {   // v connected to l
            g.add_edge(v, j);
        }

        for (int k = 3; k < 7; ++k) {    // all l to all r
            for (int j = 7; j < 12; ++j) {
                g.add_edge(k, j);
            }

        }



        graph_meta_data meta_data(g);
        graph_controller controller(g, meta_data);
        mirrors mirror(graph_unit(g, meta_data, controller));


        auto mirrors = mirror.find_mirrors(v);
        std::vector<vertex::id_type> v_vec = {v, r0, r1, r2, r3, r4};
        std::sort(mirrors.begin(), mirrors.end());
        std::sort(v_vec.begin(), v_vec.end());

        EXPECT_EQ(mirrors, v_vec);

    }







}
