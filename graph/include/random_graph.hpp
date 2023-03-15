//
// Created by Leon Suchy on 14.12.18.
//

#ifndef ALGORITHM_ENGINEERING_RANDOM_GRAPH_HPP
#define ALGORITHM_ENGINEERING_RANDOM_GRAPH_HPP

#include <random>
#include <graph.hpp>

struct generated_graph;

class random_graph_generator
{
public:

    typedef unsigned int seed_type;

    explicit random_graph_generator(unsigned max_vertices);

    generated_graph generate(seed_type seed = std::random_device()());

private:

    typedef std::mt19937 engine_type;

    engine_type m_engine;
    unsigned m_max_vertices;
};

struct generated_graph
{
    graph graph_;
    random_graph_generator::seed_type seed_;
};


#endif //ALGORITHM_ENGINEERING_RANDOM_GRAPH_HPP
