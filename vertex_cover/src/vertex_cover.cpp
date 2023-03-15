//
// Created by Leon Suchy on 24.11.18.
//

#include <iostream>
#include <fstream>
#include <edge_writer.hpp>
#include "../include/vertex_cover.hpp"


vertex_cover::vertex_cover(graph& g, const configuration& config, const configuration& pre_config)
    : m_graph_unit(g, graph_meta_data::options { false, false, false }), m_config(config), m_pre_config(pre_config), m_reduction_factory(m_config.data_reductions()),
      m_solver(graph_unit(*m_graph_unit.g, m_graph_unit.meta_data, m_graph_unit.controller), m_config, m_reduction_factory),
      m_solution(0)
{ }

void vertex_cover::solve()
{
    auto vertices_before = m_graph_unit.g->size();
    auto edges_before = m_graph_unit.g->num_edges();
    std::cout << "# graph size before (vertices, edges): " << vertices_before << ", " << edges_before << std::endl;

    m_graph_unit.meta_data.set_options(graph_meta_data::options { false, false, false });
    m_graph_unit.meta_data.rebuild();

    m_graph_unit.controller.set_vertex_cover_size(static_cast<int> (m_graph_unit.g->size()));

    data_reduction_factory loop_reduction_factory (data_reduction_options { "once", 0, { { "loop", 0, { }, { }, "" } }, { }, "" });
    auto loop_reduction = loop_reduction_factory.build(m_graph_unit.unit);
    loop_reduction->apply();

    data_reduction_factory initial_data_reduction_factory{m_pre_config.data_reductions()};
    auto reductions = initial_data_reduction_factory.build(m_graph_unit.unit);
    reductions->apply();
    auto vertices_after = m_graph_unit.g->size();
    auto edges_after = m_graph_unit.g->num_edges();

    std::cout << "# graph size after (vertices, edges): " << vertices_after << ", " << edges_after << std::endl;
    std::cout << "# taken in pre-reduction: " << m_graph_unit.g->get_vertices_with_color(vertex::color_true).size() << std::endl;

    //edge_writer w(std::cout);
    //w.write(m_graph_unit.g->data());

    m_graph_unit.meta_data.set_options(graph_meta_data::options { true, false, false });
    m_graph_unit.meta_data.rebuild();

    m_solution = m_solver.solve() - 1;

    if (m_config.log_options().recursive_steps)
        std::cout << "# recursive steps: " << m_solver.recursive_steps() << std::endl;

    reductions->accept();
    loop_reduction->accept();

    finalize();
}

void vertex_cover::finalize()
{
    auto mvc = m_graph_unit.g->get_vertices_with_color(static_cast<vertex::color_type>(vertex::color_true));
    for (auto v : mvc) {
        (*m_graph_unit.g)[v].revalidate();
    }
    auto is = m_graph_unit.g->get_vertices_with_color(static_cast<vertex::color_type>(vertex::color_false));
    for (auto v : is) {
        (*m_graph_unit.g)[v].invalidate();
    }
}

configuration vertex_cover::load_config() const
{
    std::ifstream stream(path_to_config);
    if (stream.good())
    {
        stream.seekg(0, stream.end);
        auto file_size = stream.tellg();
        if (file_size < 0)
            return configuration();

        stream.seekg(0, stream.beg);

        std::vector<std::string::value_type> buffer;
        buffer.resize(static_cast<unsigned long>(file_size)); // cast is ok since we checked if file_size < 0
        stream.read(&buffer[0], file_size);

        try
        {
            nlohmann::json json = nlohmann::json::parse(buffer.begin(), buffer.end());
            configuration config { json };
            return config;
        }
        catch (const nlohmann::json::parse_error& ex)
        {
            std::cerr << "# Failed to parse config: " << ex.what() << ". Using default config." << std::endl;
            return configuration();
        }
    }

    return configuration();
}

int vertex_cover::solution() const
{
    return m_solution;
}
