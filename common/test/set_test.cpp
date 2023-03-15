//
// Created by synis on 2/17/19.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "fast_set.hpp"

namespace
{
    using namespace ::testing;

    TEST (set_test, add)
    {
        fast_set set(8);

        bool success = set.add(0);
        EXPECT_TRUE(success);

        success = set.add(1);
        EXPECT_TRUE(success);
    }

    TEST (set_test, add_already_member)
    {
        fast_set set(8);

        bool success = set.add(0);
        success = set.add(0);

        EXPECT_FALSE(success);
    }

    TEST (set_test, invalid_index)
    {
        fast_set set(8);
        bool success = set.add(9);
        EXPECT_FALSE(success);
        success = set.is_member(9);
        EXPECT_FALSE(success);
    }

    TEST (set_test, add_after_clear)
    {
        fast_set set(8);
        bool success = set.add(0);
        EXPECT_TRUE(success);
        set.clear();
        success = set.add(0);
        EXPECT_TRUE(success);
    }

    TEST (set_test, clear_until_max_size)
    {
        auto max = std::numeric_limits<uint64_t>::max();
        fast_set set(8, max);

        bool success = set.add(0);
        EXPECT_TRUE(success);

        set.clear();

        success = set.add(0);
        EXPECT_TRUE(success);
    }

    TEST (set_test, remove_simple)
    {
        fast_set set(8);

        set.add(0);
        set.remove(0);
        auto success = set.is_member(0);

        EXPECT_FALSE(success);
    }
}

