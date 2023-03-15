//
// Created by Leon Suchy on 2019-02-09.
//

#ifndef ALGORITHM_ENGINEERING_PACE_SOLUTION_WRITER_HPP
#define ALGORITHM_ENGINEERING_PACE_SOLUTION_WRITER_HPP

#include <ostream>

#include "graph_data.hpp"

/**
 * @brief Writes a vertex cover solution in PACE format
 * @see https://pacechallenge.org/2019/vc/vc_format/
 */
class pace_solution_writer
{
public:

    struct name_lookup_exception : public std::runtime_error
    {
        explicit name_lookup_exception(vertex::id_type id);
    };

    struct exception : public std::runtime_error
    {
        explicit exception(const std::string& msg);
    };

    explicit pace_solution_writer(std::ostream& os);

    void write(const graph_data& data);

private:

    std::ostream& m_stream;

    std::vector<vertex::id_type> read_result(const graph_data& data) const;

    void print_header(uint32_t num_vertices, uint32_t solution_size);
};

#endif //ALGORITHM_ENGINEERING_PACE_SOLUTION_WRITER_HPP
