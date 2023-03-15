//
// Created by Carsten Schubert on 2018-11-24.
//

#include "../include/mirrors.hpp"
#include "util.hpp"


// Getting distance-2-neighbours as a set

std::vector<vertex::id_type> mirrors::get_distance_two_neighbors(const vertex::id_type v,
                                                                                     const std::vector<vertex::id_type> &v_neighbors) const {
    std::vector<vertex::id_type> distance_two_neighbors;

    constexpr const vertex::color_type NEIGHBOR_MARK = 0x2;     // N[v]
    constexpr const vertex::color_type DIST_TWO_MARK = 0x4;     // N²(v)


    m_g_unit.controller().mark(v, NEIGHBOR_MARK);
    for (const auto neighbor : v_neighbors)
        m_g_unit.controller().mark(neighbor, NEIGHBOR_MARK);

    for (const auto neighbor : v_neighbors){        // may the vertex and its mirror share an edge?
        auto next_neighbors = m_g_unit.graph()[neighbor].neighbors();
        for (auto next_neighbor : next_neighbors){
            if (! util::is_marked(m_g_unit.graph(), next_neighbor, NEIGHBOR_MARK) &&
            ! util::is_marked(m_g_unit.graph(), next_neighbor, DIST_TWO_MARK)){

                m_g_unit.controller().mark(next_neighbor, DIST_TWO_MARK);
                distance_two_neighbors.emplace_back(next_neighbor);
            }
        }
    }

    m_g_unit.controller().unmark(v, NEIGHBOR_MARK);
    for (const auto neighbor : v_neighbors)
        m_g_unit.controller().unmark(neighbor, NEIGHBOR_MARK);

    for (const auto u : distance_two_neighbors)
        m_g_unit.controller().unmark(u, DIST_TWO_MARK);

    return distance_two_neighbors;
}

// Checking if a distance-2-neighbour is a mirror

bool mirrors::validate_mirror(vertex::id_type potential_mirror,
                                                     const std::vector<vertex::id_type> &v_neighbors, std::map<std::pair<vertex::id_type, vertex::id_type>, edge_status>& adjacency_matrix) const {

    std::vector<vertex::id_type> clique;
    for (auto neighbor : v_neighbors){
        if (! m_g_unit.graph().has_edge(neighbor, potential_mirror)){
            // checking if new vertex ('neighbor') harms the clique; if not: add it to the clique
            for (auto clique_member : clique){
                auto entry = adjacency_matrix.find(std::make_pair(neighbor, clique_member));
                if (entry == adjacency_matrix.end()) {

                    // If it is not in the table already, it should be!
                    if (m_g_unit.graph().has_edge(neighbor, clique_member)){
                        adjacency_matrix.emplace(std::make_pair(clique_member, neighbor), edge_status::True);
                        adjacency_matrix.emplace(std::make_pair(neighbor, clique_member), edge_status::True);
                    } else{
                        adjacency_matrix.emplace(std::make_pair(clique_member, neighbor), edge_status::False);
                        adjacency_matrix.emplace(std::make_pair(neighbor, clique_member), edge_status::False);
                        return false;
                    }
                }
                else if (entry->second == edge_status::False){
                    return false;
                }
            }
            clique.emplace_back(neighbor);
        }
    }
    return true;
}

// Searching for mirrors

std::vector<vertex::id_type> mirrors::find_mirrors(vertex::id_type v){

    const std::vector<vertex::id_type> v_neighbors = m_g_unit.graph()[v].neighbors();
    const auto distance_two_neighbors = get_distance_two_neighbors(v, v_neighbors);

    // Building adjacency matrix as map for dynamic programming

    std::map<std::pair<vertex::id_type, vertex::id_type>, edge_status> adjacency_matrix;

    // Now searching for mirrors
    std::vector<vertex::id_type> mirrors;
    mirrors.emplace_back(v);
    for (auto potential_mirror : distance_two_neighbors){
        if (validate_mirror(potential_mirror, v_neighbors, adjacency_matrix))
            mirrors.emplace_back(potential_mirror);
    }

    return mirrors;
}

mirrors::mirrors(graph_unit g_unit)
: m_g_unit(g_unit)
{}

