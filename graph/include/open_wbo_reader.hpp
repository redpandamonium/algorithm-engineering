//
// Created by Carsten Schubert on 2018-12-15.
//

#ifndef ALGORITHM_ENGINEERING_OPEN_WBO_READER_HPP
#define ALGORITHM_ENGINEERING_OPEN_WBO_READER_HPP

#include <istream>
#include <stdexcept> // runtime_error

#include "vertex.hpp"
#include "graph.hpp"
#include "graph_data.hpp"

struct graph_parse_exception : std::runtime_error
{
    explicit graph_parse_exception(std::size_t line, const std::string& what);
};

/**
 * @brief Reads graph data from an input stream
 */
class open_wbo_reader
{
public:

    /**
     * @brief Construct a graph reader on a given input stream
     * @param stream Any input stream
     */
    explicit open_wbo_reader(std::istream& stream);

    /**
     * @brief Parses the input stream for graph data
     * @return Graph data
     * @throws graph_parse_exception On error
     */
    graph_data read();

private:

    static constexpr const char COMMENT_PREFIX = 'c';

    std::istream& m_stream;

    bool is_comment(const std::string& line) const;

    std::string next_word(std::string::const_iterator& current, std::string::const_iterator end) const;

    vertex::id_type add(const std::string& name, graph_data& data) const;

    int32_t m_vc_size;

};

#endif //ALGORITHM_ENGINEERING_OPEN_WBO_READER_HPP
