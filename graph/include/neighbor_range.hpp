//
// Created by synis on 3/2/19.
//

#ifndef ALGORITHM_ENGINEERING_NEIGHBOR_RANGE_HPP
#define ALGORITHM_ENGINEERING_NEIGHBOR_RANGE_HPP

#include "neighbors_iterator.hpp"

class neighbor_range {
public:
    typedef const_neighbors_iterator const_iterator_type;

    explicit neighbor_range(const graph& graph, vertex::id_type vertex);

    const_iterator_type cbegin() const;
    const_iterator_type cend() const;

    const_iterator_type begin() const;
    const_iterator_type end() const;

private:

    const graph& m_graph;
    const vertex& m_vertex;
};


#endif //ALGORITHM_ENGINEERING_NEIGHBOR_RANGE_HPP
