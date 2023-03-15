//
// Created by synis on 1/19/19.
//

#ifndef ALGORITHM_ENGINEERING_NEIGHBOR_CLIQUE_PARTITION_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_NEIGHBOR_CLIQUE_PARTITION_REDUCTION_HPP


#include "data_reduction_node.hpp"
#include "fast_set.hpp"

class neighbor_clique_partition_reduction : public data_reduction_node {
public:

    explicit neighbor_clique_partition_reduction(graph_unit graph, unsigned max_degree);

    std::string name() const override;

    bool apply(graph_operation::stack &op_stack) override;

    void accept(const graph_operation::operation_type &op) override;

    void reject(const graph_operation::operation_type &op) override;

private:

    static constexpr const vertex::color_type MARK = 0x2;

    struct handle_current_neighbor_result
    {
        bool continuing;
        unsigned taken_into_smaller_clique;
    };

    std::vector<vertex::id_type> get_anti_neighbors(const std::vector<vertex::id_type> &neighbors, vertex::id_type current);

    handle_current_neighbor_result handle_current_neighbor(
        std::vector<vertex::id_type> &neighbors,
        vertex::id_type current, std::vector<graphlib::directed_edge>& clique_relation,
        graph_operation::stack &op_stack
    );

    unsigned m_max_degree;
    fast_set m_fast_set;
    fast_set m_marking_set;

    void connect_to_them(graph_operation::stack &op_stack, vertex::id_type vertex,
                                                              const std::vector<vertex::id_type> &set);


    void reduce(graph_operation::stack &op_stack, const vertex &vertex,
                std::vector<graphlib::directed_edge> &clique_relation, unsigned int smaller_clique_size);

    handle_current_neighbor_result
    handle_anti_degree_one_neighbor(vertex::id_type current, std::vector<graphlib::directed_edge> &clique_relation,
                                    const std::vector<vertex::id_type> &anti_edge_list);

    handle_current_neighbor_result
    handle_multiple_anti_degree_neighbor(const std::vector<vertex::id_type> &neighbors, vertex::id_type current,
                                         std::vector<graphlib::directed_edge> &clique_relation,
                                         const std::vector<vertex::id_type> &anti_edge_list);
};


#endif //ALGORITHM_ENGINEERING_NEIGHBOR_CLIQUE_PARTITION_REDUCTION_HPP
