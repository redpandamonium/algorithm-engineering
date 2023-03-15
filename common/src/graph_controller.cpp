//
// Created by Leon Suchy on 08.11.18.
//

#include "graph_controller.hpp"
#include "util.hpp"
#include <components.hpp>
#include <graph_controller.hpp>


using namespace std::literals::string_literals;

graph_controller::
operation_on_invalid_vertex_error::
operation_on_invalid_vertex_error(const graph& g,
                                  vertex::id_type vertex,
                                  const std::string& operation)
    : std::runtime_error("Illegal operation '"s + operation + "': Vertex '"s + g.name_of(vertex) + "' (ID: "s + std::to_string(vertex) + ") is invalid"s)
{ }

graph_controller::graph_controller(graph& graph, graph_meta_data& meta_data)
    : m_graph(graph), m_meta_data(meta_data)
{ }

void graph_controller::include_into_vertex_cover(vertex::id_type vertex)
{
    // invalid vertices can't be taken into a vertex cover
    if (!m_graph[vertex].valid())
        return;

    if (in_vertex_cover(vertex))
        return;

    decrease_vertex_cover_size(1); // decrease size of the vertex cover we're looking for

    auto& v = m_graph[vertex];
    invalidate(vertex); // has to be before recoloring
    v.color(v.color() | vertex::color_true);

    for (auto listener : m_listeners)
        listener->on_vertex_cover_include(vertex);
}

void graph_controller::exclude_from_vertex_cover(vertex::id_type vertex)
{
    if (!in_vertex_cover(vertex))
        return;

    increase_vertex_cover_size(1);

    auto& v = m_graph[vertex];
    v.color(v.color() & (~vertex::color_true));
    revalidate(vertex); // has to be after recoloring

    for (auto listener : m_listeners)
        listener->on_vertex_cover_exclude(vertex);
}

void graph_controller::invalidate(vertex::id_type vertex)
{
    if (!m_graph[vertex].valid())
        return;

    m_graph.invalidate(vertex);

    // update heaps
    m_meta_data.heap_remove(vertex);

    for (auto listener : m_listeners)
        listener->on_remove(vertex);
}

void graph_controller::revalidate(vertex::id_type vertex)
{
    // already valid
    if (m_graph[vertex].valid())
        return;

    m_graph.revalidate(vertex);

    // update heaps
    m_meta_data.heap_insert(vertex);

    for (auto listener : m_listeners)
        listener->on_add(vertex);
}

void graph_controller::attach(graph_listener& listener)
{
    m_listeners.push_back(&listener);
}

void graph_controller::detach(graph_listener& listener)
{
    m_listeners.erase(std::find(m_listeners.begin(), m_listeners.end(), &listener));
}

bool graph_controller::in_vertex_cover(vertex::id_type vertex)
{
    return (m_graph[vertex].color() & vertex::color_true) == vertex::color_true;
}

vertex::id_type graph_controller::group(const std::vector<vertex::id_type>& group)
{
    for (auto v : group)
        if (!m_graph[v].valid())
            throw std::invalid_argument(std::string(__func__) + ":  cannot group invalid vertex");

    vertex::id_type group_vertex = m_graph.group(group);
    if (group_vertex == vertex::invalid_id)
        return group_vertex; // failed

    // update heap
    m_meta_data.heap_remove(group);
    m_meta_data.heap_insert(group_vertex);

    // notify listeners
    for (auto vertex : group)
        for (auto listener : m_listeners)
            listener->on_remove(vertex);

    for (auto listener : m_listeners)
        listener->on_add(group_vertex);

    return group_vertex;
}

std::vector<vertex::id_type> graph_controller::ungroup(vertex::id_type vertex)
{
    if (!m_graph[vertex].is_group())
        throw std::invalid_argument(std::string(__func__) + ": Cannot ungroup vertex which is not a group");

    auto group_vertices = m_graph.ungroup(vertex);

    // update heap
    m_meta_data.heap_remove(vertex);
    m_meta_data.heap_insert(group_vertices);

    // notify listeners
    for (auto listener : m_listeners)
        listener->on_remove(vertex);

    for (auto group_vertex : group_vertices)
        for (auto listener : m_listeners)
            listener->on_add(group_vertex);

    return group_vertices;
}

void graph_controller::include_into_vertex_cover(const std::vector<vertex::id_type>& vertices)
{
    for (auto vertex : vertices)
        include_into_vertex_cover(vertex);
}

void graph_controller::exclude_from_vertex_cover(const std::vector<vertex::id_type>& vertices)
{
    for (auto vertex : vertices)
        exclude_from_vertex_cover(vertex);
}

void graph_controller::add_edge(vertex::id_type a, vertex::id_type b)
{
    // keep track of degrees before adding
    auto a_degree = m_graph[a].degree();
    auto b_degree = m_graph[b].degree();

    m_graph.add_edge(a, b);

    edge e { a, b };

    if (a_degree < m_graph[a].degree())
        m_meta_data.heap_element_degree_increased(a);
    if (b_degree < m_graph[b].degree())
        m_meta_data.heap_element_degree_increased(b);

    for (auto& observer : m_listeners)
        observer->on_add(e);
}

void graph_controller::remove_edge(vertex::id_type a, vertex::id_type b)
{
    // keep track of degrees before removing
    auto a_degree = m_graph[a].degree();
    auto b_degree = m_graph[b].degree();

    edge e { a, b };

    // remove
    m_graph.remove(e);

    // update heaps
    if (a_degree > m_graph[a].degree())
        m_meta_data.heap_element_degree_decreased(a);
    if (b_degree > m_graph[b].degree())
        m_meta_data.heap_element_degree_decreased(b);

    // inform observers
    for (auto& observer : m_listeners)
        observer->on_remove(e);
}

void graph_controller::mark(vertex::id_type vertex, vertex::color_type bit)
{
    if ((bit & vertex::color_true) == 1)
        throw std::logic_error("not allowed to remove vertex cover bit");
    if (!m_graph[vertex].valid())
        throw operation_on_invalid_vertex_error(m_graph, vertex, "mark");

    m_graph[vertex].color(m_graph[vertex].color() | bit);
}

void graph_controller::unmark(vertex::id_type vertex, vertex::color_type bit)
{
    if ((bit & vertex::color_true) == 1)
        throw std::logic_error("not allowed to remove vertex cover bit");
    if (!m_graph[vertex].valid())
        throw operation_on_invalid_vertex_error(m_graph, vertex, "mark");

    m_graph[vertex].color(m_graph[vertex].color() & (~bit));
}

void graph_controller::set_vertex_cover_size(int k)
{
    m_meta_data.vertex_cover_size(k);
}

void graph_controller::collect_garbage()
{
    // go over all vertices in the graph, delete invalid vertices
    // go in reverse order to have as few swap operations as possible
    for (auto i = m_graph.size_including_inactive() - 1; i >= 0; --i)
    {
        if (!m_graph[i].valid())
            m_graph.remove(i);

        // i must never be negative to avoid unsigned undefined behaviour
        if (i == 0)
            break;
    }

    // meta data has to be rebuilt because IDs have become invalid
    m_meta_data.rebuild();
}

std::vector<std::unique_ptr<subgraph>> graph_controller::components()
{
    return components::subgraphs(m_graph);
}

void graph_controller::increase_vertex_cover_size(unsigned n)
{
    m_meta_data.vertex_cover_size(m_meta_data.vertex_cover_size() + n);
}

void graph_controller::decrease_vertex_cover_size(unsigned n)
{
    m_meta_data.vertex_cover_size(m_meta_data.vertex_cover_size() - n);
}

void graph_controller::increment_recursion_depth()
{
    m_meta_data.set_recursion_depth(m_meta_data.get_recursion_depth() + 1);
}

void graph_controller::decrement_recursion_depth()
{
    m_meta_data.set_recursion_depth(m_meta_data.get_recursion_depth() - 1);
}

void graph_controller::lp_update_matching()
{
    m_meta_data.update_lp_matching();
}

const std::vector<edge> graph_controller::lp_matching()
{
    return m_meta_data.lp_matching();
}

