//
// Created by Carsten Schubert on 2019-01-28.
//
#include <utility>

#include <util.hpp>

#include "degree_4_trebuchet_reduction.hpp"

std::string degree_4_trebuchet_reduction::name() const {
    return "degree-4-trebuchet";
}

bool degree_4_trebuchet_reduction::apply(graph_operation::stack &op_stack) {
    bool graph_changed = false;

    for (auto vertex_it = m_graph.graph().cbegin(); vertex_it != m_graph.graph().cend(); ++vertex_it)
    {
        class vertex vertex = *vertex_it;
        if (vertex.degree() != 4)
            continue;

        const auto neighbors = vertex.neighbors();

        // checks if it is a trebuchet (3-vertex-path and a separated vertex) and begins assigning the vertices
        class vertex first_neighbor, second_neighbor, third_neighbor, forth_neighbor;
        // 1-3 is the path, 4 is the isolated vertex

        bool found_first  = false;
        bool found_second = false;
        bool found_third  = false;
        bool found_forth  = false;
        bool abort = false;
        for (vertex::id_type v : neighbors){
            // 'degree' in induced subgraph
            unsigned deg = count_edges_in_subgraph(v, neighbors);
            if (deg == 0 && !found_forth){
                found_forth = true;
                forth_neighbor = m_graph.graph()[v];
            }
            else if (deg == 1 && !found_first){
                found_first = true;
                first_neighbor = m_graph.graph()[v];
            }
            else if (deg == 1 && !found_third){
                found_third = true;
                third_neighbor = m_graph.graph()[v];
            }
            else if (deg == 2 && !found_second){
                found_second = true;
                second_neighbor = m_graph.graph()[v];
            }
            else {
                abort = true;
                break;
            }
        }
        if (abort)
            continue;

        // requirements clear now, beginning procedure...
        graph_changed = true;

        op_stack.push(*this, graph_operation::data_reduction_vertices{ vertex.id(), first_neighbor.id(), second_neighbor.id(), third_neighbor.id(), forth_neighbor.id() });
        m_graph.controller().invalidate(vertex.id());

        const auto first_neighbor_neighbors  = first_neighbor.neighbors();
        const auto second_neighbor_neighbors = second_neighbor.neighbors();
        const auto third_neighbor_neighbors  = third_neighbor.neighbors();
        const auto forth_neighbor_neighbors  = forth_neighbor.neighbors();

        // now rearranging edges according to keikaku
        op_stack.push(*this, graph_operation::edge_remove{ edge{first_neighbor.id(), second_neighbor.id()}, nullptr });
        op_stack.push(*this, graph_operation::edge_insert{ edge{second_neighbor.id(), forth_neighbor.id()}, nullptr });
        op_stack.push(*this, graph_operation::edge_insert{ edge{third_neighbor.id(), forth_neighbor.id()}, nullptr });
        m_graph.controller().remove_edge(first_neighbor.id(), second_neighbor.id());
        m_graph.controller().add_edge(second_neighbor.id(), forth_neighbor.id());
        m_graph.controller().add_edge(third_neighbor.id(), forth_neighbor.id());

        // connect to neighbors
        connect_to_them_except(op_stack, first_neighbor.id(), third_neighbor_neighbors, second_neighbor.id());
        connect_to_them(op_stack, second_neighbor.id(), forth_neighbor_neighbors);
        connect_to_them(op_stack, third_neighbor.id(), forth_neighbor_neighbors);
        connect_to_them_except(op_stack, forth_neighbor.id(), first_neighbor_neighbors, second_neighbor.id());

    }
    return graph_changed;
}

void degree_4_trebuchet_reduction::connect_to_them(graph_operation::stack &op_stack, vertex::id_type vertex,
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

void degree_4_trebuchet_reduction::connect_to_them_except(graph_operation::stack &op_stack, vertex::id_type vertex,
                                                          const std::vector<vertex::id_type> &set, vertex::id_type except)
{
    for (auto& current : set)
    {
        if (current == vertex || current == except)
            continue;
        if (!m_graph.graph()[vertex].get_incident_edge_to(m_graph.graph()[current])){
            m_graph.controller().add_edge(vertex, current);
            op_stack.push(*this, graph_operation::edge_insert{edge{vertex, current}, nullptr });
        }
    }

}



bool degree_4_trebuchet_reduction::in_vc(vertex v) {
    return util::is_in_vertex_cover(v);
}

unsigned degree_4_trebuchet_reduction::count_edges_in_subgraph(const vertex::id_type v, const std::vector<vertex::id_type>& subgraph)
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

void degree_4_trebuchet_reduction::accept(const graph_operation::operation_type &op) {
    if (std::holds_alternative<graph_operation::edge_insert>(op))
    {
        const auto& insert = std::get<graph_operation::edge_insert>(op);
        m_graph.controller().remove_edge(insert.e.get_a(), insert.e.get_b());
    }
    else if (std::holds_alternative<graph_operation::edge_remove>(op))
    {
        const auto& removed = std::get<graph_operation::edge_remove>(op);
        m_graph.controller().add_edge(removed.e.get_a(), removed.e.get_b());
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
        if     ( in_vc(first_neighbor) && in_vc(second_neighbor) && in_vc(third_neighbor) && in_vc(forth_neighbor))
        {
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
            m_graph.controller().exclude_from_vertex_cover(forth_neighbor.id());
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
            // the whole clique (2, 3, 4) was chosen
        else if(!in_vc(first_neighbor) && in_vc(second_neighbor) && in_vc(third_neighbor) && in_vc(forth_neighbor))
        {
            m_graph.controller().exclude_from_vertex_cover(third_neighbor.id());
            m_graph.controller().revalidate(v);
            m_graph.controller().include_into_vertex_cover(v);
        }
            // the following three cases all have VC size 2, thus we take v and the second
        else if(!in_vc(first_neighbor) &&!in_vc(second_neighbor) && in_vc(third_neighbor) && in_vc(forth_neighbor))
        {
            m_graph.controller().exclude_from_vertex_cover(forth_neighbor.id());
            m_graph.controller().exclude_from_vertex_cover(third_neighbor.id());
            m_graph.controller().revalidate(v);
            m_graph.controller().include_into_vertex_cover(v);
            m_graph.controller().revalidate(second_neighbor.id());
            m_graph.controller().include_into_vertex_cover(second_neighbor.id());
        }
        else if(!in_vc(first_neighbor) && in_vc(second_neighbor) &&!in_vc(third_neighbor) && in_vc(forth_neighbor))
        {
            m_graph.controller().exclude_from_vertex_cover(forth_neighbor.id());
            m_graph.controller().revalidate(v);
            m_graph.controller().include_into_vertex_cover(v);
        }
        else if(!in_vc(first_neighbor) && in_vc(second_neighbor) && in_vc(third_neighbor) &&!in_vc(forth_neighbor))
        {
            m_graph.controller().exclude_from_vertex_cover(third_neighbor.id());
            m_graph.controller().revalidate(v);
            m_graph.controller().include_into_vertex_cover(v);
        }
        else
            throw std::logic_error("You topdecked a case the trebuchet reduction deemed impossible. Congratulations...");
    }
    else
        throw std::logic_error("Cannot undo operation, only handles edges and data reduction vertices");
}

void degree_4_trebuchet_reduction::reject(const graph_operation::operation_type &op) {
    if (std::holds_alternative<graph_operation::edge_insert>(op))
    {
        const auto& insert = std::get<graph_operation::edge_insert>(op);
        m_graph.controller().remove_edge(insert.e.get_a(), insert.e.get_b());
    }
    else if (std::holds_alternative<graph_operation::edge_remove>(op))
    {
        const auto& removed = std::get<graph_operation::edge_remove>(op);
        m_graph.controller().add_edge(removed.e.get_a(), removed.e.get_b());
    }
    else if (std::holds_alternative<graph_operation::data_reduction_vertices>(op))
    {
        graph_operation::data_reduction_vertices data = std::get<graph_operation::data_reduction_vertices>(op);
        auto v = data.v;
        m_graph.controller().revalidate(v);
    }
    else
        throw std::logic_error("Cannot undo operation, only handles edges and data reduction vertices (trebuchet reduction)");

}

degree_4_trebuchet_reduction::degree_4_trebuchet_reduction(graph_unit graph) : data_reduction_node(graph) {}




