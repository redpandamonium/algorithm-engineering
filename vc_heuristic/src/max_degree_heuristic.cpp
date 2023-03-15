//
// Created by iarch on 12/15/18.
//

#include <iostream>
#include "max_degree_heuristic.hpp"
#include <util.hpp>
#include <max_degree_heuristic.hpp>


void max_degree_heuristic::compute_vertex_cover_heuristic()
{
    while (!m_graph.meta_data().max_heap().empty())
    {
        auto vertex = m_graph.meta_data().max_heap().top();

        if (m_graph.graph()[vertex].valid())
        {
            if (m_graph.graph()[vertex].degree() == 0) {
                m_graph.controller().invalidate(vertex);
                continue;
            }

            m_graph.controller().mark(vertex, taken);
            m_graph.controller().invalidate(vertex);

            auto neighbors = std::move(m_graph.graph()[vertex].neighbors());

            // look for easy to take / delete vertices in the neighbourhood of the taken vertex
            for (auto it = neighbors.begin(); it != neighbors.end(); it++)
            {
                auto neighbor = *it;

                if(m_graph.graph()[neighbor].degree() == 0)
                {
                    m_graph.controller().invalidate(neighbor);
                    continue;
                }

                // deg1 means "potentially degree 1". we check this in while head
                auto deg1 = m_graph.graph()[neighbor].id();

                while(m_graph.graph()[deg1].degree() == 1)
                {
                    m_graph.controller().invalidate(deg1);

                    // deg1 has only 1 neighbor
                    auto deg1_neighbor = m_graph.graph()[deg1].neighbors()[0];

                    // neighbour of deg 1 vertex could have degree 1 too
                    if (m_graph.graph()[deg1_neighbor].degree() == 1)
                    {
                        deg1 = m_graph.graph()[deg1_neighbor].neighbors()[0];
                    }
                    else if (m_graph.graph()[deg1_neighbor].degree() == 0)// neighbour of deg 1 vertex is now isolated
                    {
                        m_graph.controller().mark(deg1_neighbor, taken);
                        m_graph.controller().invalidate(deg1_neighbor);
                        break;
                    }

                    m_graph.controller().mark(deg1_neighbor, taken);
                    m_graph.controller().invalidate(deg1_neighbor);
                }
            }
        }
    }
}

max_degree_heuristic::max_degree_heuristic(graph_unit g)
    : m_graph(g)
{

}
