//
// Created by Leon Suchy on 25.10.18.
//

#ifndef ALGORITHM_ENGINEERING_GRAPH_READER_HPP
#define ALGORITHM_ENGINEERING_GRAPH_READER_HPP

#include <istream>
#include <stdexcept> // runtime_error
#include <cstdint>

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
class graph_reader
{
public:

    /**
     * @brief Construct a graph reader on a given input stream
     * @param stream Any input stream
     */
    explicit graph_reader(std::istream& stream);

    /**
     * @brief Parses the input stream for graph data
     * @return Graph data
     * @throws graph_parse_exception On error
     */
    graph_data read();

private:

    static constexpr char COMMENT_PREFIX = '#';

    std::istream& m_stream;

    bool is_comment(const std::string& line) const;

    std::string next_word(std::string::const_iterator& current, std::string::const_iterator end) const;

    vertex::id_type get_or_add(const std::string& name, graph_data& data) const;

};

#endif //ALGORITHM_ENGINEERING_GRAPH_READER_HPP
