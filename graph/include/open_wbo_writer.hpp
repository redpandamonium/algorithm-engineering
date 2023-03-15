//
// Created by Carsten Schubert on 2018-12-15.
//

#ifndef ALGORITHM_ENGINEERING_OPEN_WBO_WRITER_HPP
#define ALGORITHM_ENGINEERING_OPEN_WBO_WRITER_HPP

#include <ostream>
#include <stdexcept>
#include <string>

#include "graph_data.hpp"

class open_wbo_writer
{
public:

    explicit open_wbo_writer(std::ostream& os);

    // transforms our graph into an instance for open-WBO MAX-SAT solver that is printed to the ostream
    void write(const graph_data& data);

private:

    std::ostream& m_stream;

};

#endif //ALGORITHM_ENGINEERING_OPEN_WBO_WRITER_HPP
