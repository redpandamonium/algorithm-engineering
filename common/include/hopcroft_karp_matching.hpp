//
// Created by iarch on 11/10/18.
//

#ifndef ALGORITHM_ENGINEERING_HOPCROFT_KARP_MATCHING_HPP
#define ALGORITHM_ENGINEERING_HOPCROFT_KARP_MATCHING_HPP

#include <vector>
#include <graph.hpp>

class hopcroft_karp_matching {
public:

    explicit hopcroft_karp_matching(graph& g);
    void init();
    void update();
    std::vector<edge> calculate_matching();
    void on_edge_remove(const edge& e);

    const std::map<vertex::id_type, vertex::id_type>& left_right_mapping() const;
    const std::map<vertex::id_type, vertex::id_type>& right_left_mapping() const;

    const std::vector<vertex::id_type>& left_right() const;
    const std::vector<vertex::id_type>& right_left() const;
    const std::vector<u_int32_t> distances() const;

private:

    graph& m_graph;

    bool bfs();
    bool dfs(vertex::id_type left);

    std::map<vertex::id_type, vertex::id_type> m_left_right;
    std::map<vertex::id_type, vertex::id_type> m_right_left;

    std::vector<vertex::id_type> m_in;
    std::vector<vertex::id_type> m_out;
    std::vector<u_int32_t> m_dist;

    static constexpr const vertex::id_type nil = vertex::invalid_id;
    static constexpr const uint32_t infinity = std::numeric_limits<uint32_t>::max();
    static constexpr const vertex::color_type VISITED_LEFT = 0x4;
    static constexpr const vertex::color_type VISITED_RIGHT = 0x8;
};


#endif //ALGORITHM_ENGINEERING_HOPCROFT_KARP_MATCHING_HPP
