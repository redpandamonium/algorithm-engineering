//
// Created by Leon Suchy on 23.01.19.
//

#ifndef ALGORITHM_ENGINEERING_DATA_REDUCTION_CONTAINER_HPP
#define ALGORITHM_ENGINEERING_DATA_REDUCTION_CONTAINER_HPP

#include <string>
#include <vector>

#include "../data_reduction_node.hpp"
#include <graph_unit.hpp>

struct data_reduction_container : public data_reduction_node
{
public:

    data_reduction_container(graph_unit unit, std::vector<std::unique_ptr<data_reduction_node>>&& children);

    virtual ~data_reduction_container() = default;

protected:

    std::vector<std::unique_ptr<data_reduction_node>> m_children;
};

#endif //ALGORITHM_ENGINEERING_DATA_REDUCTION_CONTAINER_HPP
