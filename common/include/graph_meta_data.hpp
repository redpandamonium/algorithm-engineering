//
// Created by Leon Suchy on 07.11.18.
//

#ifndef ALGORITHM_ENGINEERING_GRAPH_META_DATA_HPP
#define ALGORITHM_ENGINEERING_GRAPH_META_DATA_HPP

#include "heap/heap.hpp"
#include "heap/heap_comparators.hpp"
#include <vertex.hpp>
#include <graph.hpp>
#include "graph_listener.hpp"
#include "hopcroft_karp_matching.hpp"

/**
 * @brief Data container to model all graph metadata
 */
class graph_meta_data
{
public:

    typedef heap<vertex::id_type, boost::heap::fibonacci_heap<vertex::id_type, boost::heap::compare<min_comparator>>> min_heap_type;
    typedef heap<vertex::id_type, boost::heap::fibonacci_heap<vertex::id_type, boost::heap::compare<max_comparator>>> max_heap_type;

    struct options
    {
        bool use_max_heap;
        bool use_min_heap;
        bool use_lp_graph;
    };

    explicit graph_meta_data(class graph& g);
    explicit graph_meta_data(class graph& g, const options& options_);

    graph_meta_data(graph_meta_data&&) = default;
    virtual ~graph_meta_data() = default;

    min_heap_type& min_heap();

    max_heap_type& max_heap();

    const min_heap_type& min_heap() const;

    const max_heap_type& max_heap() const;

    int vertex_cover_size() const;

    void vertex_cover_size(int cover_size);

    void set_options(const options& ops);

    const options& get_options() const;

    void rebuild();

    void heap_element_degree_increased(vertex::id_type id);
    void heap_element_degree_decreased(vertex::id_type id);
    void heap_remove(vertex::id_type v);
    void heap_insert(vertex::id_type v);
    void heap_remove(const std::vector<vertex::id_type>& ids);
    void heap_insert(const std::vector<vertex::id_type>& ids);

    void set_recursion_depth(unsigned depth);
    unsigned get_recursion_depth() const;

    void update_lp_matching();
    const std::vector<edge> lp_matching();
    const std::vector<vertex::id_type>& left_right() const;
    const std::vector<vertex::id_type>& right_left() const;
    const std::vector<u_int32_t> distances() const;

private:

    options m_options;

    min_comparator m_min_comparator;
    max_comparator m_max_comparator;
    min_heap_type m_min_heap;
    max_heap_type m_max_heap;
    hopcroft_karp_matching m_lp_matching;

    graph& m_graph;

    int m_cover_size;

    unsigned m_recursion_depth;

    void build_heaps();

    void init_lp_matching();
};

#endif //ALGORITHM_ENGINEERING_GRAPH_META_DATA_HPP
