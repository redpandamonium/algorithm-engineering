//
// Created by Leon Suchy on 26.10.18.
//

#ifndef ALGORITHM_ENGINEERING_GRAPH_DATA_HPP
#define ALGORITHM_ENGINEERING_GRAPH_DATA_HPP

#include <vector>
#include <string>
#include <map>

#include <boost/bimap.hpp>

#include "vertex.hpp"

struct graph_data
{
    typedef boost::bimap<std::string, vertex::id_type> map_type;
    std::vector<vertex> vertices;
    map_type names;
};

#endif //ALGORITHM_ENGINEERING_GRAPH_DATA_HPP
