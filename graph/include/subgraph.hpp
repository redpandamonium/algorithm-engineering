//
// Created by Leon Suchy on 23.11.18.
//

#ifndef ALGORITHM_ENGINEERING_SUBGRAPH_HPP
#define ALGORITHM_ENGINEERING_SUBGRAPH_HPP

#include "graph.hpp"

class subgraph : public graph
{
public:

    subgraph(const graph& supergraph, const std::vector<vertex::id_type>& vertices);

    [[nodiscard]] const graph& supergraph() const;

    /**
     * @brief Translate a subgraph ID to a supergraph ID
     * @param id Subgraph ID
     * @return Supergraph ID. Returns invalid ID if the ID does not map to any vertex in the supergraph or does not exist.
     */
    vertex::id_type translate_to_super(vertex::id_type id) const;

protected:

    void on_element_replace(vertex& from, vertex& to) override;

    void on_element_remove(vertex& removed) override;

private:

    const graph& m_supergraph;
    std::map<vertex::id_type, vertex::id_type> m_backmap;

    void add_incident_edges(const std::map<vertex::id_type, vertex::id_type>& from_supergraph, vertex::id_type supergraph_vertex);
    void transfer_id(vertex::id_type from, vertex::id_type to);
};

#endif //ALGORITHM_ENGINEERING_SUBGRAPH_HPP
