//
// Created by Leon Suchy on 10.11.18.
//

#include <bipartite_graph.hpp>

#include "../include/bipartite_graph.hpp"

edge_inside_bipartite_partition::edge_inside_bipartite_partition(const graph& g, const vertex& a, const vertex& b)
        : std::logic_error("Tried to connect vertices " + g.name_of(a.id()) + " (ID " + std::to_string(a.id()) + ") "
                                                                                                                 "and " + g.name_of(b.id()) + " (ID " + std::to_string(b.id()) + ") " +
                           "which are on the same side of a bipartite graph.")
{ }

bipartite_graph::bipartite_graph(const graph& left, const graph& right)
    : graph(std::max(left.size_including_inactive(), right.size_including_inactive()) * 2), m_left_size(0), m_right_size(0)
{
    auto size = std::max(left.size_including_inactive(), right.size_including_inactive()) * 2;

    // fill the vertex list alternating between the left list on the even indices and the right list on the odd indices
    for (size_type i = 0; i < size; i++)
    {
        // right
        if (i % 2 == 1)
        {
            // end reached, fill with empty vertices
            if (right.size_including_inactive() < (i / 2) + 1)
            {
                m_data.vertices.emplace_back();
                continue;
            }

            add_vertex(vertex(right_id_at_position(i / 2)));
            m_right_size++;
        }
        // left
        else
        {
            // end reached fill with empty vertices
            if (left.size_including_inactive() < (i / 2) + 1)
            {
                m_data.vertices.emplace_back();
                continue;
            }

            add_vertex(vertex(left_id_at_position(i / 2)));
            m_left_size++;
        }
    }
}

bipartite_graph::bipartite_graph()
    : graph(), m_left_size(0), m_right_size(0)
{ }

bipartite_graph::bipartite_graph(graph::size_type capacity)
    : graph(capacity), m_left_size(0), m_right_size(0)
{ }

vertex::id_type bipartite_graph::in_original_graph(vertex::id_type id) const
{
    if (is_right(id))
    {
        return std::numeric_limits<vertex::id_type>::max() - id - 1;
    }
    return id;
}

vertex::id_type bipartite_graph::index_left(vertex::id_type id) const
{
    return id * 2;
}

vertex::id_type bipartite_graph::index_right(vertex::id_type id) const
{
    return index_left(id) + 1;
}

vertex::id_type bipartite_graph::add_vertex_left()
{
    return add_vertex_left(vertex(left_id_at_position(m_left_size)));
}

vertex::id_type bipartite_graph::group(const std::vector<vertex::id_type>& vertices)
{
    bool right = is_right(vertices[0]);
    for (auto vertex : vertices)
    {
        if (is_right(vertex) != right)
            throw group_over_multiple_partitions(*this, vertices);
    }

    vertex::id_type parent = right ? add_vertex_right(vertex(right_id_at_position(m_right_size), vertices))
            : add_vertex_left(vertex(left_id_at_position(m_left_size), vertices));

    graph::group_into(operator[](parent), vertices);

    return parent;
}

void bipartite_graph::add_edge(vertex& a, vertex& b)
{
    if ((is_left(a.id()) && is_left(b.id())) || (is_right(a.id()) && is_right(b.id())))
        throw edge_inside_bipartite_partition(*this, a, b);

    graph::add_edge(a, b);
}

std::vector<vertex::id_type> bipartite_graph::get_right() const
{
    std::vector<vertex::id_type> result;
    result.reserve(m_right_size);

    for (size_type i = 0; i < m_right_size; i++)
    {
        auto index = index_right(i);
        const vertex& v = m_data.vertices[index];
        if (!v.valid())
            continue;

        result.push_back(v.id());
    }

    return result;
}

std::vector<vertex::id_type> bipartite_graph::get_left() const
{
    std::vector<vertex::id_type> result;
    result.reserve(m_left_size);

    for (size_type i = 0; i < m_left_size; i++)
    {
        auto index = index_left(i);
        const vertex& v = m_data.vertices[index];
        if (!v.valid())
            continue;

        result.push_back(v.id());
    }

    return result;
}


std::vector<vertex::id_type> bipartite_graph::get_right_including_invalid() const
{
    std::vector<vertex::id_type> result;
    result.reserve(m_right_size);

    for (size_type i = 0; i < m_right_size; i++)
    {
        auto index = index_right(i);
        const vertex& v = m_data.vertices[index];

        result.push_back(v.id());
    }

    return result;
}

std::vector<vertex::id_type> bipartite_graph::get_left_including_invalid() const
{
    std::vector<vertex::id_type> result;
    result.reserve(m_left_size);

    for (size_type i = 0; i < m_left_size; i++)
    {
        auto index = index_left(i);
        const vertex& v = m_data.vertices[index];

        result.push_back(v.id());
    }

    return result;
}

bool bipartite_graph::is_right(vertex::id_type id) const
{
    return id > std::numeric_limits<vertex::id_type>::max() / 2;
}

bool bipartite_graph::is_left(vertex::id_type id) const
{
    return !is_right(id);
}

vertex::id_type bipartite_graph::add_vertex_right()
{
    return add_vertex_right(vertex(right_id_at_position(m_right_size)));
}

vertex* bipartite_graph::get_vertex_including_inactive(vertex::id_type index)
{
    vertex::id_type in_side_set = in_original_graph(index);

    // the side doesn't have this many elements
    if (in_side_set >= m_right_size && is_right(index))
        return nullptr;
    if (in_side_set >= m_left_size && is_left(index))
        return nullptr;

    size_type index_in_vector = is_right(index) ? index_right(in_side_set) : index_left(in_side_set);
    return &m_data.vertices[index_in_vector];
}

const vertex* bipartite_graph::get_vertex_including_inactive(vertex::id_type index) const
{
    vertex::id_type in_side_set = in_original_graph(index);

    // the side doesn't have this many elements
    if (in_side_set >= m_right_size && is_right(index))
        return nullptr;
    if (in_side_set >= m_left_size && is_left(index))
        return nullptr;

    size_type index_in_vector = is_right(index) ? index_right(in_side_set) : index_left(in_side_set);
    return &m_data.vertices[index_in_vector];
}

vertex::id_type bipartite_graph::left_id_at_position(vertex::id_type i) const
{
    return i;
}

vertex::id_type bipartite_graph::right_id_at_position(vertex::id_type i) const
{
    return std::numeric_limits<vertex::id_type>::max() - i - 1;
}

vertex::id_type bipartite_graph::add_vertex_left(vertex&& v)
{
    auto id = v.id();

    // right bias. more on the right
    if (m_right_size > m_left_size)
    {
        m_data.vertices[index_left(m_left_size)] = v;
        m_size++;
    }
        // need to insert empty element on the right
    else
    {
        add_vertex(std::move(v));           // left one
        add_vertex(vertex());               // invalid one on the right
    }

    m_left_size++;
    return id;
}

vertex::id_type bipartite_graph::add_vertex_right(vertex&& v)
{
    auto id = v.id();

    // left bias. more on the left
    if (m_left_size > m_right_size)
    {
        m_data.vertices[index_right(m_right_size)] = v;
        m_size++;
    }
        // need to insert empty element on the left
    else
    {
        add_vertex(vertex());               // invalid one on the left
        add_vertex(std::move(v));    // right one
    }

    m_right_size++;
    return id;
}

void bipartite_graph::add_edge(vertex::id_type a, vertex::id_type b)
{
    bipartite_graph::add_edge(operator[](a), operator[](b));
}

group_over_multiple_partitions::group_over_multiple_partitions(const graph& g, const std::vector<vertex::id_type>& vertices)
    : std::runtime_error("Cannot group vertices between multiple partitions. Vertices: " + list_vertices(g, vertices))
{ }

std::string group_over_multiple_partitions::list_vertices(const graph& g, const std::vector<vertex::id_type>& vertices)
{
    std::stringstream ss;
    for (auto id : vertices)
    {
        ss << id << "; ";
    }
    return ss.str();
}
