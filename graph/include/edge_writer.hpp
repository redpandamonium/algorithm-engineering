//
// Created by Carsten Schubert on 2018-11-25.
//

#ifndef ALGORITHM_ENGINEERING_EDGE_WRITER_HPP
#define ALGORITHM_ENGINEERING_EDGE_WRITER_HPP

#include <ostream>
#include <stdexcept>
#include <string>

#include "graph_data.hpp"

struct edge_name_lookup_error : public std::runtime_error
{
    explicit edge_name_lookup_error(vertex::id_type v);
};

class edge_writer
{
public:

    explicit edge_writer(std::ostream& os);

    // prints all edges between valid vertices in data to the stream
    void write(const graph_data& data);

private:

    std::ostream& m_stream;

};

#endif //ALGORITHM_ENGINEERING_EDGE_WRITER_HPP
