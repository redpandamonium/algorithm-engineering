//
// Created by Leon Suchy on 07.11.18.
//

#include <graph_meta_data.hpp>

#include "graph_meta_data.hpp"
#include "heap/heap_comparators.hpp"

graph_meta_data::graph_meta_data(class graph& g)
    : m_options { true, true, true },
      m_min_comparator(g), m_max_comparator(g),
      m_min_heap(g.size(), m_min_comparator),
      m_max_heap(g.size(), m_max_comparator),
      m_lp_matching(g),
      m_graph(g),
      m_cover_size(1),
      m_recursion_depth(0)
{
    build_heaps();
}

graph_meta_data::graph_meta_data(class graph& g, const graph_meta_data::options& options_)
    : m_options(options_),
      m_min_comparator(g), m_max_comparator(g),
      m_min_heap((m_options.use_min_heap ? g.size() : 0), m_min_comparator),
      m_max_heap((m_options.use_max_heap ? g.size() : 0), m_max_comparator),
      m_lp_matching(g),
      m_graph(g),
      m_cover_size(1),
      m_recursion_depth(0)
{
    build_heaps();
}

graph_meta_data::min_heap_type& graph_meta_data::min_heap()
{
    return m_min_heap;
}

graph_meta_data::max_heap_type& graph_meta_data::max_heap()
{
    return m_max_heap;
}

int graph_meta_data::vertex_cover_size() const
{
    return m_cover_size;
}

void graph_meta_data::vertex_cover_size(int cover_size)
{
    m_cover_size = cover_size;
}

const graph_meta_data::min_heap_type& graph_meta_data::min_heap() const
{
    return m_min_heap;
}

const graph_meta_data::max_heap_type& graph_meta_data::max_heap() const
{
    return m_max_heap;
}

void graph_meta_data::rebuild()
{
    m_min_heap.clear();
    m_max_heap.clear();
    build_heaps();
}

void graph_meta_data::build_heaps()
{
    if (m_options.use_min_heap)
    {
        for (const vertex& v : m_graph)
        {
            m_min_heap.insert(v.id());
        }
    }
    if (m_options.use_max_heap)
    {
        for (const vertex& v : m_graph)
        {
            m_max_heap.insert(v.id());
        }
    }

}

void graph_meta_data::heap_element_degree_increased(vertex::id_type id)
{
    if (m_options.use_min_heap)
        m_min_heap.decrease(id);
    if (m_options.use_max_heap)
        m_max_heap.increase(id);
}

void graph_meta_data::heap_element_degree_decreased(vertex::id_type id)
{
    if (m_options.use_min_heap)
        m_min_heap.increase(id);
    if (m_options.use_max_heap)
        m_max_heap.decrease(id);
}

void graph_meta_data::heap_remove(vertex::id_type v)
{
    if (m_options.use_min_heap)
        m_min_heap.erase(v);
    if (m_options.use_max_heap)
        m_max_heap.erase(v);

    // decrease degree of all neighbors
    auto neighbors = m_graph[v].neighbors();
    for (auto n : neighbors)
        heap_element_degree_decreased(n);
}

void graph_meta_data::heap_insert(vertex::id_type v)
{
    if (m_options.use_min_heap)
        m_min_heap.insert(v);
    if (m_options.use_max_heap)
        m_max_heap.insert(v);

    // increase degree of neighbors
    auto neighbors = m_graph[v].neighbors();
    for (auto n : neighbors)
        heap_element_degree_increased(n);
}

void graph_meta_data::heap_remove(const std::vector<vertex::id_type>& ids)
{
    for (auto id : ids)
        heap_remove(id);
}

void graph_meta_data::heap_insert(const std::vector<vertex::id_type>& ids)
{
    for (auto id : ids)
        heap_insert(id);
}

void graph_meta_data::set_options(const graph_meta_data::options& ops)
{
    m_options = ops;
}

const graph_meta_data::options& graph_meta_data::get_options() const
{
    return m_options;
}

void graph_meta_data::set_recursion_depth(unsigned depth)
{
    m_recursion_depth = depth;
}

unsigned graph_meta_data::get_recursion_depth() const
{
    return m_recursion_depth;
}

void graph_meta_data::update_lp_matching()
{
    m_lp_matching.update();
}

const std::vector<edge> graph_meta_data::lp_matching()
{
    return m_lp_matching.calculate_matching();
}

void graph_meta_data::init_lp_matching()
{
    m_lp_matching.init();
}

const std::vector<vertex::id_type>& graph_meta_data::left_right() const
{
    return m_lp_matching.left_right();
}

const std::vector<vertex::id_type>& graph_meta_data::right_left() const
{
    return m_lp_matching.right_left();
}

const std::vector<u_int32_t> graph_meta_data::distances() const
{
    return m_lp_matching.distances();
}