//
// Created by Leon Suchy on 09.11.18.
//

#ifndef ALGORITHM_ENGINEERING_GRAPH_UNIT_HPP
#define ALGORITHM_ENGINEERING_GRAPH_UNIT_HPP

#include <graph.hpp>
#include "graph_meta_data.hpp"
#include "graph_controller.hpp"

class graph_unit
{
public:

    graph_unit(const class graph& graph, const graph_meta_data& meta_data, graph_controller& controller);

    const class graph& graph() const;
    const graph_meta_data& meta_data() const;

    graph_controller& controller() const;

private:

    const class graph& m_graph;
    const graph_meta_data& m_meta_data;
    graph_controller& m_controller;

};

struct graph_unit_storage
{
    explicit graph_unit_storage(graph& g);
    explicit graph_unit_storage(graph& g, const graph_meta_data::options& opts);

    graph* g;
    graph_meta_data meta_data;
    graph_controller controller;

    graph_unit unit;
};


#endif //ALGORITHM_ENGINEERING_GRAPH_UNIT_HPP
