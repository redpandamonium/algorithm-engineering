//
// Created by Leon Suchy on 17.11.18.
//

#ifndef ALGORITHM_ENGINEERING_GRAPH_ITERATOR_HPP
#define ALGORITHM_ENGINEERING_GRAPH_ITERATOR_HPP

class graph; // predeclare, user has to include graph.hpp

#include "../include/vertex.hpp"

class graph_iterator : public std::iterator<std::forward_iterator_tag, vertex, ptrdiff_t, vertex*, vertex&>
{
public:

    typedef std::size_t size_type;
    typedef size_type index_type;

    explicit graph_iterator(graph& graph, size_type index = 0);

    vertex& operator*() const;

    vertex* operator->() const;

    graph_iterator& operator++();

    const graph_iterator operator++(int offset);

    bool operator==(const graph_iterator& rhs) const;

    bool operator!=(const graph_iterator& rhs) const;

private:

    graph*      m_graph;
    index_type  m_index;
};

class const_graph_iterator : public std::iterator<std::forward_iterator_tag, vertex, ptrdiff_t, const vertex*, const vertex&>
{
public:

    typedef std::size_t size_type;
    typedef size_type index_type;

    explicit const_graph_iterator(const graph& graph, size_type index = 0);

    const vertex& operator*() const;

    const vertex* operator->() const;

    const_graph_iterator& operator++();

    const const_graph_iterator operator++(int offset);

    bool operator==(const const_graph_iterator& rhs) const;

    bool operator!=(const const_graph_iterator& rhs) const;

private:

    const graph* m_graph;
    index_type   m_index;
};

#endif //ALGORITHM_ENGINEERING_GRAPH_ITERATOR_HPP
