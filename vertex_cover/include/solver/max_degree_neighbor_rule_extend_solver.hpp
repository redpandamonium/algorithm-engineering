//
// Created by Leon Suchy on 28.10.18.
//

#ifndef ALGORITHM_ENGINEERING_MAX_DEGREE_NEIGHBOR_RULE_EXTEND_SOLVER_HPP
#define ALGORITHM_ENGINEERING_MAX_DEGREE_NEIGHBOR_RULE_EXTEND_SOLVER_HPP


#include <data_reduction_factory.hpp>
#include "k_solver.hpp"
#include "../configuration.hpp"

class max_degree_neighbor_rule_extend_solver : public k_solver
{
public:

    explicit max_degree_neighbor_rule_extend_solver(graph_unit g, const data_reduction_factory& data_reduction_factory, const configuration& config);

    bool solve_for(int k) override;

    uint32_t recursive_steps() override;

private:

    uint32_t m_recursive_steps;
    const data_reduction_factory& m_data_reduction_factory;
    const configuration& m_config;

    bool solve(int k) override;

    bool solve_step(int k);

    uint32_t best_lowerbound();

    bool kernel_edge_condition();

    bool branch_on(vertex::id_type v);

    bool branch();

    /**
     * @brief Merge the solutions found in a set of components.
     * @param subgraphs Subgraphs of G
     */
    void merge_subgraph_solutions(const std::vector<std::unique_ptr<subgraph>>& subgraphs);

    /**
     * @brief Takes v into the vertex cover and tries if there exists a MVC with v in it.
     * @param v Valid vertex
     * @return True if there exists a MVC with v in it.
     * This function might also do something equivalent to taking v. Do not assume v to be taken.
     */
    bool take_v(vertex::id_type v);

    /**
     * @brief Take v's neighbours.
     * @param v Valid vertex
     * @return True if there exists a MVC with v's neighbors.
     * This function might also do something equivalent to taking v's neighbours. Do not assume them to be taken.
     */
    bool take_v_neighbors(vertex::id_type v);

    /**
     * @brief Find a minimum vertex cover of size <= k in a component
     * @param component A subgraph of a real component.
     * @return True if the component could be solved.
     * If the subgraph contains multiple components this is undefined behaviour.
     */
    bool solve_component(subgraph& component);

    /**
     * @brief Select a vertex based on some heuristic
     * @return A vertex to branch on. Invalid iff G is empty.
     */
    vertex::id_type select_vertex() const;
};


#endif //ALGORITHM_ENGINEERING_MAX_DEGREE_NEIGHBOR_RULE_EXTEND_SOLVER_HPP
