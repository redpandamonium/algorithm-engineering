//
// Created by Leon Suchy on 26.10.18.
//

#include <algorithm>

#include "graph_writer.hpp"

graph_writer::graph_writer(std::ostream& os)
    : m_stream(os)
{ }

void graph_writer::write(const graph_data& data)
{
    for (const auto& vertex : data.vertices)
    {
        if (!vertex.valid())
            continue;

        auto it = data.names.right.find(vertex.id());
        if (it == data.names.right.end())
            throw name_lookup_error(vertex);

        m_stream << it->second << "\n";
    }
}

name_lookup_error::name_lookup_error(const vertex& v)
    : std::runtime_error("Failed to look up name of vertex " + v.string())
{ }
