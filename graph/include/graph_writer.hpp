//
// Created by Leon Suchy on 26.10.18.
//

#ifndef ALGORITHM_ENGINEERING_GRAPH_WRITER_HPP
#define ALGORITHM_ENGINEERING_GRAPH_WRITER_HPP

#include <ostream>
#include <stdexcept>

#include "graph_data.hpp"

struct name_lookup_error : public std::runtime_error
{
    explicit name_lookup_error(const vertex& v);
};

class graph_writer
{
public:

    explicit graph_writer(std::ostream& os);

    void write(const graph_data& data);

private:

    std::ostream& m_stream;

};

#endif //ALGORITHM_ENGINEERING_GRAPH_WRITER_HPP
