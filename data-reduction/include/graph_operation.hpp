//
// Created by Leon Suchy on 12.11.18.
//

#ifndef ALGORITHM_ENGINEERING_GRAPH_OPERATION_HPP
#define ALGORITHM_ENGINEERING_GRAPH_OPERATION_HPP

#include <vector>
#include <variant>
#include <stack>

#include <vertex.hpp>

class data_reduction_node;
#include "data_reduction_node.hpp"

namespace graph_operation
{
    struct group
    {
        vertex::id_type vertex;
        void* user_data;
    };

    struct ungroup
    {
        std::vector<vertex::id_type> members;
        void* user_data;
    };

    struct include
    {
        vertex::id_type vertex;
        void* user_data;
    };

    struct exclude
    {
        vertex::id_type vertex;
        void* user_data;
    };

    struct invalidate
    {
        vertex::id_type vertex;
        void* user_data;
    };

    struct revalidate
    {
        vertex::id_type vertex;
        void* user_data;
    };

    struct edge_insert
    {
        edge e;
        void* user_data;
    };

    struct edge_remove
    {
        edge e;
        void* user_data;
    };

    struct neighborhood_clique_partition
    {
        vertex::id_type v;
        std::vector<graphlib::directed_edge> big_to_small_clique;
        unsigned amount_of_vertices_in_smaller_clique;
    };

    struct data_reduction_vertices
    {
        vertex::id_type v;
        vertex::id_type first_neighbor;
        vertex::id_type second_neighbor;
        vertex::id_type third_neighbor;
        vertex::id_type forth_neighbor;

    };

    struct folding
    {
        vertex::id_type middle;
        vertex::id_type left;
        vertex::id_type right;
    };

    struct alternative
    {
        std::vector<vertex::id_type> A;
        std::vector<vertex::id_type> B;
        // N(A) \ N[B]
        std::vector<vertex::id_type> A_B_difference;
        // N(B) \ N[A]
        std::vector<vertex::id_type> B_A_difference;
    };

    typedef std::variant<group, ungroup, include, exclude, invalidate, revalidate, edge_insert, edge_remove, neighborhood_clique_partition, data_reduction_vertices, folding, alternative> operation_type;


    class stack
    {
    public:

        struct operation
        {
            data_reduction_node& actor;
            operation_type action;
        };

        void push(operation&& op);
        void push(data_reduction_node& actor, group&& op);
        void push(data_reduction_node& actor, ungroup&& op);
        void push(data_reduction_node& actor, include&& op);
        void push(data_reduction_node& actor, exclude&& op);
        void push(data_reduction_node& actor, invalidate&& op);
        void push(data_reduction_node& actor, revalidate&& op);
        void push(data_reduction_node& actor, edge_insert&& op);
        void push(data_reduction_node& actor, edge_remove&& op);
        void push(data_reduction_node& actor, neighborhood_clique_partition&& op);
        void push(data_reduction_node& actor, data_reduction_vertices&& op);
        void push(data_reduction_node& actor, folding&& op);
        void push(data_reduction_node& actor, alternative&& op);

        operation pop();
        operation& top();
        const operation& top() const;

        size_t size() const;
        bool empty() const;

    private:

        std::stack<operation> m_operation_stack;
    };
}






#endif //ALGORITHM_ENGINEERING_GRAPH_OPERATION_HPP
