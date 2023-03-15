//
// Created by Leon Suchy on 15.12.18.
//

#ifndef ALGORITHM_ENGINEERING_DATA_REDUCTION_OPTIONS_HPP
#define ALGORITHM_ENGINEERING_DATA_REDUCTION_OPTIONS_HPP

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct data_reduction_options
{
    std::string name;
    int priority;
    std::vector<data_reduction_options> children;
    nlohmann::json additional_options;
    std::string section;
};

#endif //ALGORITHM_ENGINEERING_DATA_REDUCTION_OPTIONS_HPP
