//
// Created by Leon Suchy on 26.10.18.
//

#ifndef ALGORITHM_ENGINEERING_GRAPH_IO_HPP
#define ALGORITHM_ENGINEERING_GRAPH_IO_HPP

#include "graph_reader.hpp"
#include "graph_writer.hpp"
#include "edge_writer.hpp"

inline std::ostream& operator<<(std::ostream& os, const graph_data& data)
{
    graph_writer writer(os);
    writer.write(data);
    return os;
}

inline std::istream& operator>>(std::istream& is, graph_data& data)
{
    graph_reader reader(is);
    graph_data res (reader.read());
    data.vertices   = std::move(res.vertices);
    data.names      = std::move(res.names);

    return is;
}

#endif //ALGORITHM_ENGINEERING_GRAPH_IO_HPP
