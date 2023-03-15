//
// Created by Leon Suchy on 09.11.18.
//

#include <graph_unit.hpp>

#include "../include/graph_unit.hpp"

graph_unit::graph_unit(const class graph& graph, const graph_meta_data& meta_data, graph_controller& controller)
    : m_graph(graph), m_meta_data(meta_data), m_controller(controller)
{ }

const graph& graph_unit::graph() const
{
    return m_graph;
}

const graph_meta_data& graph_unit::meta_data() const
{
    return m_meta_data;
}

graph_controller& graph_unit::controller() const
{
    return m_controller;
}

graph_unit_storage::graph_unit_storage(graph& g_)
    : g(&g_), meta_data(g_), controller(g_, meta_data), unit(g_, meta_data, controller)
{ }

graph_unit_storage::graph_unit_storage(graph& g_, const graph_meta_data::options& opts)
    : g(&g_), meta_data(g_, opts), controller(g_, meta_data), unit(g_, meta_data, controller)
{

}
