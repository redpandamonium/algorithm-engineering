//
// Created by Carsten Schubert on 2018-11-19.
//

#ifndef ALGORITHM_ENGINEERING_UNCONFINED_REDUCTION_HPP
#define ALGORITHM_ENGINEERING_UNCONFINED_REDUCTION_HPP


#include "data_reduction_node.hpp"
#include <fast_set.hpp>

class unconfined_reduction : public data_reduction_node {
public:

    explicit unconfined_reduction(graph_unit graph, unsigned min_degree, unsigned max_degree);

    std::string name() const override;

    bool apply(graph_operation::stack& op_stack) override;

    void accept(const graph_operation::operation_type& op) override;

    void reject(const graph_operation::operation_type& op) override;

private:

    bool neighbor_difference(vertex::id_type u, vertex::id_type& single);
    void update_unconfined_set(
            vertex::id_type w,
            std::vector<vertex::id_type>& unconfined_neighbors);

    void reduce_neighbors(std::vector<vertex::id_type>& neighbors, graph_operation::stack& op_stack);

    // diamond reduction
    void initialize_vertex_set(std::vector<vertex::id_type>& unconfined_neighbors);
    bool find_diamond(vertex::id_type v, std::vector<vertex::id_type>& unconfined_neighbors, graph_operation::stack& op_stack);

    // constants
    const vertex::id_type m_size = m_graph.graph().size_including_inactive();

    // config members
    unsigned m_min_degree;
    unsigned m_max_degree;
    // TODO implement
    //bool apply_once;

    // data structures
    fast_set m_set;
    std::vector<u_int64_t> m_unconfined_degrees;
    std::vector<vertex::id_type> m_temp;
};


#endif //ALGORITHM_ENGINEERING_UNCONFINED_REDUCTION_HPP
