//
// Created by Leon Suchy on 23.01.19.
//

#include <container/data_reduction_container.hpp>

data_reduction_container::data_reduction_container(
    graph_unit unit,
    std::vector<std::unique_ptr<data_reduction_node>>&& children
)
    : data_reduction_node(unit), m_children(std::move(children))
{ }
