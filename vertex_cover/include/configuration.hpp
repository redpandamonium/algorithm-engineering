//
// Created by Leon Suchy on 10.11.18.
//

#ifndef ALGORITHM_ENGINEERING_CONFIGURATION_HPP
#define ALGORITHM_ENGINEERING_CONFIGURATION_HPP

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include <bounds/clique_cover_lb.hpp>
#include <data_reduction_options.hpp>

class configuration
{
public:

    struct log_options
    {
        bool recursive_steps;
        bool solution_size;
    };

    struct lower_bound_options
    {
        bool triangle_cover;
        bool cycle_cover;
        bool clique_cover;
        bool lp_bound;

        clique_cover_lb::order_options clique_cover_orders;
    };

    /**
     * @brief Default configuration
     */
    configuration();

    /**
     * @brief Load json config
     * @param json JSON
     */
    explicit configuration(const nlohmann::json& json);

    [[nodiscard]] const std::string& solver();

    [[nodiscard]] const data_reduction_options& data_reductions() const;

    [[nodiscard]] const std::map<std::string, int>& bounds() const;

    [[nodiscard]] const log_options& log_options() const;

    [[nodiscard]] bool kernel_edge_condition_enabled() const;

    /**
     * @brief Setting for the max mirror degree
     * @return The maximum degree where mirror branching is applied
     */
    [[nodiscard]] unsigned max_mirror_degree() const;

    [[nodiscard]] lower_bound_options lower_bounds() const;

private:

    struct key
    {
        static constexpr const char* const log_section = "log";
        static constexpr const char* const data_reduction_section = "data-reductions";
        static constexpr const char* const solver_section = "solver";
        static constexpr const char* const bounds_section = "bounds";
        static constexpr const char* const name = "name";
        static constexpr const char* const priority = "priority";
        static constexpr const char* const recursive_steps = "recursive-steps";
        static constexpr const char* const solution_size = "solution-size";
        static constexpr const char* const kernel_edge_condition = "kernel-edge-condition";
        static constexpr const char* const max_mirror_degree = "max-mirror-degree";
        static constexpr const char* const lower_bounds = "lower-bound";
        static constexpr const char* const cycle_cover = "cycle-cover";
        static constexpr const char* const triangle_cover = "triangle-cover";
        static constexpr const char* const clique_cover = "clique-cover";
        static constexpr const char* const lp_bound = "lp-bound";
        static constexpr const char* const iteration_order = "iteration-order";
        static constexpr const char* const neighbor_iteration_order = "neighbor-iteration-order";
    };

    std::string m_solver;
    data_reduction_options m_data_reductions;
    std::map<std::string, int> m_bounds;
    struct log_options m_log_options;
    bool m_kernel_edge_condition;
    unsigned m_max_mirror_degree;
    lower_bound_options m_lower_bound_options;

    data_reduction_options read_data_reduction(const std::string& section, const nlohmann::json& jsn) const;

    std::vector<data_reduction_options> read_data_reduction_list(const std::string& section, const nlohmann::json& jsn) const;

    void read_data_reduction_options(const nlohmann::json& json);

    void read_bound_options(const nlohmann::json& json);

    void read_solver(const nlohmann::json& json);

    void read_log_options(const nlohmann::json& json);

    void read_misc_options(const nlohmann::json& json);

    void read_max_mirror_degree(const nlohmann::json& json);

    void read_lower_bounds(const nlohmann::json& json);

    clique_cover_lb::order_options read_order_options(const std::string& section, const nlohmann::json& jsn) const;

    /**
     * @brief Read a map of names and priorities from JSON
     * @param map JSON input
     * @param section_id ID string of the map section
     * @return A map of all names and priorities
     * @note Names and priorities need to be unique
     * @throws configuration_parse_exception If the JSON is malformed
     */
    std::map<std::string, int>  read_name_priority_map(const nlohmann::json& map, const std::string& section_id) const;

    /**
     * @brief Reads a single element from a property map
     * @param element JSON representation of the element
     * @param section_id ID string of the element JSON
     * @return A pair of the name and priority <name, priority>
     * @throws configuration_parse_exception If the JSON is malformed
     */
    std::pair<std::string, int> read_name_priority_map_entry(const nlohmann::json& element, const std::string& section_id) const;

    /**
     * @brief Verify the consistency of the bound options read
     */
    void verify_bound_options() const;

    /**
     * Verify the validity of the solver options
     */
    void verify_solver_options() const;

    bool is_container_type(const std::string& name) const;

    clique_cover_lb::order clique_cover_order_from_string(const std::string& name) const;
};

struct configuration_parse_exception : public std::runtime_error
{
    configuration_parse_exception(const std::string& section, const std::string& message);
};


#endif //ALGORITHM_ENGINEERING_CONFIGURATION_HPP
