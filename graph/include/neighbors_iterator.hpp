//
// Created by synis on 3/2/19.
//

#ifndef ALGORITHM_ENGINEERING_NEIGHBORS_ITERATOR_HPP
#define ALGORITHM_ENGINEERING_NEIGHBORS_ITERATOR_HPP


#include <iterator>
#include "vertex.hpp"
#include "graph.hpp"

class neighbors_iterator : public std::iterator<std::forward_iterator_tag, vertex, ptrdiff_t, vertex*, vertex&>
{
public:
    typedef std::size_t size_type;
    typedef std::vector<::graphlib::directed_edge>::iterator container_iterator_type;

    explicit neighbors_iterator(graph& g, vertex& vertex, container_iterator_type it);

    vertex& operator*() const;
    vertex* operator->() const;
    neighbors_iterator& operator++();
    const neighbors_iterator operator++(int offset);
    bool operator ==(const neighbors_iterator& n) const;
    bool operator !=(const neighbors_iterator& n) const;

private:
    graph& m_graph;
    vertex& m_vertex;
    container_iterator_type m_it;
};

class const_neighbors_iterator : public std::iterator<std::forward_iterator_tag, vertex, ptrdiff_t, const vertex*, const vertex&>
{
public:
    typedef std::size_t size_type;
    typedef std::vector<::graphlib::directed_edge>::const_iterator container_iterator_type;

    explicit const_neighbors_iterator(const graph& graph, const vertex& vertex, container_iterator_type it);

    const vertex& operator*() const;
    const vertex* operator->() const;
    const_neighbors_iterator& operator++();
    const const_neighbors_iterator operator++(int offset);
    bool operator ==(const const_neighbors_iterator& n) const;
    bool operator !=(const const_neighbors_iterator& n) const;

private:
    const graph& m_graph;
    const vertex& m_vertex;
    container_iterator_type m_it;
};

#endif //ALGORITHM_ENGINEERING_NEIGHBORS_ITERATOR_HPP
