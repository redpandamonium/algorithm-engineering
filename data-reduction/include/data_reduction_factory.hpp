//
// Created by Leon Suchy on 12.11.18.
//

#ifndef ALGORITHM_ENGINEERING_DATA_REDUCTION_FACTORY_HPP
#define ALGORITHM_ENGINEERING_DATA_REDUCTION_FACTORY_HPP

#include <map>
#include <string>
#include <functional>
#include <memory>

#include <graph_unit.hpp>
#include "data_reduction_options.hpp"
#include "data_reductions.hpp"
#include "data_reduction_node.hpp"
#include "loop_reduction.hpp"

class data_reduction_factory
{
public:

    /**
     * @brief Empty factory. Builds nothing
     */
    data_reduction_factory() = default;

    explicit data_reduction_factory(const data_reduction_options& config);

    static std::vector<std::string> allowed_names();

    static std::vector<std::string> allowed_container_types();

    std::unique_ptr<data_reductions> build(graph_unit graph) const;

    struct required_option_error : public std::runtime_error
    {
        required_option_error(const std::string& section, const std::string& key, nlohmann::json::value_t type);
    };

private:

    struct json_data
    {
        const nlohmann::json& jsn;
        const std::string& section;
    };

    struct key
    {
        static constexpr const char* frequency = "frequency";
        static constexpr const char* min_degree = "min-degree";
        static constexpr const char* max_degree = "max-degree";
        static constexpr const char* apply_once = "apply-once";
    };

    static std::map<std::string, std::function<std::unique_ptr<data_reduction_node>(graph_unit, data_reduction_factory::json_data)>> s_factory_capabilities;
    static std::map<std::string, std::function<std::unique_ptr<data_reduction_node>(graph_unit, std::vector<std::unique_ptr<data_reduction_node>>&&, json_data)>> s_container_capabilities;

    static std::unique_ptr<data_reduction_node> build_degree_0(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_degree_1(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_2_fold(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_triangle(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_degree_greater_k(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_dominate(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_unconfined(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_neighbor_clique_partition(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_degree_3_independent_set(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_degree_4_path(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_degree_4_crossbow(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_degree_4_trebuchet(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_lp(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_loop(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_funnel(graph_unit graph, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_desk(graph_unit graph, json_data additional_data);

    static std::unique_ptr<data_reduction_node> build_container_exhaustive(graph_unit unit, std::vector<std::unique_ptr<data_reduction_node>>&& children, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_container_exhaustive_infrequent(graph_unit unit, std::vector<std::unique_ptr<data_reduction_node>>&& children, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_container_once(graph_unit unit, std::vector<std::unique_ptr<data_reduction_node>>&& children, json_data additional_data);
    static std::unique_ptr<data_reduction_node> build_container_exhaustive_iterative(graph_unit unit, std::vector<std::unique_ptr<data_reduction_node>>&& children, json_data additional_data);


    data_reduction_options m_config;

    static void require_optional_value_of_type(json_data data, const std::string& key, nlohmann::json::value_t type);
    static unsigned require_optional_unsigned_with_default(json_data data, const std::string& key, unsigned default_);
    static bool require_optional_bool_with_default(json_data data, const std::string& key, bool default_);

    std::unique_ptr<data_reduction_node> build(graph_unit graph, const data_reduction_options& current) const;
};

#endif //ALGORITHM_ENGINEERING_DATA_REDUCTION_FACTORY_HPP
