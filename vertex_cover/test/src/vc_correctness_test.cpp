//
// Created by Leon Suchy on 28.10.18.
//

// std
#include <fstream>
#include <memory> // unique_ptr


// googletest
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// graph
#include <graph.hpp>
#include <graph_io.hpp>

// vertex_cover
#include <vertex_cover_solver.hpp>
#include <solver/max_degree_neighbor_rule_solver.hpp>
#include <solver/random_edge_solver.hpp>
#include <vertex_cover.hpp>

namespace
{
    using namespace ::testing;

    struct test_data
    {
        std::shared_ptr<graph> G;
        unsigned solution_size;
    };

    struct vc_correctness : public Test
    {
    protected:

        static const std::vector<std::string> all_files;
    };

    const std::vector<std::string> vc_correctness::all_files = {
            "000002_000000000006",
            "000004_000000000024",
            "000012_000000000072",
            "000012_000000000120",
            "000024_000000000144",
            "000095_000000000570",
            "000095_000000000950",
            "000300_000000001800",
            "adjnoun.graph",
            "chesapeake.graph",
            "polbooks.graph",
            "000028_000000000033",
            "000070_000000000210",
            "football.graph",
            "crack_reduced.graph",
            "crack.graph",
            "crack_reduced_3.graph",
            "crack_reduced_crown_bug",
            "ninja_star",
    };

    test_data load(const std::string& name)
    {
        test_data data { nullptr, 0 };
        graph_data g_data;

        std::fstream input("test/res/" + name + ".input", (std::ios_base::openmode) std::ios::beg | std::ios::in);
        if (input.bad())
            return { nullptr, 0 };

        input >> g_data;
        data.G = std::make_shared<graph>(std::move(g_data));
        input.close();

        std::fstream solution("test/res/" + name + ".solution", (std::ios_base::openmode) std::ios::beg | std::ios::in);
        if (solution.bad())
            return { nullptr, 0 };

        solution >> data.solution_size;

        return data;
    }

    unsigned solution_size(graph& g)
    {
        unsigned solution_size = 0;

        for (const auto& v : g)
        {
            if (v.color() == vertex::color_true)
                solution_size++;
        }

        return solution_size;
    }

    TEST_F(vc_correctness, max_degree_neighbor_rule_solver)
    {
        configuration config(R"({
            "data-reductions": {
                "name": "exhaustive", "priority": 1, "data-reductions": [
                { "name": "exhaustive", "priority": 1, "data-reductions": [
                    { "name": "exhaustive", "priority": 2, "data-reductions": [
                        {"name": "degree-0", "priority": 4},
                        {"name": "degree-1", "priority": 3},
                        {"name": "degree > k", "priority": 0},
                        {"name": "triangle", "priority": 2},
                        {"name": "2-fold",   "priority": 1}
                    ]},
                    { "name": "exhaustive", "priority": 1, "data-reductions": [
                        {"name": "crown",   "priority": 0}
                    ]}
                ]},
                { "name": "exhaustive-infrequent", "priority": 0, "frequency": 5, "data-reductions": [
                    {"name": "unconfined",   "priority": 0},
                    {"name": "dominate",     "priority": 1}
                ]}
            ]},
            "kernel-edge-condition": false
        })"_json);

        for (auto i = 0; i < all_files.size(); ++i)
        {
            std::cout << "\nSolving: " << all_files[i] << std::endl;
            test_data data = load(all_files[i]);
            vertex_cover solver(*data.G, config, config);
            solver.solve();
            unsigned solution_size_calculated = solution_size(*data.G);
            EXPECT_EQ(data.solution_size, solution_size_calculated);
        }
    }

    TEST_F(vc_correctness, no_data_reductions)
    {
        configuration config(R"({
            "data-reductions": { "name": "exhaustive", "priority": 0, "data-reductions": [] }
        })"_json);

        for (auto i = 1; i < 5; ++i)
        {
            std::cout << "\nSolving: " << all_files[i] << std::endl;
            test_data data = load(all_files[i]);
            vertex_cover solver(*data.G, config, config);
            solver.solve();
            unsigned solution_size_calculated = solution_size(*data.G);
            EXPECT_EQ(data.solution_size, solution_size_calculated);
        }
    }

    TEST_F(vc_correctness, with_folding)
    {
        configuration config(R"({"data-reductions": {
                "name": "exhaustive", "priority": 1, "data-reductions": [{"name": "2-fold", "priority": 1}]}})"_json);

        for (auto i = 12; i < 7; ++i)
        {
            std::cout << "\nSolving: " << all_files[i] << std::endl;
            test_data data = load(all_files[i]);
            vertex_cover solver(*data.G, config, config);
            solver.solve();
            unsigned solution_size_calculated = solution_size(*data.G);
            EXPECT_EQ(data.solution_size, solution_size_calculated);
        }
    }

    TEST_F(vc_correctness, components)
    {
        graph g;
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();

        g.add_edge(v1, v2);
        g.add_edge(v3, v4);

        configuration empty(R"({"data-reductions": []})"_json);

        vertex_cover solver(g, empty, empty);
        solver.solve();
        unsigned solution = solution_size(g);
        EXPECT_EQ(2, solution);
    }

    TEST_F(vc_correctness, components_complex)
    {
        graph g;
        auto root = g.add_vertex();
        auto v1 = g.add_vertex();
        auto v2 = g.add_vertex();
        auto v3 = g.add_vertex();
        auto v4 = g.add_vertex();
        auto v5 = g.add_vertex();
        auto v6 = g.add_vertex();
        auto v7 = g.add_vertex();
        auto v8 = g.add_vertex();

        g.add_edge(root, v1);
        g.add_edge(v1, v2);
        g.add_edge(root, v3);
        g.add_edge(v3, v4);
        g.add_edge(root, v5);
        g.add_edge(v5, v6);
        g.add_edge(root, v7);
        g.add_edge(v7, v8);

        configuration empty(R"({"data-reductions": []})"_json);

        vertex_cover solver(g, empty, empty);
        solver.solve();
        auto solution = solution_size(g);
        EXPECT_EQ(4, solution);
    }

    /*
    TEST(vc_correctness, random_edge_solver)
    {
        for (auto i = 0; i < 2; i++)
        {
            test_data data = load(std::string(NAMES[i]));
            vertex_cover_solver solver(*data.G, new random_edge_solver(*data.G));
            solver.solve();
            unsigned solution_size_calculated = solution_size(*data.G);
            EXPECT_EQ(data.solution_size, solution_size_calculated);
        }
    }
     */
}