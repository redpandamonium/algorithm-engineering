//
// Created by Leon Suchy on 2019-02-08.
//

#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <pace_graph_reader.hpp>
#include <pace_solution_writer.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-result"
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"

namespace
{
    using namespace ::testing;

    vertex::id_type get_id_for_name(const graph_data& data, const std::string& name)
    {
        auto it = data.names.left.find(name);
        return it->second;
    }

    std::string get_name_for_id(const graph_data& data, vertex::id_type id)
    {
        auto it = data.names.right.find(id);
        return it->second;
    }

    TEST(pace_reader, extension)
    {
        std::string correct = "pace.gr";
        std::string wrong_no_ext = "pace";
        std::string wrong_ext = "pace.dimacs";
        std::string correct_multiple_dots = "pace.graph.gr";
        std::string wrong_hidden_file = ".gr";

        EXPECT_TRUE(pace_graph_reader::is_pace_graph_file(correct));
        EXPECT_TRUE(pace_graph_reader::is_pace_graph_file(correct_multiple_dots));
        EXPECT_FALSE(pace_graph_reader::is_pace_graph_file(wrong_ext));
        EXPECT_FALSE(pace_graph_reader::is_pace_graph_file(wrong_no_ext));
        EXPECT_FALSE(pace_graph_reader::is_pace_graph_file(wrong_hidden_file));
    }

    TEST(pace_reader, no_header)
    {
        std::stringstream content;
        content << "";
        pace_graph_reader reader(content);
        EXPECT_THROW(reader.read(), pace_graph_reader::parse_exception);
    }

    TEST(pace_reader, simple_graph)
    {
        std::stringstream content;
        content << "p td 4 4" << '\n';
        content << "1 2" << '\n';
        content << "2 3" << '\n';
        content << "3 4" << '\n';
        content << "4 1" << '\n';

        pace_graph_reader reader(content);
        graph_data data = reader.read();

        // sizes
        EXPECT_EQ(4, data.vertices.size());
        EXPECT_EQ(4, data.names.left.size());
        EXPECT_EQ(4, data.names.right.size());

        auto v1 = get_id_for_name(data, "1");
        auto v2 = get_id_for_name(data, "2");
        auto v3 = get_id_for_name(data, "3");
        auto v4 = get_id_for_name(data, "4");

        // unique IDs
        EXPECT_NE(v1, v2);
        EXPECT_NE(v1, v3);
        EXPECT_NE(v1, v4);
        EXPECT_NE(v2, v3);
        EXPECT_NE(v2, v4);
        EXPECT_NE(v3, v4);

        // Edges that do exist
        EXPECT_TRUE(data.vertices[v1].get_incident_edge_to(data.vertices[v2]));
        EXPECT_TRUE(data.vertices[v2].get_incident_edge_to(data.vertices[v3]));
        EXPECT_TRUE(data.vertices[v3].get_incident_edge_to(data.vertices[v4]));
        EXPECT_TRUE(data.vertices[v4].get_incident_edge_to(data.vertices[v1]));
        EXPECT_TRUE(data.vertices[v2].get_incident_edge_to(data.vertices[v1]));
        EXPECT_TRUE(data.vertices[v3].get_incident_edge_to(data.vertices[v2]));
        EXPECT_TRUE(data.vertices[v4].get_incident_edge_to(data.vertices[v3]));
        EXPECT_TRUE(data.vertices[v1].get_incident_edge_to(data.vertices[v4]));

        // Edges that don't exist
        EXPECT_FALSE(data.vertices[v1].get_incident_edge_to(data.vertices[v3]));
        EXPECT_FALSE(data.vertices[v2].get_incident_edge_to(data.vertices[v4]));
        EXPECT_FALSE(data.vertices[v3].get_incident_edge_to(data.vertices[v1]));
        EXPECT_FALSE(data.vertices[v4].get_incident_edge_to(data.vertices[v2]));

        // back map
        EXPECT_EQ("1", get_name_for_id(data, v1));
        EXPECT_EQ("2", get_name_for_id(data, v2));
        EXPECT_EQ("3", get_name_for_id(data, v3));
        EXPECT_EQ("4", get_name_for_id(data, v4));
    }

    TEST(pace_reader, loop)
    {
        std::stringstream content;
        content << "p td 1 1" << '\n';
        content << "1 1" << '\n';

        pace_graph_reader reader(content);
        graph_data data = reader.read();

        // sizes
        EXPECT_EQ(1, data.vertices.size());
        EXPECT_EQ(1, data.names.left.size());
        EXPECT_EQ(1, data.names.right.size());

        auto v1 = get_id_for_name(data, "1");
        EXPECT_EQ(1, data.vertices[v1].get_incident_edges_including_inactive().size());
        EXPECT_TRUE(data.vertices[v1].get_incident_edge_to(data.vertices[v1]));
        data.vertices[v1].remove_edge_to(data.vertices[v1]);
        EXPECT_EQ(0, data.vertices[v1].get_incident_edges_including_inactive().size());
    }

    TEST(pace_reader, comments)
    {
        std::stringstream content;
        content << "c I am a comment" << '\n';
        content << "" << '\n';
        content << "p td 4 4" << '\n';
        content << "1 2" << '\n';
        content << "2 3" << '\n';
        content << "c Comment" << '\n';
        content << "3 4" << '\n';
        content << "4 1" << '\n';
        content << "\t" << '\n';

        pace_graph_reader reader(content);
        graph_data data = reader.read();

        // sizes
        EXPECT_EQ(4, data.vertices.size());
        EXPECT_EQ(4, data.names.left.size());
        EXPECT_EQ(4, data.names.right.size());
    }

    TEST(pace_reader, multiple_headers)
    {
        std::stringstream content;
        content << "p td 4 4" << '\n';
        content << "1 2" << '\n';
        content << "2 3" << '\n';
        content << "3 4" << '\n';
        content << "4 1" << '\n';
        content << "p td 4 4" << '\n';

        pace_graph_reader reader(content);
        EXPECT_THROW(reader.read(), pace_graph_reader::parse_exception);
    }

    TEST(pace_writer, simple_solution)
    {
        std::stringstream content;
        content << "p td 2 1" << '\n';
        content << "1 2" << '\n';

        pace_graph_reader reader(content);
        graph_data data = reader.read();

        data.vertices[get_id_for_name(data, "1")].invalidate();

        std::stringstream expected;
        expected << "s vc 2 1" << '\n';
        expected << "2" << '\n';

        std::stringstream result;
        pace_solution_writer writer(result);
        writer.write(data);

        EXPECT_EQ(expected.str(), result.str());
    }

    TEST(pace_writer, empty_solution)
    {
        std::stringstream content;
        content << "p td 2 1" << '\n';
        content << "1 2" << '\n';

        pace_graph_reader reader(content);
        graph_data data = reader.read();

        std::stringstream expected;
        expected << "s vc 2 0" << '\n';

        data.vertices[get_id_for_name(data, "1")].invalidate();
        data.vertices[get_id_for_name(data, "2")].invalidate();

        std::stringstream result;
        pace_solution_writer writer(result);
        writer.write(data);

        EXPECT_EQ(expected.str(), result.str());
    }

    TEST(pace_writer, sorted)
    {
        std::stringstream content;
        content << "p td 2 1" << '\n';
        content << "2 1" << '\n';

        pace_graph_reader reader(content);
        graph_data data = reader.read();

        std::stringstream expected;
        expected << "s vc 2 2" << '\n';
        expected << "1" << '\n';
        expected << "2" << '\n';

        std::stringstream result;
        pace_solution_writer writer(result);
        writer.write(data);

        EXPECT_EQ(expected.str(), result.str());
    }
}
#pragma clang diagnostic pop