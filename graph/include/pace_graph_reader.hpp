//
// Created by Leon Suchy on 2019-02-08.
//

#ifndef ALGORITHM_ENGINEERING_PACE_GRAPH_READER_HPP
#define ALGORITHM_ENGINEERING_PACE_GRAPH_READER_HPP

#include <istream>
#include <string>

#include "graph_data.hpp"
#include "open_wbo_reader.hpp"

/**
 * @brief Reader for graphs in pace format
 * @see https://pacechallenge.org/2019/vc/vc_format/
 */
class pace_graph_reader
{
public:

    struct parse_exception : public graph_parse_exception
    {
        explicit parse_exception(uint32_t line, const std::string& msg);
    };

    /**
     * @brief Check for compatible file extension
     * @param path Path to a graph file
     * @return True if the file at that path is a PACE graph
     */
    [[nodiscard]] static bool is_pace_graph_file(const std::string& path);

    /**
     * @brief Construct a graph reader on a given input stream
     * @param stream Any input stream
     */
    explicit pace_graph_reader(std::istream& stream);

    /**
     * @brief Parses the input stream for graph data
     * @return Graph data
     * @throws graph_parse_exception On error
     */
    [[nodiscard]] graph_data read();

private:

    struct header
    {
        std::string problem_descriptor;
        uint32_t num_vertices;
        uint32_t num_hyperedges;
    };

    struct header_tokens
    {
        std::string problem_descriptor;
        std::string vertex_count;
        std::string hyperedge_count;
    };

    static constexpr char COMMENT_PREFIX = 'c';
    static constexpr char HEADER_PREFIX = 'p';
    static const std::string PROBLEM_DESCRIPTOR;

    std::istream& m_stream;
    unsigned m_line;

    bool is_comment(const std::string& line) const;

    bool is_header(const std::string& line) const;

    /**
     * @brief Get the next word in a sequence
     * @param current Current position in the sequence
     * @param end Sequence end
     * @return The next word stripped of surrounding whitespace or an empty string if no word is left
     */
    std::string next_word(std::string::const_iterator& current, std::string::const_iterator end) const;

    vertex::id_type get_or_add(const std::string& name, graph_data& data) const;

    header read_header();

    header_tokens extract_tokens(const std::string& line) const;

    std::string parse_problem_descriptor_token(const std::string& problem_desc) const;

    uint32_t parse_amount_token(const std::string& token, uint16_t token_index) const;
};

#endif //ALGORITHM_ENGINEERING_PACE_GRAPH_READER_HPP
