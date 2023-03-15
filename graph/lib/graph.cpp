//
// Created by Leon Suchy on 21.10.18.
//

#include <set>
#include <algorithm>
#include <iostream>     // cerr
#include <graph.hpp>

graph::graph()
    : m_data { }, m_size(0), m_num_edges(0)
{ }

graph::graph(size_type capacity)
    : m_data { }, m_size(0), m_num_edges(0)
{
    m_data.vertices.reserve(capacity);
}

vertex::id_type graph::add_vertex()
{
    return add_vertex("A" + std::to_string(m_data.vertices.size()));
}

void graph::add_edge(vertex& a, vertex& b)
{
    if (a == b)
        throw invalid_graph_operation("Cannot create a loop in the graph (" + a.string() + ")");

#undef GRAPH_DO_DOUBLE_EDGE_CHECK
#ifdef GRAPH_DO_DOUBLE_EDGE_CHECK
    auto neighbors = a.neighbors();
    auto it = std::find(neighbors.begin(), neighbors.end(), b.id());
    if (it != neighbors.end())
        throw invalid_graph_operation("Cannot add an edge twice. " + a.string() + ", " + b.string());
#endif

    bool success = true;
    success = success && a.add_edge_to(b);
    success = success && b.add_edge_to(a);

    if (success && a.valid() && b.valid())
        m_num_edges++;
}

void graph::remove_edge(vertex& a, vertex& b)
{
    /*
    if (a.is_group())
        throw invalid_graph_operation("Cannot remove edge from vertex " + a.string() + ": Is a group.");
    if (b.is_group())
        throw invalid_graph_operation("Cannot remove edge from vertex " + b.string() + ": Is a group.");
        */

    bool success = true;
    success = success && a.remove_edge_to(b);
    success = success && b.remove_edge_to(a);

    if (success && a.valid() && b.valid())
        m_num_edges--;
}

bool graph::has_edge(const vertex& a, const vertex& b) const
{
    auto a_neighbours = a.neighbors();
    auto b_neighbours = b.neighbors();

    // find vertex with smaller neighbourhood
    if (a_neighbours.size() > b_neighbours.size())
    {
        auto n_it = std::find(b_neighbours.begin(), b_neighbours.end(), a.id());
        return n_it != b_neighbours.end();
    }
    auto n_it = std::find(a_neighbours.begin(), a_neighbours.end(), b.id());
    return n_it != a_neighbours.end();
}

vertex::id_type graph::group(const std::vector<vertex::id_type>& vertices)
{
    if (!verify_groupable(vertices))
    {
        #ifdef GRAPH_DO_GROUP_EXCEPTION
            throw invalid_graph_operation("Tried to group vertices where at least one is invalid");
        #else
            return vertex::invalid_id;
        #endif
    }

    m_data.vertices.emplace_back(static_cast<unsigned int>(m_data.vertices.size()), vertices);
    m_size++; // add group vertex

    group_into(m_data.vertices.back(), vertices);

    return m_data.vertices.back().id();
}

void graph::group_into(vertex& parent, const std::vector<vertex::id_type>& vertices)
{
    // invalidate all vertices of this group
    for (auto vertex : vertices)
        invalidate(operator[](vertex));

    // find all edges that end outside the group
    std::set<vertex::id_type> group_neighbors; // prevent double edges
    for (auto vertex : vertices)
    {
        const auto& neighbors = operator[](vertex).neighbors_including_inactive();
        for (auto neighbor : neighbors)
        {
            // find out if this neighbor is also in the group
            auto it = std::find(vertices.begin(), vertices.end(), neighbor);

            // a vertex in the set has an edge to a vertex not in the set
            if (it == vertices.end())
                group_neighbors.insert(neighbor);
        }
    }

    // add all edges to outside the group
    for (auto neighbor : group_neighbors)
        add_edge(operator[](neighbor), parent);
}

vertex* graph::get_vertex(vertex::id_type index)
{
    vertex* vertex = get_vertex_including_inactive(index);

    // don't return an invalid vertex
    if (vertex && !vertex->valid())
        return nullptr;

    return vertex;
}

graph::size_type graph::size() const
{
    return m_size;
}

bool graph::empty() const
{
    return m_size == 0;
}

const std::vector<vertex::id_type>& graph::ungroup(vertex& v)
{
    // not a group. cannot ungroup
    if (!v.is_group())
        return v.children();

    // revalidate all children
    for (auto child : v.children())
        revalidate(operator[](child));

    invalidate(v);
    return v.children();
}

vertex* graph::get_vertex_including_inactive(vertex::id_type index)
{
    return m_data.vertices.size() > index ? &m_data.vertices[index] : nullptr;
}

void graph::invalidate(vertex& v)
{
    if (v.invalidate())
    {
        // invalidate edges to v
        for (auto neighbor : v.neighbors_including_inactive())
            operator[](neighbor).invalidate(v);
        m_size--;
        m_num_edges -= v.degree();
    }
}

void graph::revalidate(vertex& v)
{
    if (v.revalidate())
    {
        // revalidate edges to v
        for (auto neighbor : v.neighbors_including_inactive())
            operator[](neighbor).revalidate(v);

        m_size++;
        m_num_edges += v.degree();
    }
}

graph::iterator_type graph::begin()
{
    return iterator_type{ *this, static_cast<iterator_type::index_type>(0) };
}

graph::iterator_type graph::end()
{
    return iterator_type{ *this, m_data.vertices.size() };
}

void graph::add_edge(vertex::id_type a, vertex::id_type b)
{
    add_edge(operator[](a), operator[](b));
}

void graph::invalidate(vertex::id_type v)
{
    invalidate(operator[](v));
}

void graph::remove_edge(vertex::id_type a, vertex::id_type b)
{
    remove_edge(operator[](a), operator[](b));
}

bool graph::has_edge(vertex::id_type a, vertex::id_type b) const
{
    return has_edge(operator[](a), operator[](b));
}

const std::vector<vertex::id_type>& graph::ungroup(vertex::id_type v)
{
    return ungroup(operator[](v));
}

const vertex& graph::operator[](vertex::id_type id) const
{
    return *get_vertex_including_inactive(id);
}

vertex& graph::operator[](vertex::id_type id)
{
    return *get_vertex_including_inactive(id);
}

graph::const_iterator_type graph::cend() const
{
    return graph::const_iterator_type(*this, m_data.vertices.size());
}

graph::const_iterator_type graph::cbegin() const
{
    return graph::const_iterator_type(*this, 0);
}

void graph::invalidate(const std::vector<vertex::id_type>& vertices)
{
    for (auto vertex : vertices)
        invalidate(vertex);
}

graph::graph(graph_data&& data)
    : m_data(data), m_size(0), m_num_edges(0)
{
    for (const auto& v : m_data.vertices)
    {
        if (v.valid())
        {
            m_size++;
            m_num_edges += v.degree();
        }
    }

    m_num_edges /= 2; // undirected, we counted double
}

const graph_data& graph::data() const
{
    return m_data;
}

vertex::id_type graph::add_vertex(const std::string& name)
{
    auto id = static_cast<vertex::id_type>(m_data.vertices.size());
    bool existed = false;

    if (!name.empty())
        existed = !m_data.names.insert(graph_data::map_type::value_type(name, id)).second;

    if (existed)
        throw invalid_graph_operation("A named vertex must have a unique name");

    add_vertex(vertex(id));
    return m_data.vertices.back().id();
}

std::vector<vertex::id_type> graph::get_vertices_with_color(vertex::color_type color) const
{
    std::vector<vertex::id_type> result;
    for (const auto& vertex : m_data.vertices)
    {
        if (vertex.color() == color)
            result.push_back(vertex.id());
    }

    return result;
}

invalid_graph_operation::invalid_graph_operation(const std::string& error)
    : std::runtime_error(error)
{ }

edge::edge(vertex::id_type a, vertex::id_type b)
    : m_a(a), m_b(b)
{ }

edge::edge(const vertex& v1, const vertex& v2)
    : m_a(v1.id()), m_b(v2.id())
{ }

vertex::id_type edge::get_a() const
{
    return m_a;
}

vertex::id_type edge::get_b() const
{
    return m_b;
}

bool edge::operator==(const edge& e) const
{
    return (m_b == e.m_a && m_a == e.m_b) || (m_a == e.m_a && m_b == e.m_b);
}

bool edge::operator!=(const edge& e) const
{
    return !operator==(e);
}


bool edge::operator<(const edge& e) const
{
    // being less than means not being equal
    if ((m_b == e.m_a && m_a == e.m_b) || (m_a == e.m_a && m_b == e.m_b))
        return false;
    // if not equal, compare first by smaller, then by bigger
    auto left_max = std::max(m_a, m_b);
    auto left_min = std::min(m_a, m_b);
    auto right_max = std::max(e.m_a, e.m_b);
    auto right_min = std::min(e.m_a, e.m_b);

    if (left_min != right_min)
        return (left_min < right_min);
    else
        return (left_max < right_max);

}


void graph::revalidate(vertex::id_type v)
{
    revalidate(operator[](v));
}

std::string graph::name_of(vertex::id_type vertex) const
{
    auto it = m_data.names.right.find(vertex);
    if (it == m_data.names.right.end())
        return "";

    return it->second;
}

graph::const_iterator_type graph::end() const
{
    return cend();
}

graph::const_iterator_type graph::begin() const
{
    return cbegin();
}

bool graph::verify_groupable(const std::vector<vertex::id_type>& group) const
{
    for (auto vertex : group)
        if (!operator[](vertex).valid())
            return false;
    return true;
}

const vertex* graph::get_vertex_including_inactive(vertex::id_type index) const
{
    return m_data.vertices.size() > index ? &m_data.vertices[index] : nullptr;
}

graph::size_type graph::size_including_inactive() const
{
    return m_data.vertices.size();
}

void graph::add_vertex(vertex&& v)
{
    bool valid = v.valid();
    m_data.vertices.push_back(std::move(v));

    if (valid)
    {
        m_size++;
    }
}

graph::size_type graph::num_edges() const
{
    return m_num_edges;
}

void graph::remove(vertex& v)
{
    // if v is valid, invalidate it
    if (v.valid())
        invalidate(v);

    // remove all of v's edges
    auto neighbors = v.neighbors_including_inactive();
    for (auto neighbor : neighbors)
    {
        remove_edge(v, operator[](neighbor));
    }

    // remove v's name from the table
    {
        auto it = m_data.names.right.find(v.id());
        if (it != m_data.names.right.end())
            m_data.names.right.erase(it);
    }

    // find a swap element
    vertex* swap_in = m_data.vertices.back() != v ? &m_data.vertices.back() : nullptr;

    // v is not the last element
    if (swap_in != nullptr)
    {
        on_element_replace(v, *swap_in);

        auto swap_neighbors = swap_in->neighbors_including_inactive();

        // remove the edges of the swap-in element
        for (auto neighbor : swap_neighbors)
            remove_edge(*swap_in, operator[](neighbor));

        // copy it into position, assign a new ID
        m_data.vertices[v.id()] = vertex(*swap_in, v.id());

        // add all edges with the new ID
        for (auto neighbor : swap_neighbors)
            add_edge(m_data.vertices[v.id()], operator[](neighbor));

        // replace v's name with swap_in's name
        auto it = m_data.names.right.find(swap_in->id());
        if (it != m_data.names.right.end())
        {
            const std::string& name = it->second;
            m_data.names.right.erase(it);
            m_data.names.insert(graph_data::map_type::value_type(name, v.id()));
        }
    }

    on_element_remove(m_data.vertices.back());
    m_data.vertices.pop_back();
}

void graph::on_element_remove(vertex& removed)
{

}

void graph::remove(vertex::id_type v_id)
{
    remove(operator[](v_id));
}

void graph::on_element_replace(vertex& u, vertex& v)
{

}

void graph::remove(edge e)
{
    remove_edge(e.get_a(), e.get_b());
}
