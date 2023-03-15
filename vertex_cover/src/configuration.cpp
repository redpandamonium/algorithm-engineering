//
// Created by Leon Suchy on 10.11.18.
//

#include <data_reduction_factory.hpp>
#include <configuration.hpp>
#include <util.hpp>

#include "../include/configuration.hpp"

using json = nlohmann::json;
using namespace std::literals::string_literals;

configuration::configuration()
    : m_solver("max_neighbor"),
    m_data_reductions
    {
        "exhaustive",
        0,
        {
            { "degree-0", 4, { }, { }, "" },
            { "degree-1", 3, { }, { }, "" },
            { "degree > k", 2, { }, { }, "" },
            { "triangle", 1, { }, { }, "" },
            { "2-fold", 0, { }, { }, "" }
        },
        { },
        ""
    },
    m_bounds
    {
        { "clique-cover", 1 },
        { "konig-lp", 0 }
    },
    m_log_options
    {
        true,
        true
    },
    m_kernel_edge_condition(false),
    m_max_mirror_degree(std::numeric_limits<unsigned>::max()),
    m_lower_bound_options
    {
        true, true, true, true,
        clique_cover_lb::order_options { clique_cover_lb::order::descending, clique_cover_lb::order::none }
    }
{ }

const std::string& configuration::solver()
{
    return m_solver;
}

const data_reduction_options& configuration::data_reductions() const
{
    return m_data_reductions;
}

configuration::configuration(const nlohmann::json& json)
    : configuration()
{
    read_data_reduction_options(json);
    read_bound_options(json);
    read_log_options(json);
    read_solver(json);
    read_misc_options(json);
    read_max_mirror_degree(json);
    read_lower_bounds(json);
}

const std::map<std::string, int>& configuration::bounds() const
{
    return m_bounds;
}

void configuration::read_data_reduction_options(const json& json_in)
{
    json::const_iterator data_reduction_section_it = json_in.find(key::data_reduction_section);

    // section is not present, use default
    if (data_reduction_section_it == json_in.end())
        return;

    m_data_reductions = read_data_reduction("", *data_reduction_section_it);
}

void configuration::read_bound_options(const json& json_in)
{
    json::const_iterator bounds_section_it = json_in.find(key::bounds_section);

    // section is not present, use default
    if (bounds_section_it == json_in.end())
        return;

    // don't use defaults
    m_bounds.clear();

    auto map = read_name_priority_map(*bounds_section_it, key::bounds_section);
    m_bounds = std::move(map);
    verify_bound_options();
}

std::map<std::string, int> configuration::read_name_priority_map(const nlohmann::json& map, const std::string& section_id) const
{
    if (!map.is_array())
        throw configuration_parse_exception(section_id, "Expected an array");

    std::map<std::string, int> result;

    int elem_index = 0;
    for (const json& entry : map)
    {
        auto pair = read_name_priority_map_entry(entry, section_id + "::" + std::to_string(elem_index));

        // check if priority is unambiguous
        int priority = pair.second;
        auto same_priority_it = std::find_if(
                result.begin(), result.end(),
                [priority] (auto pair) { return pair.second == priority; }
        );
        if (same_priority_it != result.end())
            throw configuration_parse_exception(section_id + "::"s + key::priority, "Duplicate priority. Ambiguous ordering.");

        bool success = result.insert(pair).second;
        if (!success)
            throw configuration_parse_exception(section_id + "::" + std::to_string(elem_index) + "::" + key::name, "Duplicate name. Cannot enable an option twice.");

        elem_index++;
    }

    return result;
}

std::pair<std::string, int> configuration::read_name_priority_map_entry(const nlohmann::json& element, const std::string& section_id) const
{
    // check format
    if (!element.is_object())
        throw configuration_parse_exception(section_id, "Map elements need to be pairs");

    // check if members are present
    json::const_iterator name_it = element.find(key::name);
    if (name_it == element.end())
        throw configuration_parse_exception(section_id, "Has no name");
    json::const_iterator priority_it = element.find(key::priority);
    if (name_it == element.end())
        throw configuration_parse_exception(section_id, "Has no priority");

    // check if types are correct
    if (!name_it->is_string())
        throw configuration_parse_exception(section_id + "::"s + key::name, "Name property is not a string");
    if (!priority_it->is_number())
        throw configuration_parse_exception(section_id + "::"s + key::priority, "Priority property is not a number");


    // read values
    auto name = element[key::name].get<std::string>();
    auto priority = element[key::priority].get<int>();

    return std::pair<std::string, int>(name, priority);
}

void configuration::verify_bound_options() const
{

}

const struct configuration::log_options& configuration::log_options() const
{
    return m_log_options;
}

void configuration::read_solver(const nlohmann::json& json)
{
    json::const_iterator solver_it = json.find(key::solver_section);

    // no solver specified, use the default
    if (solver_it == json.end())
        return;

    // invalid type
    if (!solver_it->is_string())
        throw configuration_parse_exception(key::solver_section, "Expected a string");

    m_solver = solver_it->get<std::string>();
    verify_solver_options();
}

void configuration::read_log_options(const nlohmann::json& json)
{
    json::const_iterator log_options_it = json.find(key::log_section);

    // no log options specified, use default
    if (log_options_it == json.end())
        return;

    if (!log_options_it->is_object())
        throw configuration_parse_exception(key::log_section, "Expected an object");

    // if it exists read recursive step toggle
    json::const_iterator enable_recursive_steps_it = log_options_it->find(key::recursive_steps);
    if (enable_recursive_steps_it != log_options_it->end())
    {
        if (!enable_recursive_steps_it->is_boolean())
            throw configuration_parse_exception(key::log_section + "::"s + key::recursive_steps, "Expected a bool");

        m_log_options.recursive_steps = enable_recursive_steps_it->get<bool>();
    }

    // if it exists read solution size toggle
    json::const_iterator enable_solution_size_it = log_options_it->find(key::solution_size);
    if (enable_solution_size_it != log_options_it->end())
    {
        if (!enable_solution_size_it->is_boolean())
            throw configuration_parse_exception(key::log_section + "::"s + key::solution_size, "Expected a bool");

        m_log_options.recursive_steps = enable_solution_size_it->get<bool>();
    }
}

void configuration::verify_solver_options() const
{

}

std::vector<data_reduction_options>
configuration::read_data_reduction_list(const std::string& section, const nlohmann::json& jsn) const
{
    std::vector<data_reduction_options> result;

    // check if the section exists and is an array
    auto arr_it = jsn.find(key::data_reduction_section);
    if (arr_it == jsn.end())
        throw configuration_parse_exception(section + "::" + key::data_reduction_section, "Aggregate reduction needs children. No field called '"s + key::data_reduction_section + "'.");

    nlohmann::json children = *arr_it;
    if (!children.is_array())
        throw configuration_parse_exception(section + "::" + key::data_reduction_section, "Expected an array");

    // read entries
    {
        size_t i = 0;
        for (const auto& child : children)
        {
            data_reduction_options child_reduction = read_data_reduction(
                    section + "::" + key::data_reduction_section + "::" + std::to_string(i), child);
            result.push_back(std::move(child_reduction));
            i++;
        }
    }

    // sort - high priority first
    std::sort(result.begin(), result.end(),
              [](const auto& left, const auto& right) { return left.priority > right.priority; });

    // check for uniqueness of priorities
    if (!result.empty())
    {
        int last = result[0].priority;
        for (auto i = 1; i < static_cast<int>(result.size()); ++i)
        {
            if (result[i].priority == last)
            {
                throw configuration_parse_exception
                (
                    section + "::" +
                    key::data_reduction_section + "::" +
                    std::to_string(i) + "::" +
                    key::priority,
                    "Priority needs to be unique"
                );
            }
            last = result[i].priority;
        }
    }

    return result;
}

data_reduction_options configuration::read_data_reduction(const std::string& section, const nlohmann::json& jsn) const
{
    auto name_it = jsn.find(key::name);
    auto prio_it = jsn.find(key::priority);

    if (name_it == jsn.end())
        throw configuration_parse_exception(section + "::" + key::name, "Field not present");
    if (prio_it == jsn.end())
        throw configuration_parse_exception(section + "::" + key::priority, "Field not present");

    auto name = name_it->get<std::string>();
    auto priority = prio_it->get<int>();

    data_reduction_options result { name, priority, { }, jsn, section };

    // normal case: data reduction (leaf node)
    if (!is_container_type(name))
    {
        auto allowed = data_reduction_factory::allowed_names();
        if (std::find(allowed.begin(), allowed.end(), name) == allowed.end())
            throw configuration_parse_exception(section + "::" + key::name, name + " is not a valid data reduction");

        return result;
    }

    // special case: aggregate reduction
    result.children = read_data_reduction_list(section, jsn);

    // Why? Just allow empty containers.
    // if (result.children.empty())
    //     throw configuration_parse_exception(section + "::" + key::data_reduction_section, "Cannot have aggregate reduction object without children");

    return result;
}

void configuration::read_misc_options(const nlohmann::json& json)
{
    auto kernel_edge_it = json.find(key::kernel_edge_condition);
    if (kernel_edge_it != json.end())
    {
        if (!kernel_edge_it->is_boolean())
            throw configuration_parse_exception("::"s + key::kernel_edge_condition, "Expected bool");
        m_kernel_edge_condition = kernel_edge_it->get<bool>();
    }
}

bool configuration::kernel_edge_condition_enabled() const
{
    return m_kernel_edge_condition;
}

bool configuration::is_container_type(const std::string& name) const
{
    auto names = data_reduction_factory::allowed_container_types();
    auto it = std::find(names.begin(), names.end(), name);
    return it != names.end();
}

unsigned configuration::max_mirror_degree() const
{
    return m_max_mirror_degree;
}

void configuration::read_max_mirror_degree(const nlohmann::json& json)
{
    auto max_mirror_degree_it = json.find(key::max_mirror_degree);
    if (max_mirror_degree_it != json.end())
    {
        if (!max_mirror_degree_it->is_number())
            throw configuration_parse_exception("::"s + key::max_mirror_degree, "Expected int");
        auto max_deg = max_mirror_degree_it->get<int>();
        if (max_deg < 0)
            throw configuration_parse_exception("::"s + key::max_mirror_degree, "Expected unsigned");
        m_max_mirror_degree = static_cast<unsigned>(max_deg);
    }
}

void configuration::read_lower_bounds(const nlohmann::json& json)
{
    auto section_it = json.find(key::lower_bounds);
    if (section_it != json.end())
    {
        // lp bound
        auto lp_bound_it = section_it->find(key::lp_bound);
        if (lp_bound_it != section_it->end())
        {
            if (!lp_bound_it->is_boolean())
                throw configuration_parse_exception("::"s + key::lower_bounds + "::" + key::lp_bound, "Expected bool");
            m_lower_bound_options.lp_bound = lp_bound_it->get<bool>();
        }

        // cycle cover
        auto cycle_cover_it = section_it->find(key::cycle_cover);
        if (cycle_cover_it != section_it->end())
        {
            if (!cycle_cover_it->is_boolean())
                throw configuration_parse_exception("::"s + key::lower_bounds + "::" + key::cycle_cover, "Expected bool");
            m_lower_bound_options.cycle_cover = cycle_cover_it->get<bool>();
        }

        // triangle cover
        auto triangle_cover_it = section_it->find(key::triangle_cover);
        if (triangle_cover_it != section_it->end())
        {
            if (!triangle_cover_it->is_boolean())
                throw configuration_parse_exception("::"s + key::lower_bounds + "::" + key::triangle_cover, "Expected bool");
            m_lower_bound_options.triangle_cover = triangle_cover_it->get<bool>();
        }

        // clique cover
        auto clique_cover_it = section_it->find(key::clique_cover);
        if (clique_cover_it != section_it->end())
        {
            if (!clique_cover_it->is_boolean())
                throw configuration_parse_exception("::"s + key::lower_bounds + "::" + key::clique_cover, "Expected bool");
            m_lower_bound_options.clique_cover = clique_cover_it->get<bool>();
        }

        m_lower_bound_options.clique_cover_orders = read_order_options("::"s + key::lower_bounds, *section_it);
    }
}

configuration::lower_bound_options configuration::lower_bounds() const
{
    return m_lower_bound_options;
}

clique_cover_lb::order_options
configuration::read_order_options(const std::string& section, const nlohmann::json& jsn) const
{
    clique_cover_lb::order_options options { clique_cover_lb::order::descending, clique_cover_lb::order::none };

    auto iteration_order_it = jsn.find(key::iteration_order);
    auto neighbor_iteration_order_it = jsn.find(key::neighbor_iteration_order);

    if (iteration_order_it != jsn.end())
    {
        if (!iteration_order_it->is_string())
            throw configuration_parse_exception(section + "::" + key::iteration_order, "Required string");

        options.iteration_order = clique_cover_order_from_string(iteration_order_it->get<std::string>());
    }

    if (neighbor_iteration_order_it != jsn.end())
    {
        if (!neighbor_iteration_order_it->is_string())
            throw configuration_parse_exception(section + "::" + key::iteration_order, "Required string");

        options.neighbor_iteration_order = clique_cover_order_from_string(neighbor_iteration_order_it->get<std::string>());
    }

    return options;
}

clique_cover_lb::order configuration::clique_cover_order_from_string(const std::string& name) const
{
    if (util::equals_ignore_case("ascending", name))
        return clique_cover_lb::order::ascending;
    if (util::equals_ignore_case("descending", name))
        return clique_cover_lb::order::descending;

    return clique_cover_lb::order::none;
}

configuration_parse_exception::configuration_parse_exception(const std::string& section, const std::string& message)
: runtime_error("Failed to parse json at " + section + ": " + message)
{ }
