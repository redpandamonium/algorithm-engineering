//
// Created by synis on 1/19/19.
//
#include <utility>


#include <util.hpp>
#include <iostream>

#include "neighbor_clique_partition_reduction.hpp"

std::string neighbor_clique_partition_reduction::name() const {
    return "neighbor-clique-partition";
}


std::vector<vertex::id_type> neighbor_clique_partition_reduction::get_anti_neighbors
    (const std::vector<vertex::id_type> &neighbors, const vertex::id_type current)
{
    // pls review, can this be faster somehow?
    auto current_neighbors = m_graph.graph()[current].neighbors();
    current_neighbors.emplace_back(current);
    std::vector<vertex::id_type> anti_edge_list;

    // Set Difference: Neighbors \ Current_Neighbors
    for (vertex::id_type current_neighbor : current_neighbors)
    {
        if (m_fast_set.is_member(current_neighbor))
            m_fast_set.remove(current_neighbor);
    }
    for (vertex::id_type original_neighbor : neighbors)
    {
        if (m_fast_set.is_member(original_neighbor))
            anti_edge_list.emplace_back(original_neighbor);
        else
            m_fast_set.add(original_neighbor);
    }

    return anti_edge_list;
}


bool neighbor_clique_partition_reduction::apply(graph_operation::stack &op_stack) 
{
    bool graph_changed = false;
    for (auto vertex_it = m_graph.graph().cbegin(); vertex_it != m_graph.graph().cend(); ++vertex_it)
    {
        const vertex &vertex = *vertex_it;
        if (m_graph.meta_data().vertex_cover_size() <= 0)   // do we still need this?
            return false;
        if (vertex.degree() > m_max_degree)
            continue;

        std::vector<graphlib::directed_edge> clique_relation;
        auto neighbors = vertex.neighbors();
        for (vertex::id_type neighbor : neighbors)
            m_fast_set.add(neighbor);

        bool abort = false;
        unsigned smaller_clique_size = 0;

        for (auto current : neighbors) 
        {
            handle_current_neighbor_result res = handle_current_neighbor(neighbors, current, clique_relation, op_stack);
            smaller_clique_size += res.taken_into_smaller_clique;
            if (!res.continuing)
            {
                abort = true;
                break;
            }
        }

        // If we did find the cliques:
        if (!abort && !clique_relation.empty()) 
        {
            reduce(op_stack, vertex, clique_relation, smaller_clique_size);
            graph_changed = true;
        }

        // cleaning up
        m_marking_set.clear();
        m_fast_set.clear();
    }
    return graph_changed;
}

void neighbor_clique_partition_reduction::connect_to_them(graph_operation::stack &op_stack, vertex::id_type vertex,
                                                   const std::vector<vertex::id_type> &set)
{
    for (auto& current : set)
    {
        if (current == vertex)
            continue;
        if (!m_graph.graph()[vertex].get_incident_edge_to(m_graph.graph()[current]))
        {
            m_graph.controller().add_edge(vertex, current);
            op_stack.push(*this, graph_operation::edge_insert{edge{vertex, current}, nullptr });
        }
    }
}


void neighbor_clique_partition_reduction::reduce(graph_operation::stack& op_stack, const vertex& vertex,
                                                 std::vector<graphlib::directed_edge>& clique_relation,
                                                 unsigned int smaller_clique_size)
{
    for (graphlib::directed_edge &relation : clique_relation)
    {
        vertex::id_type greater_clique_member_id = relation.get_from();
        vertex::id_type smaller_clique_member_id = relation.get_to();
        auto smaller_clique_member_neighbors = m_graph.graph()[smaller_clique_member_id].neighbors();
        connect_to_them(op_stack, greater_clique_member_id, smaller_clique_member_neighbors);
    }

    op_stack.push(*this, graph_operation::neighborhood_clique_partition{ vertex.id(), clique_relation, smaller_clique_size });
    
    // invalidating smaller clique
    for (graphlib::directed_edge &relation : clique_relation)
    {
        vertex::id_type smaller_clique_member_id = relation.get_to();
        m_graph.controller().invalidate(smaller_clique_member_id);
    }
    
    // decreasing VC size and invalidating initial vertex
    m_graph.controller().decrease_vertex_cover_size(smaller_clique_size);
    m_graph.controller().invalidate(vertex.id());
}


neighbor_clique_partition_reduction::handle_current_neighbor_result
neighbor_clique_partition_reduction::handle_current_neighbor
    (std::vector<vertex::id_type> &neighbors,
     vertex::id_type current,
     std::vector<graphlib::directed_edge> &clique_relation,
     graph_operation::stack &op_stack) 
{
    std::vector<vertex::id_type> anti_edge_list = get_anti_neighbors(neighbors, current);
    // Connected to all others, dominates origin
    // This happens when dominate wasn't correctly applied before
    if (anti_edge_list.empty())
        // do not continue, and no vertex taken into smaller clique
        return {false, 0};
    
    else if (anti_edge_list.size() == 1)
        return handle_anti_degree_one_neighbor(current, clique_relation, anti_edge_list);

    else
        return handle_multiple_anti_degree_neighbor(neighbors, current, clique_relation, anti_edge_list);

    throw std::logic_error("There seems to be an unhandled case that should not occur (neighbor clique partition)");
}

neighbor_clique_partition_reduction::handle_current_neighbor_result
neighbor_clique_partition_reduction::handle_anti_degree_one_neighbor(vertex::id_type current,
                                                                     std::vector<graphlib::directed_edge> &clique_relation,
                                                                     const std::vector<vertex::id_type> &anti_edge_list)
{
    // case in which other vertex is also anti-degree 1
    if (m_marking_set.is_member(anti_edge_list[0]))
    {
        clique_relation.emplace_back(current, anti_edge_list[0]);
        // continue, and take a vertex into smaller clique
        return {true, 1};
    }
    else
    {
        m_marking_set.add(current);
        // continue, but do not take vertex yet
        return {true, 0};
    }
}

neighbor_clique_partition_reduction::handle_current_neighbor_result
neighbor_clique_partition_reduction::handle_multiple_anti_degree_neighbor(
    const std::vector<vertex::id_type> &neighbors,
    vertex::id_type current,
    std::vector<graphlib::directed_edge> &clique_relation,
    const std::vector<vertex::id_type> &anti_edge_list)
{
    for (auto anti_neighbor : anti_edge_list)
    {
        auto next_anti_neighbors = get_anti_neighbors(neighbors, anti_neighbor);
        if (next_anti_neighbors.size() != 1)
            return {false, 0};
            // do not continue, and no vertex taken into smaller clique
    }

    for (auto anti_neighbor : anti_edge_list)
        clique_relation.emplace_back(anti_neighbor, current);
    // continue, and take a vertex into smaller clique
    return {true, 1};
}

void neighbor_clique_partition_reduction::accept(const graph_operation::operation_type &op)
{
    if (std::holds_alternative<graph_operation::edge_insert>(op)) 
    {
        const auto &insert = std::get<graph_operation::edge_insert>(op);
        m_graph.controller().remove_edge(insert.e.get_a(), insert.e.get_b());
    } 
    else if (std::holds_alternative<graph_operation::neighborhood_clique_partition>(op)) 
    {
        const auto &partition_data = std::get<graph_operation::neighborhood_clique_partition>(op);
        bool using_v = false; // tells if the original vertex has to be in VC
        vertex::id_type untaken_smaller_clique_member = vertex::invalid_id;
        const std::vector<graphlib::directed_edge>& relations = partition_data.big_to_small_clique;

        for (const graphlib::directed_edge& relation : relations)
        {
            graphlib::directed_edge::vertex_type bigger_clique_member_id = relation.get_from();
            graphlib::directed_edge::vertex_type smaller_clique_member_id = relation.get_to();
            m_graph.controller().revalidate(smaller_clique_member_id);

            if (util::is_in_vertex_cover(m_graph.graph(), bigger_clique_member_id))
            {
                if (smaller_clique_member_id != untaken_smaller_clique_member)
                    m_graph.controller().include_into_vertex_cover(smaller_clique_member_id);
            } 
            else 
            {
                if (using_v)
                    throw std::logic_error("Something went tragically wrong: This was a clique and now there are 2 of its vertices outside VC");

                using_v = true;
                m_graph.controller().exclude_from_vertex_cover(smaller_clique_member_id);
                untaken_smaller_clique_member = smaller_clique_member_id;
            }
        }

        m_graph.controller().revalidate(partition_data.v);
        if (using_v)
            m_graph.controller().include_into_vertex_cover(partition_data.v);

        // re-increasing
        m_graph.controller().increase_vertex_cover_size(partition_data.amount_of_vertices_in_smaller_clique);

    } else
        throw std::logic_error("Cannot undo operation, only handles edges and neighborhood clique partitions");
}

void neighbor_clique_partition_reduction::reject(const graph_operation::operation_type &op) 
{
    if (std::holds_alternative<graph_operation::edge_insert>(op)) 
    {
        const auto &insert = std::get<graph_operation::edge_insert>(op);
        m_graph.controller().remove_edge(insert.e.get_a(), insert.e.get_b());
    } 
    else if (std::holds_alternative<graph_operation::neighborhood_clique_partition>(op)) 
    {
        const auto &partition_data = std::get<graph_operation::neighborhood_clique_partition>(op);
        for (const graphlib::directed_edge &relation : partition_data.big_to_small_clique)
            m_graph.controller().revalidate(relation.get_to());
        
        m_graph.controller().revalidate(partition_data.v);
        // re-increasing
        m_graph.controller().increase_vertex_cover_size(static_cast<unsigned int>
            (m_graph.graph()[partition_data.v].neighbors().size() - partition_data.big_to_small_clique.size()));
    } else
        throw std::logic_error("Cannot undo operation, only handles edges and neighborhood clique partitions");

}

neighbor_clique_partition_reduction::neighbor_clique_partition_reduction(graph_unit graph, unsigned max_degree) :
    data_reduction_node(graph),
    m_max_degree(max_degree), m_fast_set(graph.graph().size_including_inactive()), m_marking_set(graph.graph().size_including_inactive()) {}

