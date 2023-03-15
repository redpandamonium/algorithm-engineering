#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by Leon Suchy on 25.10.18.
//

#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <graph_reader.hpp>

namespace
{
    using namespace ::testing;

    TEST(graph_reader_test, simple)
    {
        static constexpr const char* CONTENT = "a b\nb c";
        std::istringstream istream { CONTENT };

        graph_reader reader { istream };

        auto data = reader.read();

        // sizes match
        EXPECT_EQ(3, data.vertices.size());
        EXPECT_EQ(3, data.names.size());

        // verify all vertices exist
        EXPECT_TRUE(data.names.left.find("a") != data.names.left.end());
        EXPECT_TRUE(data.names.left.find("b") != data.names.left.end());
        EXPECT_TRUE(data.names.left.find("c") != data.names.left.end());

        auto index_a = data.names.left.find("a")->second;
        auto index_b = data.names.left.find("b")->second;
        auto index_c = data.names.left.find("c")->second;

        // verify edges
        EXPECT_THAT(data.vertices[index_a].neighbors(), Contains(index_b));
        EXPECT_THAT(data.vertices[index_b].neighbors(), Contains(index_a));
        EXPECT_THAT(data.vertices[index_b].neighbors(), Contains(index_c));
        EXPECT_THAT(data.vertices[index_c].neighbors(), Contains(index_b));
        EXPECT_THAT(data.vertices[index_a].neighbors(), Not(Contains(index_c)));
        EXPECT_THAT(data.vertices[index_c].neighbors(), Not(Contains(index_a)));
    }

    TEST(graph_reader_test, comment)
    {
        static constexpr const char* CONTENT = "#comment\na b\n#comment2\nb c\n   \t#comment";
        std::istringstream istream { CONTENT };

        graph_reader reader { istream };

        auto data = reader.read();

        // sizes match
        EXPECT_EQ(3, data.vertices.size());
        EXPECT_EQ(3, data.names.size());

        // verify all vertices exist
        EXPECT_TRUE(data.names.left.find("a") != data.names.left.end());
        EXPECT_TRUE(data.names.left.find("b") != data.names.left.end());
        EXPECT_TRUE(data.names.left.find("c") != data.names.left.end());
    }

    TEST(graph_reader_test, empty_line)
    {
        static constexpr const char* CONTENT = "\na b\n \nb c\n   \t";
        std::istringstream istream { CONTENT };

        graph_reader reader { istream };

        auto data = reader.read();

        // sizes match
        EXPECT_EQ(3, data.vertices.size());
        EXPECT_EQ(3, data.names.size());

        // verify all vertices exist
        EXPECT_TRUE(data.names.left.find("a") != data.names.left.end());
        EXPECT_TRUE(data.names.left.find("b") != data.names.left.end());
        EXPECT_TRUE(data.names.left.find("c") != data.names.left.end());
    }

    TEST(graph_reader_test, line_with_only_one_vertex)
    {
        static constexpr const char* CONTENT = "a b\nc";
        std::istringstream istream { CONTENT };

        graph_reader reader { istream };

        EXPECT_THROW(reader.read(), graph_parse_exception);
    }

    TEST(graph_reader_test, line_with_three_vertices)
    {
        static constexpr const char* CONTENT = "a b\na b c";
        std::istringstream istream { CONTENT };

        graph_reader reader { istream };

        EXPECT_THROW(reader.read(), graph_parse_exception);
    }

    TEST(graph_reader_test, loop)
    {
        static constexpr const char* CONTENT = "a b\na a";
        std::istringstream istream { CONTENT };

        graph_reader reader { istream };

        EXPECT_THROW(reader.read(), graph_parse_exception);
    }
}
#pragma clang diagnostic pop