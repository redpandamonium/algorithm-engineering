//
// Created by Leon Suchy on 25.10.18.
//

#include <cctype> // isspace
#include <utility> // make_pair

#include "graph_reader.hpp"

graph_reader::graph_reader(std::istream& stream)
    : m_stream(stream)
{ }

graph_data graph_reader::read()
{
    using namespace std::literals::string_literals;

    graph_data data { };

    std::string line;
    std::size_t line_number = 0;
    std::ios_base::sync_with_stdio(false);
    while (std::getline(m_stream, line))
    {
        line_number++;
        // ignore comments
        if (is_comment(line))
            continue;

        std::string::const_iterator it = line.begin();
        auto first_vertex = next_word(it, line.end());
        auto second_vertex = next_word(it, line.end());

        // verify both vertices exist
        if (first_vertex.empty())
            throw (graph_parse_exception(line_number, "No first vertex found"));
        if (second_vertex.empty())
            throw (graph_parse_exception(line_number, "No second vertex found"));

        // no more data after the second vertex
        if (!next_word(it, line.end()).empty())
            throw (graph_parse_exception(line_number, "Line does not end after the second vertex"));

        // loop
        //if (first_vertex == second_vertex)
        //    continue;

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

bool graph_reader::is_comment(const std::string& line) const
{
    for (char c : line)
    {
        if (!std::isspace(c))
            return c == COMMENT_PREFIX;
    }

    return true; // empty line counts as a comment
}

std::string graph_reader::next_word(std::string::const_iterator& current, std::string::const_iterator end) const
{
    // scoll past white space before the word
    while (current != end && std::isspace(*current))
    {
        current++;
    }

    std::string::const_iterator begin = current;

    // scroll past the current word
    while (current != end && !std::isspace(*current))
    {
        current++;
    }

    // avoid undefined behaviour on invalid iterators
    if (begin == end)
        return "";

    return std::string(begin, current);
}

vertex::id_type graph_reader::get_or_add(const std::string& name, graph_data& data) const
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