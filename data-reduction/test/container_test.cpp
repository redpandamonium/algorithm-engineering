#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by Leon Suchy on 24.01.19.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>
#include <data_reduction_options.hpp>
#include <data_reduction_factory.hpp>
#include <container/exhaustive_infrequent_container.hpp>

namespace
{
    using namespace ::testing;

    TEST(infrequent_exhaustive, frequency_parsing)
    {
        nlohmann::json jsn = { { "name", "exhaustive-infrequent"}, { "frequency", 5 }, { "priority", 0 } };
        data_reduction_options conf { jsn["name"], jsn["priority"], { }, jsn, "::" };
        data_reduction_factory factory(conf);
        
        graph g;
        graph_meta_data m { g };
        graph_controller c { g, m };
        
        auto reductions = std::move(factory.build(graph_unit { g, m, c }));
        EXPECT_EQ(5, ((exhaustive_infrequent_container&) reductions->root()).get_frequency());
    }
}
#pragma clang diagnostic pop