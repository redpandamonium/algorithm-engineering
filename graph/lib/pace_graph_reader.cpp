//
// Created by Leon Suchy on 2019-02-08.
//

#include <pace_graph_reader.hpp>
#include <algorithm>

using std::string_literals::operator""s;

const std::string pace_graph_reader::PROBLEM_DESCRIPTOR = "td";

bool pace_graph_reader::is_pace_graph_file(const std::string& path)
{
    // find last dot
    auto last_dot_it = std::find(path.rbegin(), path.rend(), '.');
    if (last_dot_it == path.rend())
        return false; // no file extension

    if (last_dot_it.base() == path.begin() + 1)
        return false; // hidden file with no extension

    // create file extension substring
    std::string extension(last_dot_it.base(), path.end());
    return extension == "gr" || extension == "hgr";
}

bool pace_graph_reader::is_comment(const std::string& line) const
{
    std::string::size_type i = 0;
    for (; i < line.size(); ++i)
    {
        if (!std::isspace(line[i]))
            break;
    }

    // empty lines count as comments
    if (i == line.size())
        return true;

    return line[i] == COMMENT_PREFIX;
}

std::string pace_graph_reader::next_word(std::string::const_iterator& current, std::string::const_iterator end) const
{
    // scoll past white space before the word
    while (current != end && std::isspace(*current))
        ++current;

    std::string::const_iterator begin = current;

    // scroll past the current word
    while (current != end && !std::isspace(*current))
        ++current;

    // avoid undefined behaviour on invalid iterators
    if (begin == end)
        return "";

    return std::string(begin, current);
}

vertex::id_type pace_graph_reader::get_or_add(const std::string& name, graph_data& data) const
{
    auto it = data.names.left.find(name);

    // vertex already contained
    if (it != data.names.left.end())
        return it->second;

    // insert because it isn't known yet
    auto index = data.vertices.size();

    data.names.insert(graph_data::map_type::value_type(name, index));
    data.vertices.emplace_back((unsigned int) index);

    return index;
}

pace_graph_reader::pace_graph_reader(std::istream& stream)
    : m_stream(stream), m_line(0)
{ }

graph_data pace_graph_reader::read()
{
    graph_data data { };

    header head = read_header();

    std::string line;
    std::ios_base::sync_with_stdio(false);
    while (std::getline(m_stream, line))
    {
        m_line++;
        // ignore comments
        if (is_comment(line))
            continue;

        // header needs to be unique
        if (is_header(line))
            throw parse_exception(m_line, "Header must be unique");

        std::string::const_iterator it = line.begin();
        auto first_vertex = next_word(it, line.end());
        auto second_vertex = next_word(it, line.end());

        // verify both vertices exist
        if (first_vertex.empty())
            throw (graph_parse_exception(m_line, "No first vertex found"));
        if (second_vertex.empty())
            throw (graph_parse_exception(m_line, "No second vertex found"));

        // no more data after the second vertex
        if (!next_word(it, line.end()).empty())
            throw (graph_parse_exception(m_line, "Line does not end after the second vertex"));

        // insert the vertices if necessary
        auto vec1 = get_or_add(first_vertex, data);
        auto vec2 = get_or_add(second_vertex, data);

        // insert the edge
        data.vertices[vec1].add_edge_to(data.vertices[vec2]);
        data.vertices[vec2].add_edge_to(data.vertices[vec1]);
    }

    std::ios_base::sync_with_stdio(true);

    return data;
}

bool pace_graph_reader::is_header(const std::string& line) const
{
    std::string::size_type i = 0;
    for (; i < line.size(); ++i)
    {
        if (!std::isspace(line[i]))
            break;
    }

    if (i == line.size())
        return false;

    return line[i] == HEADER_PREFIX;
}

pace_graph_reader::header pace_graph_reader::read_header()
{
    header head;
    std::string line;
    bool head_found = false;

    while (std::getline(m_stream, line))
    {
        m_line++;

        // ignore comments
        if (is_comment(line))
            continue;

        // first non-commenting line is required to be a header
        if (!is_header(line))
            throw parse_exception(m_line, "Expected header on first content line");

        // extract tokens
        header_tokens tokens = extract_tokens(line);

        // parse tokens
        head.problem_descriptor = parse_problem_descriptor_token(tokens.problem_descriptor);
        head.num_vertices       = parse_amount_token(tokens.vertex_count, 3);
        head.num_hyperedges     = parse_amount_token(tokens.hyperedge_count, 4);

        head_found = true;
        break; // found header and parsed correctly
    }

    if (!head_found)
        throw parse_exception(m_line, "Expected header");

    return head;
}

pace_graph_reader::header_tokens pace_graph_reader::extract_tokens(const std::string& line) const
{
    header_tokens tokens;

    std::string::const_iterator it = line.begin();
    next_word(it, line.end()); // discard content identifier

    // extract tokens
    tokens.problem_descriptor = next_word(it, line.end());
    tokens.vertex_count = next_word(it, line.end());
    tokens.hyperedge_count = next_word(it, line.end());

    // no extra tokens
    if (!next_word(it, line.end()).empty())
        throw parse_exception(m_line, "Too many tokens in header");

    // tokens need to exist
    if (tokens.problem_descriptor.empty() || tokens.vertex_count.empty() || tokens.hyperedge_count.empty())
        throw parse_exception(m_line, "Too few tokens in header");

    return tokens;
}

std::string pace_graph_reader::parse_problem_descriptor_token(const std::string& problem_desc) const
{
    if (problem_desc != PROBLEM_DESCRIPTOR)
        throw parse_exception(m_line, "Unknown problem descriptor token in header");

    return problem_desc;
}

uint32_t pace_graph_reader::parse_amount_token(const std::string& token, uint16_t token_index) const
{
    char* validation = nullptr;
    long val = std::strtol(token.c_str(), &validation, 10);
    if (!validation)
        throw parse_exception(m_line, "Expected number token in header at index "s + std::to_string(token_index));
    if (val < 0)
        throw parse_exception(m_line, "Expected unsigned number in header at index "s + std::to_string(token_index));

    return static_cast<uint32_t>(val);
}

pace_graph_reader::parse_exception::parse_exception(uint32_t line, const std::string& msg)
    : graph_parse_exception(line, msg)
{ }
