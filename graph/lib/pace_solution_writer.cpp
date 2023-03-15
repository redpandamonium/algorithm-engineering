//
// Created by Leon Suchy on 2019-02-09.
//

#include <pace_solution_writer.hpp>

pace_solution_writer::pace_solution_writer(std::ostream& os)
    : m_stream(os)
{ }

void pace_solution_writer::write(const graph_data& data)
{
    auto output = read_result(data);

    print_header(data.vertices.size(), output.size());

    for (auto v : output)
        m_stream << v << '\n';

    m_stream.flush();
}

std::vector<vertex::id_type> pace_solution_writer::read_result(const graph_data& data) const
{
    std::vector<vertex::id_type> output;

    for (const auto& vertex : data.vertices)
    {
        if (!vertex.valid())
            continue;

        auto it = data.names.right.find(vertex.id());
        if (it == data.names.right.end())
            throw name_lookup_exception(vertex.id());

        // names are guaranteed numbers
        char* validation = nullptr;
        long num = std::strtol(it->second.c_str(), &validation, 10);
        if (!validation)
            throw exception("Error with vertex " + std::to_string(vertex.id()) + ": Original name '" + it->second + "' is NaN");
        if (num < 0)
            throw exception("Error with vertex " + std::to_string(vertex.id()) + ": Original name '" + it->second + "' is negative");

        output.push_back(static_cast<vertex::id_type>(num));
    }
    std::sort(output.begin(), output.end());

    return output;
}

void pace_solution_writer::print_header(uint32_t num_vertices, uint32_t solution_size)
{
    m_stream << "s vc " << num_vertices << " " << solution_size << '\n';
}

pace_solution_writer::name_lookup_exception::name_lookup_exception(vertex::id_type id)
    : std::runtime_error("Failed to look up the name for vertex " + std::to_string(id))
{ }

pace_solution_writer::exception::exception(const std::string& msg)
    : std::runtime_error(msg)
{ }
