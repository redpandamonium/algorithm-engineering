//
// Created by Leon Suchy on 12.11.18.
//

#include <data_reduction_factory.hpp>

#include "degree_0_reduction.hpp"
#include "degree_1_reduction.hpp"
#include "degree_2_folding.hpp"
#include "degree_greater_k_reduction.hpp"
#include "dominate_reduction.hpp"
#include "unconfined_reduction.hpp"
#include "triangle_reduction.hpp"
#include "neighbor_clique_partition_reduction.hpp"
#include "lp_reduction.hpp"
#include "container/exhaustive_container.hpp"
#include "container/exhaustive_infrequent_container.hpp"
#include "degree_3_independent_set_reduction.hpp"
#include "degree_4_path_reduction.hpp"
#include "degree_4_crossbow_reduction.hpp"
#include "degree_4_trebuchet_reduction.hpp"
#include "funnel_reduction.hpp"
#include "desk_reduction.hpp"
#include "container/once_container.hpp"
#include "container/exhaustive_iterative_container.hpp"

data_reduction_factory::data_reduction_factory(const data_reduction_options& config)
    : m_config(config)
{ }

std::vector<std::string> data_reduction_factory::allowed_names()
{
    std::vector<std::string> names;
    names.reserve(s_factory_capabilities.size());
    for (const auto& entry : s_factory_capabilities)
    {
        names.push_back(entry.first);
    }
    return names;
}

using namespace std::literals::string_literals;

// std::function isn't noexcept, but when is this really gonna throw? Not enough space for a function pointer BEFORE the program runs?!
std::map<std::string, std::function<std::unique_ptr<data_reduction_node>(graph_unit, data_reduction_factory::json_data)>> data_reduction_factory::s_factory_capabilities = // NOLINT(cert-err58-cpp)
{
        { "degree-0"s,                  { &data_reduction_factory::build_degree_0 } },
        { "degree-1"s,                  { &data_reduction_factory::build_degree_1 } },
        { "2-fold"s,                    { &data_reduction_factory::build_2_fold } },
        { "degree > k"s,                { &data_reduction_factory::build_degree_greater_k } },
        { "dominate"s,                  { &data_reduction_factory::build_dominate } },
        { "unconfined"s,                { &data_reduction_factory::build_unconfined } },
        { "triangle"s,                  { &data_reduction_factory::build_triangle } },
        { "neighbor-clique-partition"s, { &data_reduction_factory::build_neighbor_clique_partition } },
        { "degree-3-independent-set"s,  { &data_reduction_factory::build_degree_3_independent_set } },
        { "degree-4-path"s,             { &data_reduction_factory::build_degree_4_path } },
        { "degree-4-crossbow"s,         { &data_reduction_factory::build_degree_4_crossbow } },
        { "degree-4-trebuchet"s,        { &data_reduction_factory::build_degree_4_trebuchet } },
        { "lp"s,                        { &data_reduction_factory::build_lp } },
        { "loop"s,                      { &data_reduction_factory::build_loop } },
        { "funnel"s,                    { &data_reduction_factory::build_funnel } },
        { "desk"s,                      { &data_reduction_factory::build_desk } },
};

std::map<std::string, std::function<std::unique_ptr<data_reduction_node>(graph_unit unit, std::vector<std::unique_ptr<data_reduction_node>>&& children, data_reduction_factory::json_data)>> data_reduction_factory::s_container_capabilities =
{
        { "exhaustive", { &data_reduction_factory::build_container_exhaustive } },
        { "exhaustive-infrequent", { &data_reduction_factory::build_container_exhaustive_infrequent } },
        { "once", { &data_reduction_factory::build_container_once } },
        { "exhaustive-iterative", { &data_reduction_factory::build_container_exhaustive_iterative } },
};

std::unique_ptr<data_reduction_node> data_reduction_factory::build_degree_0(graph_unit graph, json_data additional_data)
{
    return std::make_unique<degree_0_reduction>(graph);
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_degree_1(graph_unit graph, json_data additional_data)
{
    return std::make_unique<degree_1_reduction>(graph);
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_degree_greater_k(graph_unit graph, json_data additional_data)
{
    return std::make_unique<degree_greater_k_reduction>(graph);
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_unconfined(graph_unit graph, json_data additional_data)
{
    unsigned min_degree = require_optional_unsigned_with_default(additional_data, key::min_degree, std::numeric_limits<unsigned>::min());
    unsigned max_degree = require_optional_unsigned_with_default(additional_data, key::max_degree, std::numeric_limits<unsigned>::max());
    //bool apply_once = require_optional_bool_with_default(additional_data, key::apply_once, false);
    return std::unique_ptr<data_reduction_node>(new unconfined_reduction(graph, min_degree, max_degree));
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_dominate(graph_unit graph, json_data additional_data)
{
    unsigned min_degree = require_optional_unsigned_with_default(additional_data, key::min_degree, std::numeric_limits<unsigned>::min());
    unsigned max_degree = require_optional_unsigned_with_default(additional_data, key::max_degree, std::numeric_limits<unsigned>::max());
    return std::unique_ptr<data_reduction_node>(new dominate_reduction(graph, min_degree, max_degree));
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_2_fold(graph_unit graph, json_data additional_data)
{
    return std::make_unique<degree_2_folding>(graph);
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_triangle(graph_unit graph, json_data additional_data)
{
    return std::make_unique<triangle_reduction>(graph);
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_lp(graph_unit graph, json_data additional_data)
{
    return std::make_unique<lp_reduction>(graph);
}
std::unique_ptr<data_reduction_node> data_reduction_factory::build_neighbor_clique_partition(graph_unit graph, json_data additional_data)
{
    unsigned max_degree = require_optional_unsigned_with_default(additional_data, key::max_degree, std::numeric_limits<unsigned>::max());
    return std::unique_ptr<data_reduction_node>(new neighbor_clique_partition_reduction(graph, max_degree));
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_degree_3_independent_set(graph_unit graph, data_reduction_factory::json_data additional_data) {
    return std::make_unique<degree_3_independent_set_reduction>(graph);
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_degree_4_path(graph_unit graph, data_reduction_factory::json_data additional_data) {
    return std::make_unique<degree_4_path_reduction>(graph);
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_degree_4_crossbow(graph_unit graph, data_reduction_factory::json_data additional_data) {
    return std::make_unique<degree_4_crossbow_reduction>(graph);
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_degree_4_trebuchet(graph_unit graph, data_reduction_factory::json_data additional_data) {
    return std::make_unique<degree_4_trebuchet_reduction>(graph);
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_funnel(graph_unit graph, data_reduction_factory::json_data additional_data)
{
    return std::make_unique<funnel_reduction>(graph);
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_desk(graph_unit graph, data_reduction_factory::json_data additional_data)
{
    return std::make_unique<desk_reduction>(graph);
}

std::unique_ptr<data_reductions> data_reduction_factory::build(graph_unit graph) const
{

    return std::make_unique<data_reductions>(build(graph, m_config));
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build(graph_unit graph, const data_reduction_options& current) const
{
    std::vector<std::unique_ptr<data_reduction_node>> children;

    for (const data_reduction_options& child : current.children)
    {
        // leaf
        if (child.children.empty())
        {
            auto it = s_factory_capabilities.find(child.name);
            if (it == s_factory_capabilities.end())
                continue;

            children.push_back(it->second(graph, json_data { child.additional_options, child.section }));
        }
        // container
        else
        {
            children.push_back(build(graph, child));
        }
    }

    // container
    auto it = s_container_capabilities.find(current.name);
    if (it == s_container_capabilities.end())
        throw std::invalid_argument("Cannot build container '" + current.name + "'");

    std::unique_ptr<data_reduction_node> container = it->second(graph, std::move(children), json_data { current.additional_options, current.section });

    return container;
}

std::vector<std::string> data_reduction_factory::allowed_container_types()
{
    std::vector<std::string> names;
    names.reserve(s_container_capabilities.size());
    for (const auto& entry : s_container_capabilities)
    {
        names.push_back(entry.first);
    }
    return names;
}

std::unique_ptr<data_reduction_node>
data_reduction_factory::build_container_exhaustive(
    graph_unit unit,
    std::vector<std::unique_ptr<data_reduction_node>>&& children,
    json_data additional_data
)
{
    return std::unique_ptr<data_reduction_node>(new exhaustive_container(unit, std::move(children)));
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_container_exhaustive_infrequent(
    graph_unit unit,
    std::vector<std::unique_ptr<data_reduction_node>>&& children,
    json_data additional_data
)
{
    require_optional_value_of_type(additional_data, key::frequency, nlohmann::json::value_t::number_integer);
    int frequency = additional_data.jsn[key::frequency].get<int>();
    return std::unique_ptr<data_reduction_node>(new exhaustive_infrequent_container(unit, std::move(children), frequency));
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_container_exhaustive_iterative(
        graph_unit unit,
        std::vector<std::unique_ptr<data_reduction_node>> &&children,
        data_reduction_factory::json_data additional_data)
{
    require_optional_value_of_type(additional_data, key::frequency, nlohmann::json::value_t::number_integer);
    int freq = additional_data.jsn[key::frequency].get<int>();
    return std::unique_ptr<data_reduction_node>(new exhaustive_iterative_container(unit, std::move(children), freq));
}

void
data_reduction_factory::require_optional_value_of_type(
    data_reduction_factory::json_data data,
    const std::string& key,
    nlohmann::json::value_t type
)
{
    auto it = data.jsn.find(key);
    if (it == data.jsn.end())
        throw std::invalid_argument("Could not find value " + data.section + "::" + key);

    if (type == nlohmann::json::value_t::number_unsigned || type == nlohmann::json::value_t::number_integer)
    {
        if (it->type() != nlohmann::json::value_t::number_unsigned && it->type() != nlohmann::json::value_t::number_integer)
            throw required_option_error(data.section, key, type);
    }
    else
    {
        if (it->type() != type)
            throw required_option_error(data.section, key, type);
    }
}

std::unique_ptr<data_reduction_node> data_reduction_factory::build_container_once(
    graph_unit unit,
    std::vector<std::unique_ptr<data_reduction_node>>&& children,
    data_reduction_factory::json_data additional_data
)
{
    unsigned frequency = require_optional_unsigned_with_default(additional_data, key::frequency, 1);
    return std::unique_ptr<data_reduction_node>(
            new once_container(unit, std::move(children), static_cast<unsigned int>(frequency)));
}

unsigned data_reduction_factory::require_optional_unsigned_with_default(data_reduction_factory::json_data data,
                                                                    const std::string& key, unsigned default_)
{
    auto it = data.jsn.find(key);
    if (it == data.jsn.end())
        return default_;

    if (it->type() != nlohmann::json::value_t::number_unsigned && it->type() != nlohmann::json::value_t::number_integer)
        throw required_option_error(data.section, key, nlohmann::json::value_t::number_unsigned);

    int val = it->get<int>();
    if (val < 0)
        throw std::invalid_argument(data.section + "::"s + key + " cannot be less than 0");

    return static_cast<unsigned>(val);
}

bool data_reduction_factory::require_optional_bool_with_default(data_reduction_factory::json_data data,
                                                                const std::string &key, bool default_)
{
    auto it = data.jsn.find(key);
    if (it == data.jsn.end())
    {
        return default_;
    }

    if (it->type() != nlohmann::json::value_t::boolean)
        throw required_option_error(data.section, key, nlohmann::json::value_t::boolean);

    bool val = it->get<bool>();

    return val;
}

std::unique_ptr<data_reduction_node>
data_reduction_factory::build_loop(graph_unit graph, data_reduction_factory::json_data additional_data)
{
    return std::make_unique<loop_reduction>(graph);
}

data_reduction_factory::required_option_error::required_option_error(const std::string& section, const std::string& key, nlohmann::json::value_t type)
    : std::runtime_error("Option " + section + "::" + key + " does not match the required criteria")
{
}

