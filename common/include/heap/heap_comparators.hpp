//
// Created by Leon Suchy on 07.11.18.
//

#ifndef ALGORITHM_ENGINEERING_HEAP_COMPARATORS_HPP
#define ALGORITHM_ENGINEERING_HEAP_COMPARATORS_HPP

#include <graph.hpp>

/**
 * @brief Comparator to build min heaps for vertices
 */
class min_comparator
{
public:

    explicit min_comparator(const graph& graph);

    min_comparator(const min_comparator& cpy) = default;
    min_comparator& operator=(const min_comparator& cpy) = default;

    bool operator()(const vertex::id_type& first, const vertex::id_type& second) const;

private:

    const graph* m_graph;
};

/**
 * @brief Comparator to build max heaps for vertices
 */
class max_comparator
{
public:

    explicit max_comparator(const graph& graph);

    max_comparator(const max_comparator& cpy) = default;
    max_comparator& operator=(const max_comparator& cpy) = default;

    bool operator()(const vertex::id_type& first, const vertex::id_type& second) const;

private:

    const graph* m_graph;
};

#endif //ALGORITHM_ENGINEERING_HEAP_COMPARATORS_HPP
