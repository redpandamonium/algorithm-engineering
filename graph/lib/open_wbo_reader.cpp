//
// Created by Carsten Schubert on 2018-12-15.
//

#include <cctype> // isspace
#include <utility> // make_pair
#include <cstdlib>

#include "open_wbo_reader.hpp"

open_wbo_reader::open_wbo_reader(std::istream& stream)
        : m_stream(stream), m_vc_size(-1)
{ }

graph_data open_wbo_reader::read()
{
    using namespace std::literals::string_literals;

    graph_data data { };

    std::string line;
    std::size_t line_number = 0;
    while (std::getline(m_stream, line))
    {
        line_number++;
        // ignore comments
        if (is_comment(line))
            continue;

        std::string::const_iterator it = line.begin();
        auto line_identifier = next_word(it, line.end());

        if (line_identifier == "o"){
            m_vc_size = std::atoi(next_word(it, line.end()).c_str());
            continue;
        }
        else if (line_identifier == "s"){
            auto word = next_word(it, line.end());
            if (word != "OPTIMUM")
                throw (graph_parse_exception(line_number, "Expected optimal solution, but got " + word + " instead"));
            continue;
        }
        else if (line_identifier == "v"){
            for (auto vertex_name = next_word(it, line.end()); !vertex_name.empty(); vertex_name = next_word(it, line.end())){
                //ignore vertices not in the cover
                if (vertex_name[0] == '-')
                    continue;
                else{
                    // insert the vertices if necessary
                    if (add(vertex_name, data) == vertex::invalid_id)
                        throw std::runtime_error("Failed to add vertex in OpenWBO file");
                }
            }
        }

    }

    return data;
}

bool open_wbo_reader::is_comment(const std::string& line) const
{
    for (char c : line)
    {
        if (!std::isspace(c))
            return c == COMMENT_PREFIX;
    }

    return true; // empty line counts as a comment
}

std::string open_wbo_reader::next_word(std::string::const_iterator& current, std::string::const_iterator end) const
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

vertex::id_type open_wbo_reader::add(const std::string& name, graph_data& data) const
{
    // name converted back --> -1
    auto new_name = std::to_string(std::atoi(name.c_str()) - 1);

    // insert because it isn't known yet
    auto index = data.vertices.size();

    data.names.insert(graph_data::map_type::value_type(new_name, index));
    data.vertices.emplace_back((unsigned int) index);
    data.vertices[index].color(vertex::color_true);
    return index;
}

graph_parse_exception::graph_parse_exception(std::size_t line, const std::string& what) : runtime_error("Failed to parse graph. Error on line " + std::to_string(line) + ": " + what) { }
