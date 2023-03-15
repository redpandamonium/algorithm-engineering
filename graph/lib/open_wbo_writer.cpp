//
// Created by Carsten Schubert on 2018-12-15.
//


#include <algorithm>
#include "graph.hpp"
#include "open_wbo_writer.hpp"
#include <edge_writer.hpp>
#include <string>
#include <cstdlib>

open_wbo_writer::open_wbo_writer(std::ostream& os)
        : m_stream(os)
{ }

void open_wbo_writer::write(const graph_data& data) {
    std::set<edge> edges;
    for (const auto &vertex : data.vertices) {
        if (!vertex.valid())
            continue;

        for (const vertex::id_type neighbor : vertex.neighbors()) {
            edges.emplace(vertex.id(), neighbor);
        }
    }

    // print starting line first
    auto n = data.vertices.size();
    auto top = n + 1;
    auto m = edges.size();
    m_stream << "p wcnf " << n << " " << n + m << " " << top << "\n";

    // print edge clauses
    for (const auto &edge : edges) {
        auto a = data.names.right.find(edge.get_a());
        if (a == data.names.right.end())
            throw edge_name_lookup_error(edge.get_a());

        auto b = data.names.right.find(edge.get_b());
        if (b == data.names.right.end())
            throw edge_name_lookup_error(edge.get_b());

        m_stream << top << " " << a->first + 1 << " " << b->first + 1 << " 0\n";
    }

    // print vertex clauses
    for (const auto &vertex : data.vertices) {
        if (!vertex.valid())
            continue;
        else{
            auto v_name = data.names.right.find(vertex.id());
            if (v_name == data.names.right.end())
                throw edge_name_lookup_error(vertex.id());

            m_stream << "1" << " -" << v_name->first + 1 << " 0\n";
        }
    }
}