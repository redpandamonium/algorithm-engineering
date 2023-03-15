//
// Created by Leon Suchy on 25.10.18.
//

#include <limits>
#include <vertex.hpp>

#include "vertex.hpp"

vertex::vertex(id_type id)
        : m_id(id), m_incident_edges(), m_children(), m_color(0), m_valid(true), m_num_valid_neighbors(0)
{

}

vertex::size_type vertex::degree() const
{
    return m_num_valid_neighbors;
}

bool vertex::valid() const
{
    return m_valid;
}

bool vertex::operator==(const vertex& n) const
{
    return m_id == n.m_id;
}

vertex::id_type vertex::id() const
{
    return m_id;
}

std::ostream& operator<<(std::ostream& os, const vertex& n)
{
    os << n.string();
    return os;
}

bool vertex::invalidate()
{
    if (m_valid)
    {
        m_valid = false;
        return true;
    }
    return false;
}

void vertex::invalidate(const vertex& neighbor)
{
    // we ignore this request if the neighbor doesn't exist or is already invalidated
    auto index = get_neighbor_index(neighbor);

    if (index == m_incident_edges.size()) // not found
        return;
    if (index >= m_num_valid_neighbors) // already invalidated
        return;

    // swap the invalid vertex with the last valid one and decrease the number of valid neighbors
    std::swap(m_incident_edges[m_num_valid_neighbors - 1], m_incident_edges[index]);
    m_num_valid_neighbors--;
}

bool vertex::add_edge_to(const vertex& v)
{
    return add_edge_to(v, ::graphlib::directed_edge(m_id, v.id()));
}

bool vertex::remove_edge_to(const vertex& v)
{
    // check if v is actually a neighbor
    size_type i = get_neighbor_index(v);
    if (i == m_incident_edges.size())
        return false;

    // vertex is the last in the list
    if (i == m_incident_edges.size() - 1)
    {
        m_incident_edges.pop_back();
    }
        // vertex should be replaced by an invalid vertex. this happens when it's either the last valid or any invalid one.
    else if (i + 1 >= m_num_valid_neighbors)
    {
        m_incident_edges[i] = m_incident_edges.back();
        m_incident_edges.pop_back();
    }
        // should be replaced by a valid vertex
    else
    {
        // replace by last valid vertex
        m_incident_edges[i] = m_incident_edges[m_num_valid_neighbors - 1];

        // replace former last valid vertex by the last invalid one
        if (m_num_valid_neighbors != m_incident_edges.size())
        {
            m_incident_edges[m_num_valid_neighbors - 1] = m_incident_edges.back();
        }

        m_incident_edges.pop_back();
    }

    // adjust number of valid neighbors
    if (v.valid())
        m_num_valid_neighbors--;

    return true;
}

bool vertex::revalidate()
{
    if (!m_valid)
    {
        m_valid = true;
        return true;
    }
    return false;
}

void vertex::revalidate(const vertex& neighbor)
{
    size_type i = get_neighbor_index(neighbor);
    if (i == m_incident_edges.size()) // not found
        return;
    if (i < m_num_valid_neighbors) // already valid
        return;

    // swap the vertex with the first invalid one
    std::swap(m_incident_edges[i], m_incident_edges[m_num_valid_neighbors]);

    m_num_valid_neighbors++;
}

vertex::size_type vertex::get_neighbor_index(const vertex& v) const
{
    size_type i = 0;

    // find the index of v in the neighbor list
    for (; i < m_incident_edges.size(); i++)
    {
        if (m_incident_edges[i].get_to() == v.id())
            break; // found
    }

    return i;
}

vertex::vertex(id_type id, std::vector<id_type> children)
        : m_id(id), m_incident_edges{ }, m_children(std::move(children)), m_color(0), m_valid(true), m_num_valid_neighbors(0)
{ }

std::vector<vertex::id_type> vertex::neighbors() const
{
    if (m_num_valid_neighbors == 0) return std::vector<id_type>{ };

    std::vector<id_type> res(m_num_valid_neighbors);
    for (std::size_t i = 0; i < m_num_valid_neighbors; ++i)
    {
        res[i] = m_incident_edges[i].get_to();
    }
    return res;
}

bool vertex::is_group() const
{
    return !m_children.empty();
}

const std::vector<vertex::id_type>& vertex::children() const
{
    return m_children;
}

std::vector<vertex::id_type> vertex::neighbors_including_inactive() const
{
    std::vector<id_type> res(m_incident_edges.size());
    for (std::size_t i = 0; i < m_incident_edges.size(); i++)
    {
        res[i] = m_incident_edges[i].get_to();
    }
    return res;
}

bool vertex::operator!=(const vertex& n) const
{
    return !operator==(n);
}

bool vertex::operator<(const vertex& n) const
{
    return m_id < n.m_id;
}

vertex::vertex()
        : m_id(invalid_id), m_incident_edges(), m_children(), m_color(0), m_valid(false), m_num_valid_neighbors(0)
{ }

std::string vertex::string() const
{
    using namespace std::string_literals;
    return "[q"s + std::to_string(m_id) + "]"s;
}

vertex::color_type vertex::color() const
{
    return m_color;
}

void vertex::color(color_type c)
{
    m_color = c;
}

vertex::vertex(const vertex& cpy, vertex::id_type new_id)
    : vertex(cpy)
{
    m_id = new_id;
}

bool vertex::add_edge_to(const vertex& v, ::graphlib::directed_edge::flow_type flow)
{
    return add_edge_to(v, ::graphlib::directed_edge(m_id, v.id(), false, true, 0, flow));
}

bool vertex::add_edge_to(const vertex& v, const ::graphlib::directed_edge& e)
{
    // check if v is already a neighbor
    size_type i = get_neighbor_index(v);
    if (i != m_incident_edges.size())
        return false;

    // if the edge is invalid we only put it at the end
    if (!v.valid())
    {
        m_incident_edges.push_back(e);
        return true;
    }

    // simple case: all neighbors are valid. just push back and increment
    if (m_num_valid_neighbors == m_incident_edges.size())
    {
        m_incident_edges.push_back(e);
    }
        // complex case: some neighbors are invalid
    else
    {
        // swap the first invalid element with the new one (end) and increment
        auto first_invalid = m_incident_edges[m_num_valid_neighbors];
        m_incident_edges[m_num_valid_neighbors] = e;
        m_incident_edges.push_back(first_invalid);
    }

    m_num_valid_neighbors++;
    return true;
}

std::vector<::graphlib::directed_edge> vertex::get_incident_edges() const
{
    return m_num_valid_neighbors == 0 ? std::vector<::graphlib::directed_edge>() : std::vector<::graphlib::directed_edge>(m_incident_edges.begin(), m_incident_edges.begin() + m_num_valid_neighbors);
}

const std::vector<::graphlib::directed_edge>& vertex::get_incident_edges_including_inactive() const
{
    return m_incident_edges;
}

boost::optional<const graphlib::directed_edge&> vertex::get_incident_edge_to(const vertex& v) const
{
    auto it = std::find(m_incident_edges.begin(), m_incident_edges.end(), ::graphlib::directed_edge(m_id, v.id()));
    if (it == m_incident_edges.end())
        return boost::optional<const graphlib::directed_edge&>();

    return boost::optional<const graphlib::directed_edge&>(*it);
}

std::vector<::graphlib::directed_edge>& vertex::get_incident_edges_including_inactive()
{
    return m_incident_edges;
}

boost::optional<::graphlib::directed_edge&> vertex::get_incident_edge_to(const vertex& v)
{
    auto it = std::find(m_incident_edges.begin(), m_incident_edges.end(), ::graphlib::directed_edge(m_id, v.id()));
    if (it == m_incident_edges.end())
        return boost::optional<graphlib::directed_edge&>();

    return boost::optional<graphlib::directed_edge&>(*it);
}
