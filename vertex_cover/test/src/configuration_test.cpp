#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"
#pragma ide diagnostic ignored "cert-err58-cpp"

//
// Created by Leon Suchy on 25.11.18.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <configuration.hpp>
#include <data_reduction_options.hpp>

namespace
{
    using namespace ::testing;

    TEST(configuration, data_reduction_empty)
    {
        nlohmann::json jsn = R"({"data-reductions": {}})"_json;
        EXPECT_THROW(configuration config(jsn), configuration_parse_exception);
    }

    TEST(configuration, data_reduction_shallow)
    {
        nlohmann::json jsn = R"({"data-reductions": { "name": "degree-0", "priority": 0}})"_json;
        configuration config(jsn);
        EXPECT_EQ(0, config.data_reductions().children.size());
        EXPECT_STREQ("degree-0", config.data_reductions().name.c_str());
        EXPECT_EQ(0, config.data_reductions().priority);
    }

    TEST(configuration, data_reduction_deep)
    {
        nlohmann::json jsn = R"({"data-reductions": {"name": "exhaustive", "priority": 0, "data-reductions": [{"name": "degree-0", "priority": 0}]}})"_json;
        configuration config(jsn);

        ASSERT_STREQ("exhaustive", config.data_reductions().name.c_str());
        ASSERT_EQ(0, config.data_reductions().priority);
        ASSERT_EQ(1, config.data_reductions().children.size());

        data_reduction_options r = config.data_reductions().children[0];
        EXPECT_STREQ("degree-0", r.name.c_str());
        EXPECT_EQ(0, r.children.size());
        EXPECT_EQ(0, r.priority);
    }

    TEST(configuration, data_reduction_empty_second_layer)
    {
        nlohmann::json jsn = R"({"data-reductions": {"name": "exhaustive", "priority": 0, "data-reductions": []}})"_json;
        EXPECT_THROW(configuration config(jsn), configuration_parse_exception);
    }

    TEST(configuration, data_reduction_invalid_container_name)
    {
        nlohmann::json jsn = R"({"data-reductions": {"name": "def-does-not-exist", "priority": 0}})"_json;
        EXPECT_THROW(configuration config(jsn), configuration_parse_exception);
    }

    TEST(configuration, data_reduction_invalid_reduction_name)
    {
        nlohmann::json jsn = R"({"data-reductions": {"name": "exhaustive", "priority": 0, "data-reductions": [{"name": "degree-7", "priority": 0}]}})"_json;
        EXPECT_THROW(configuration config(jsn), configuration_parse_exception);
    }

    TEST(configuration, data_reduction_prio_not_unique)
    {
        nlohmann::json jsn = R"({
            "data-reductions": { "name": "exhaustive", "priority": 0, "data-reductions": [
                { "name": "degree-0",   "priority": 0 },
                { "name": "degree-1",   "priority": 1 },
                { "name": "2-fold",     "priority": 0 },
                { "name": "degree > k", "priority": 3 }
            ]}})"_json;

        EXPECT_THROW(configuration config(jsn), configuration_parse_exception);
    }

    TEST(configuration, complex)
    {
        nlohmann::json jsn = R"({
            "data-reductions": { "name": "exhaustive", "priority": 0, "data-reductions": [
                { "name": "degree-0",   "priority": 0 },
                { "name": "exhaustive", "priority": 1, "data-reductions": [
                    { "name": "degree-1",   "priority": 1 }
                ]}
            ]}})"_json;
        configuration config(jsn);

        EXPECT_STREQ("exhaustive", config.data_reductions().name.c_str());
        EXPECT_EQ(2, config.data_reductions().children.size());

        EXPECT_STREQ("exhaustive", config.data_reductions().children[0].name.c_str());
        EXPECT_EQ(1, config.data_reductions().children[0].children.size());

        EXPECT_STREQ("degree-1", config.data_reductions().children[0].children[0].name.c_str());
        EXPECT_EQ(0, config.data_reductions().children[0].children[0].children.size());

        EXPECT_STREQ("degree-0", config.data_reductions().children[1].name.c_str());
        EXPECT_EQ(0, config.data_reductions().children[1].children.size());
    }

    TEST(configuration, data_reduction_ordering)
    {
        nlohmann::json jsn = R"({
            "data-reductions": { "name": "exhaustive", "priority": 0, "data-reductions": [
                { "name": "degree-0",   "priority": 2 },
                { "name": "degree-1",   "priority": 1 },
                { "name": "2-fold",     "priority": 3 },
                { "name": "degree > k", "priority": 0 }
            ]}})"_json;

        configuration config(jsn);
        const data_reduction_options& r = config.data_reductions();

        EXPECT_STREQ("degree-0",   r.children[1].name.c_str());
        EXPECT_STREQ("degree-1",   r.children[2].name.c_str());
        EXPECT_STREQ("2-fold",     r.children[0].name.c_str());
        EXPECT_STREQ("degree > k", r.children[3].name.c_str());
    }
}

#pragma clang diagnostic pop