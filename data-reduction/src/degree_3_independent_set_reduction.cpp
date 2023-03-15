//
// Created by Carsten Schubert on 2019-01-24.
//

#include <utility>


#include <util.hpp>

#include "degree_3_independent_set_reduction.hpp"

std::string degree_3_independent_set_reduction::name() const {
    return "degree-3-independent-set";
}

bool degree_3_independent_set_reduction::apply(graph_operation::stack &op_stack) {
    bool graph_changed = false;

    for (auto vertex_it = m_graph.graph().cbegin(); vertex_it != m_graph.graph().cend(); ++vertex_it)
    {
        class vertex vertex = *vertex_it;
        if (vertex.degree() != 3)
            continue;

        const auto neighbors = vertex.neighbors();
        class vertex first_neighbor = m_graph.graph()[neighbors[0]];
        class vertex second_neighbor = m_graph.graph()[neighbors[1]];
        class vertex third_neighbor = m_graph.graph()[neighbors[2]];


        if (first_neighbor.get_incident_edge_to(second_neighbor) ||
            first_neighbor.get_incident_edge_to(third_neighbor)  ||
            third_neighbor.get_incident_edge_to(second_neighbor))
            continue;

        // requirements clear now, beginning procedure...
        graph_changed = true;

        op_stack.push(*this, graph_operation::data_reduction_vertices{ vertex.id(), first_neighbor.id(), second_neighbor.id(), third_neighbor.id(), vertex::invalid_id });
        m_graph.controller().invalidate(vertex.id());

        const auto first_neighbor_neighbors  = first_neighbor.neighbors();
        const auto second_neighbor_neighbors = second_neighbor.neighbors();
        const auto third_neighbor_neighbors  = third_neighbor.neighbors();

        op_stack.push(*this, graph_operation::edge_insert{ edge{first_neighbor.id(), second_neighbor.id()}, nullptr });
        op_stack.push(*this, graph_operation::edge_insert{ edge{second_neighbor.id(), third_neighbor.id()}, nullptr });
        m_graph.controller().add_edge(first_neighbor.id(), second_neighbor.id());
        m_graph.controller().add_edge(second_neighbor.id(), third_neighbor.id());

        // now connecting according to keikaku
        connect_to_them(op_stack, first_neighbor.id(), second_neighbor_neighbors);
        connect_to_them(op_stack, second_neighbor.id(), third_neighbor_neighbors);
        connect_to_them(op_stack, third_neighbor.id(), first_neighbor_neighbors);
    }
    return graph_changed;
}

void degree_3_independent_set_reduction::connect_to_them(graph_operation::stack &op_stack, vertex::id_type vertex,
                                                         const std::vector<vertex::id_type> &set) {
    for (vertex::id_type current : set)
    {
        if (!m_graph.graph()[vertex].get_incident_edge_to(m_graph.graph()[current])){
            m_graph.controller().add_edge(vertex, current);
            op_stack.push(*this, graph_operation::edge_insert{edge{vertex, current}, nullptr });
        }
    }
}


void degree_3_independent_set_reduction::accept(const graph_operation::operation_type &op) {
    if (std::holds_alternative<graph_operation::edge_insert>(op))
    {
        const auto& insert = std::get<graph_operation::edge_insert>(op);
        m_graph.controller().remove_edge(insert.e.get_a(), insert.e.get_b());
    }
    else if (std::holds_alternative<graph_operation::data_reduction_vertices>(op))
    {
        graph_operation::data_reduction_vertices data = std::get<graph_operation::data_reduction_vertices>(op);
        vertex::id_type v            = data.v;
        class vertex first_neighbor  = m_graph.graph()[data.first_neighbor];
        class vertex second_neighbor = m_graph.graph()[data.second_neighbor];
        class vertex third_neighbor  = m_graph.graph()[data.third_neighbor];

        // all 3 were chosen
        if (util::is_in_vertex_cover(first_neighbor) && util::is_in_vertex_cover(second_neighbor) && util::is_in_vertex_cover(third_neighbor))
        {
        }
        // 2 and 3 were chosen
        else if (!util::is_in_vertex_cover(first_neighbor) && util::is_in_vertex_cover(second_neighbor) && util::is_in_vertex_cover(third_neighbor))
        {
            m_graph.controller().revalidate(v);
            m_graph.controller().include_into_vertex_cover(v);
            m_graph.controller().exclude_from_vertex_cover(second_neighbor.id());

        }
        // 1 and 3 were chosen
        else if (util::is_in_vertex_cover(first_neighbor) && !util::is_in_vertex_cover(second_neighbor) && util::is_in_vertex_cover(third_neighbor))
        {
            m_graph.controller().revalidate(v);
            m_graph.controller().include_into_vertex_cover(v);
            m_graph.controller().exclude_from_vertex_cover(third_neighbor.id());
        }
        // 1 and 2 were chosen
        else if (util::is_in_vertex_cover(first_neighbor) && util::is_in_vertex_cover(second_neighbor) && !util::is_in_vertex_cover(third_neighbor))
        {
            m_graph.controller().revalidate(v);
            m_graph.controller().include_into_vertex_cover(v);
            m_graph.controller().exclude_from_vertex_cover(first_neighbor.id());
        }
        // only one of them was chosen
        else if (util::is_in_vertex_cover(first_neighbor) || util::is_in_vertex_cover(second_neighbor) || util::is_in_vertex_cover(third_neighbor))
        {
            m_graph.controller().revalidate(v);
            m_graph.controller().include_into_vertex_cover(v);
            m_graph.controller().exclude_from_vertex_cover(first_neighbor.id());
            m_graph.controller().exclude_from_vertex_cover(second_neighbor.id());
            m_graph.controller().exclude_from_vertex_cover(third_neighbor.id());

        }
        else
            throw std::logic_error("You managed to topdeck a case that should not exist. Don't let fear overcome you!");
    }
    else
        throw std::logic_error("Cannot undo operation, only handles edges and degree 3 IS");
}

void degree_3_independent_set_reduction::reject(const graph_operation::operation_type &op) {
    if (std::holds_alternative<graph_operation::edge_insert>(op))
    {
        const auto& insert = std::get<graph_operation::edge_insert>(op);
        m_graph.controller().remove_edge(insert.e.get_a(), insert.e.get_b());
    }
    else if (std::holds_alternative<graph_operation::data_reduction_vertices>(op))
    {
        graph_operation::data_reduction_vertices data = std::get<graph_operation::data_reduction_vertices>(op);
        auto v = data.v;
        m_graph.controller().revalidate(v);
    }
    else
        throw std::logic_error("Cannot undo operation, only handles edges and degree 3 IS");

}

degree_3_independent_set_reduction::degree_3_independent_set_reduction(graph_unit graph) : data_reduction_node(graph) {}
