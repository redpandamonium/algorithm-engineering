//
// Created by Carsten Schubert on 2018-12-01.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <graph.hpp>
#include <graph_data.hpp>
#include <edge_writer.hpp>
#include <open_wbo_writer.hpp>

using namespace ::testing;

TEST(edge_writer, simple)
{
    graph g(3);
    auto v0 = g.add_vertex();
    auto v1 = g.add_vertex();
    auto v2 = g.add_vertex();
    auto v3 = g.add_vertex();



    g.add_edge(v0, v1);
    g.add_edge(v1, v2);
    g.add_edge(v0, v2);
    g.add_edge(v3, v2);


    std::stringstream ss;
    edge_writer writer (ss);
    //open_wbo_writer writer(ss);

    writer.write(g.data());
    auto str = ss.str();
    EXPECT_STREQ(str.c_str(), "A0 A1\nA0 A2\nA1 A2\nA2 A3\n");

}
