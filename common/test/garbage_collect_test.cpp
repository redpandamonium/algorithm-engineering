//
// Created by Leon Suchy on 18.11.18.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <graph.hpp>
#include <graph_meta_data.hpp>
#include <graph_controller.hpp>

namespace
{
    using namespace ::testing;

    class garbage_collect : public Test
    {
    protected:

        void SetUp() override
        {
            // build K5
            m_graph = graph { 5 };

            for (auto i = 0; i < 5; i++)
                m_graph.add_vertex();

            for (auto i = 0; i < 5; i++)
            {
                for (auto j = i + 1; j < 5; j++)
                {
                    m_graph.add_edge(i, j);
                }
            }

            m_meta_data = std::move(std::make_unique<graph_meta_data>(m_graph));
            m_controller = std::move(std::make_unique<graph_controller>(m_graph, *m_meta_data));
        }

        graph m_graph;
        std::unique_ptr<graph_meta_data> m_meta_data;
        std::unique_ptr<graph_controller> m_controller;
    };

    TEST_F(garbage_collect, completeness)
    {
        m_controller->invalidate(0);
        m_controller->invalidate(1);

        m_controller->collect_garbage();

        EXPECT_EQ(3, m_graph.size_including_inactive());
        EXPECT_EQ(3, m_graph.num_edges());
    }
}

#pragma clang diagnostic pop