//
// Created by Carsten Schubert on 2018-11-24.
//

#ifndef ALGORITHM_ENGINEERING_MIRRORS_HPP
#define ALGORITHM_ENGINEERING_MIRRORS_HPP

#include <graph_unit.hpp>
#include <vector>
#include <map>
#include <vertex.hpp>

typedef enum {
    False = 0,
    True = 1,
    Undefined = -1
} edge_status;


class mirrors {
public:

    explicit mirrors(graph_unit g_unit);

    std::vector<vertex::id_type> find_mirrors(vertex::id_type vertex);


private:

    graph_unit m_g_unit;

    bool validate_mirror(vertex::id_type potential_mirror,
                         const std::vector<vertex::id_type> &v_neighbors,
                         std::map<std::pair<vertex::id_type, vertex::id_type>, edge_status>& adjacency_matrix) const;

    std::vector<vertex::id_type> get_distance_two_neighbors(vertex::id_type v,
                                                         const std::vector<vertex::id_type> &v_neighbors) const;

};


#endif //ALGORITHM_ENGINEERING_MIRRORS_HPP
