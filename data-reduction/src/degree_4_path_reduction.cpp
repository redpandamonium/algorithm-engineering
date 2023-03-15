//
// Created by Carsten Schubert on 2019-01-28.
//

#include <utility>

#include <util.hpp>


#include "degree_4_path_reduction.hpp"


std::string degree_4_path_reduction::name() const {
    return "degree-4-path";
}

bool degree_4_path_reduction::apply(graph_operation::stack &op_stack) {
    bool graph_changed = false;

    for (auto vertex_it = m_graph.graph().cbegin(); vertex_it != m_graph.graph().cend(); ++vertex_it)
    {
        class vertex vertex = *vertex_it;
        if (vertex.degree() != 4)
            continue;

        const auto neighbors = vertex.neighbors();

        // 'degree' in the induced path
        std::vector<vertex::id_type> degree_1_vertices;
        std::vector<vertex::id_type> degree_2_vertices;

        // checks if it is a path and begins assigning the vertices
        for (vertex::id_type v : neighbors){
            unsigned deg = count_edges_in_subgraph(v, neighbors);
            if (deg == 1)
                degree_1_vertices.emplace_back(v);
            if (deg == 2)
                degree_2_vertices.emplace_back(v);
        }
        if (degree_1_vertices.size() != 2 || degree_2_vertices.size() != 2)
            continue;

        class vertex first_neighbor  = m_graph.graph()[degree_1_vertices[0]];
        class vertex forth_neighbor  = m_graph.graph()[degree_1_vertices[1]];
        class vertex second_neighbor = m_graph.graph()[degree_2_vertices[0]].get_incident_edge_to(first_neighbor)
                                     ? m_graph.graph()[degree_2_vertices[0]] : m_graph.graph()[degree_2_vertices[1]];
        class vertex third_neighbor  = m_graph.graph()[degree_2_vertices[0]].get_incident_edge_to(forth_neighbor)
                                     ? m_graph.graph()[degree_2_vertices[0]] : m_graph.graph()[degree_2_vertices[1]];

        // requirements clear now, beginning procedure...
        graph_changed = true;

        op_stack.push(*this, graph_operation::data_reduction_vertices{ vertex.id(), first_neighbor.id(), second_neighbor.id(), third_neighbor.id(), forth_neighbor.id() });
        m_graph.controller().invalidate(vertex.id());

        const auto first_neighbor_neighbors  = first_neighbor.neighbors();
        const auto second_neighbor_neighbors = second_neighbor.neighbors();
        const auto third_neighbor_neighbors  = third_neighbor.neighbors();
        const auto forth_neighbor_neighbors  = forth_neighbor.neighbors();

        // making it a clique

        op_stack.push(*this, graph_operation::edge_insert{ edge{first_neighbor.id(), third_neighbor.id()}, nullptr });
        op_stack.push(*this, graph_operation::edge_insert{ edge{first_neighbor.id(), forth_neighbor.id()}, nullptr });
        op_stack.push(*this, graph_operation::edge_insert{ edge{second_neighbor.id(), forth_neighbor.id()}, nullptr });
        m_graph.controller().add_edge(first_neighbor.id(), third_neighbor.id());
        m_graph.controller().add_edge(first_neighbor.id(), forth_neighbor.id());
        m_graph.controller().add_edge(second_neighbor.id(), forth_neighbor.id());

        // now connecting according to keikaku
        connect_to_them(op_stack, first_neighbor.id(), forth_neighbor_neighbors);
        connect_to_them(op_stack, second_neighbor.id(), forth_neighbor_neighbors);
        connect_to_them(op_stack, third_neighbor.id(), first_neighbor_neighbors);
        connect_to_them(op_stack, forth_neighbor.id(), first_neighbor_neighbors);

    }
    return graph_changed;
}

void degree_4_path_reduction::connect_to_them(graph_operation::stack &op_stack, vertex::id_type vertex,
                                                         const std::vector<vertex::id_type> &set)
{
    for (vertex::id_type current : set)
    {
        if (vertex != current && !m_graph.graph()[vertex].get_incident_edge_to(m_graph.graph()[current])){
            m_graph.controller().add_edge(vertex, current);
            op_stack.push(*this, graph_operation::edge_insert{edge{vertex, current}, nullptr });
        }
    }
}


bool degree_4_path_reduction::in_vc(vertex v) {
    return util::is_in_vertex_cover(v);
}

unsigned degree_4_path_reduction::count_edges_in_subgraph(const vertex::id_type v, const std::vector<vertex::id_type>& subgraph)
{
    unsigned res = 0;
    for (vertex::id_type n : subgraph)
    {
        if (v == n)
            continue;
        else if (m_graph.graph()[v].get_incident_edge_to(m_graph.graph()[n]))
            res++;
    }
    return res;
}

void degree_4_path_reduction::accept(const graph_operation::operation_type &op) {
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
        class vertex forth_neighbor  = m_graph.graph()[data.forth_neighbor];

        // all 4 were chosen
        if (in_vc(first_neighbor) && in_vc(second_neighbor) && in_vc(third_neighbor) && in_vc(forth_neighbor))
        {
        }
        // all but the first were chosen
        else if(!in_vc(first_neighbor) && in_vc(second_neighbor) && in_vc(third_neighbor) && in_vc(forth_neighbor))
        {
            m_graph.controller().exclude_from_vertex_cover(forth_neighbor.id());
            m_graph.controller().revalidate(v);
            m_graph.controller().include_into_vertex_cover(v);
        }
        // all but the second were chosen
        else if (in_vc(first_neighbor) &&!in_vc(second_neighbor) && in_vc(third_neighbor) && in_vc(forth_neighbor))
        {
            m_graph.controller().exclude_from_vertex_cover(forth_neighbor.id());
            m_graph.controller().revalidate(v);
            m_graph.controller().include_into_vertex_cover(v);
        }
        // all but the third were chosen
        else if (in_vc(first_neighbor) && in_vc(second_neighbor) &&!in_vc(third_neighbor) && in_vc(forth_neighbor))
        {
            m_graph.controller().exclude_from_vertex_cover(first_neighbor.id());
            m_graph.controller().revalidate(v);
            m_graph.controller().include_into_vertex_cover(v);
        }
        // all but the forth were chosen
        else if (in_vc(first_neighbor) && in_vc(second_neighbor) && in_vc(third_neighbor) &&!in_vc(forth_neighbor))
        {
            m_graph.controller().exclude_from_vertex_cover(first_neighbor.id());
            m_graph.controller().revalidate(v);
            m_graph.controller().include_into_vertex_cover(v);
        }
        else
            throw std::logic_error("You want to tell me that there can be a clique with more than 1 vertex outside VC? Well, the path reduction does not agree.");
    }
    else
        throw std::logic_error("Cannot undo operation, only handles edges and data reduction vertices");
}

void degree_4_path_reduction::reject(const graph_operation::operation_type &op) {
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
        throw std::logic_error("Cannot undo operation, only handles edges and data reduction vertices (degree 3 path reduction)");

}

degree_4_path_reduction::degree_4_path_reduction(graph_unit graph) : data_reduction_node(graph) {}



