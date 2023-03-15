//
// Created by Leon Suchy on 23.11.18.
//

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <util.hpp>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma ide diagnostic ignored "performance-inefficient-vector-operation"
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"

namespace
{
    using namespace ::testing;

    TEST(util, flip)
    {
        std::map<int, int> m;
        m.emplace(1, 2);
        m.emplace(3, 4);
        m.emplace(5, 6);

        auto flipped = graphlib::detail::util::flip(m);
        EXPECT_EQ(1, flipped[2]);
        EXPECT_EQ(3, flipped[4]);
        EXPECT_EQ(5, flipped[6]);
    }
}