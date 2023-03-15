//
// Created by Leon Suchy on 14.12.18.
//

#include <algorithm>
#include <random_graph.hpp>

#include "random_graph.hpp"

generated_graph random_graph_generator::generate(random_graph_generator::seed_type seed)
{
    m_engine.seed(seed);
    std::uniform_int_distribution<engine_type::result_type> graph_size_distribution(1, m_max_vertices);
    engine_type::result_type graph_size = graph_size_distribution(m_engine);
    std::uniform_int_distribution<engine_type::result_type> neighbor_count_distribution(0, graph_size - 1);

    generated_graph result { graph { graph_size }, seed };

    std::vector<vertex::id_type> vertices;

    // add vertices
    for (engine_type::result_type i = 0; i < graph_size; i++)
    {
        vertices.push_back(result.graph_.add_vertex());
    }

    // add edges
    for (std::size_t i = 0; i < graph_size; i++)
    {
        vertex::id_type self = vertices[i];

        // create vector of all vertices except for self
        std::vector<vertex::id_type> other_vertices(vertices);
        other_vertices[i] = other_vertices.back();
        other_vertices.pop_back();

        engine_type::result_type num_edges = neighbor_count_distribution(m_engine);

        // remove all neighbors and decrease the remaining required edges
        auto new_end = std::remove_if(other_vertices.begin(), other_vertices.end(),
                [&result, self] (auto v) {
                    auto neighbors = std::move(result.graph_[v].neighbors());
                    return std::find(neighbors.begin(), neighbors.end(), self) != neighbors.end();
        });
        auto distance = std::distance(new_end, other_vertices.end());
        num_edges = static_cast<unsigned int>(num_edges < distance ? 0 : num_edges - distance); // unsigned must not overflow

        // shuffle to select random edges
        std::shuffle(other_vertices.begin(), new_end, m_engine);

        // pick as many edges as required
        for (std::size_t j = 0; j < num_edges; ++j)
        {
            result.graph_.add_edge(vertices[i], other_vertices[j]);
        }
    }

    return result;
}

random_graph_generator::random_graph_generator(unsigned max_vertices)
    : m_engine(), m_max_vertices(max_vertices)
{

}
