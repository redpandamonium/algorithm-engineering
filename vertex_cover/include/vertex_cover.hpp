//
// Created by Leon Suchy on 24.11.18.
//

#ifndef ALGORITHM_ENGINEERING_VERTEX_COVER_HPP
#define ALGORITHM_ENGINEERING_VERTEX_COVER_HPP


#include "configuration.hpp"
#include "vertex_cover_solver.hpp"

class vertex_cover
{
public:

    vertex_cover(graph& g, const configuration& config, const configuration& pre_config);

    void solve();

    int solution() const;

private:

    static constexpr const char* const path_to_config = "res/config.json";

    graph_unit_storage m_graph_unit;
    configuration m_config;
    configuration m_pre_config;
    data_reduction_factory m_reduction_factory;
    vertex_cover_solver m_solver;
    int m_solution;

    void finalize();
    [[deprecated("Use program options instead")]] configuration load_config() const;
};


#endif //ALGORITHM_ENGINEERING_VERTEX_COVER_HPP
