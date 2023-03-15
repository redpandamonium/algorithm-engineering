//
// Created by Carsten Schubert on 2018-11-25.
//


#include <algorithm>
#include "graph.hpp"
#include "edge_writer.hpp"

edge_writer::edge_writer(std::ostream& os)
        : m_stream(os)
{ }

void edge_writer::write(const graph_data& data)
{
    std::set<edge> edges;
    for (const auto& vertex : data.vertices)
    {
        if (!vertex.valid())
            continue;

        for (const vertex::id_type neighbor : vertex.neighbors())
        {
            edges.emplace(vertex.id(), neighbor);
        }
    }


    for (const auto& edge : edges)
    {
        auto a = data.names.right.find(edge.get_a());
        if (a == data.names.right.end())
            throw edge_name_lookup_error(edge.get_a());

        auto b = data.names.right.find(edge.get_b());
        if (b == data.names.right.end())
            throw edge_name_lookup_error(edge.get_b());

        m_stream << a->second << " " << b->second << "\n";
    }
}

edge_name_lookup_error::edge_name_lookup_error(const vertex::id_type v)
        : std::runtime_error("Failed to look up name of vertex with ID " + std::to_string(v))
{ }
