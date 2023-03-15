//
// Created by Leon Suchy on 21.10.18.
//

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#include <graph.hpp>
#include <graph_io.hpp>
#include <fstream>
#include "vertex_cover.hpp"

boost::optional<configuration> load_config(const std::string path)
{
    if (!boost::filesystem::exists(boost::filesystem::path(path)))
    {
        std::cerr << "# Config location at '" << path << "' does not exist." << std::endl;
        std::cout << "# Using default config" << std::endl;
        return boost::optional<configuration>();
    }

    std::ifstream stream(path);
    if (stream.bad())
    {
        std::cerr << "# Bad stream. Failed to load config at '" << path << "'." << std::endl;
        std::cerr << "# Using default config" << std::endl;
        return boost::optional<configuration>();
    }

    stream.seekg(0, stream.end);
    auto file_size = stream.tellg();
    if (file_size < 0)
    {
        std::cout << "# Using default config" << std::endl;
        return boost::optional<configuration>();
    }

    stream.seekg(0, stream.beg);

    std::vector<std::string::value_type> buffer;
    buffer.resize(static_cast<unsigned long>(file_size)); // cast is ok since we checked if file_size < 0
    stream.read(&buffer[0], file_size);

    try
    {
        nlohmann::json json = nlohmann::json::parse(buffer.begin(), buffer.end());
        return boost::optional<configuration>(configuration { json });
    }
    catch (const nlohmann::json::parse_error& ex)
    {
        std::cerr << "# Failed to parse config: " << ex.what() << ". Using default config." << std::endl;
        std::cout << "# Using default config" << std::endl;
    }

    return boost::optional<configuration>();
}

boost::optional<std::fstream> prepare_graph_input(const std::string& path)
{
    if (!boost::filesystem::exists(boost::filesystem::path(path)))
    {
        std::cerr << "# File does not exist: '" << path << "'." << std::endl;
        exit(1);
    }

    std::fstream fstr { path,  static_cast<std::ios::openmode>(std::fstream::beg | std::fstream::in) };

    if (fstr.bad())
    {
        fstr.close();
        std::cerr << "# Couldn't read file: '" <<  path << "'." << std::endl;
        return boost::optional<std::fstream>();
    }

    return boost::optional<std::fstream>(std::move(fstr));
}

int main (int ac, char** av)
{
    namespace po = boost::program_options;

    bool no_solution_dump = false;

    po::options_description desc("Options");
    desc.add_options()
    ("file,f", po::value<std::string>(), "Set graph file")
    ("config,c", po::value<std::string>(), "Set config file")
    ("pre-config,p", po::value<std::string>(), "Set pre-processing config file")
    ("no-solution-dump", po::bool_switch(&no_solution_dump), "Disable solution printing")
    ("help,h", "Show help page");

    po::variables_map map;
    po::store(po::parse_command_line(ac, av, desc), map);
    po::notify(map);

    graph_data input_data;

    if (map.count("help"))
    {
        std::cout << desc << std::endl;
        return 0;
    }

    boost::optional<std::fstream> graph_input;

    if (map.count("file"))
    {
        std::string path = map["file"].as<std::string>();
        graph_input = prepare_graph_input(path);
    }

    if (graph_input)
        (*graph_input) >> input_data;
    else
        std::cin >> input_data;

    graph G (std::move(input_data));
    boost::optional<configuration> config;

    if (map.count("config"))
    {
        std::string path = map["config"].as<std::string>();
        config = load_config(path);
    }

    boost::optional<configuration> pre_config;
    if (map.count("pre-config"))
    {
        std::string path = map["pre-config"].as<std::string>();
        pre_config = load_config(path);
    }

    if (config && pre_config)
    {
        vertex_cover vc(G, *config, *pre_config);
        vc.solve();

        if (config->log_options().solution_size)
            std::cout << "# solution size: " << G.get_vertices_with_color(vertex::color_true).size() << std::endl;

        if (!no_solution_dump)
        {
            std::ios::sync_with_stdio(false);
            std::cout << G.data();
        }
    }
    else
    {
        std::cerr << "# missing program arguments for configs" << std::endl;
        std::cerr << desc << std::endl;
    }
}