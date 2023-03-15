//
// Created by iarch on 11/10/18.
//

#include <queue>
#include <util.hpp>
#include "hopcroft_karp_matching.hpp"

hopcroft_karp_matching::hopcroft_karp_matching(graph& g) : m_graph(g)
{
    init();
}

void hopcroft_karp_matching::init()
{
    m_in.resize(m_graph.size_including_inactive(), nil);
    m_out.resize(m_graph.size_including_inactive(), nil);
    m_dist.resize(m_graph.size_including_inactive(), infinity);
}

void hopcroft_karp_matching::update()
{
    for (graph::size_type v = 0; v < m_graph.size_including_inactive(); v++)
    {
        auto left_matched = m_out[v];

        // the vertex at i is matched AND only one of the endpoints in the matching edge is invalid
        if (left_matched != nil && (m_graph[v].valid() ^ m_graph[left_matched].valid()))
        {
            m_in[left_matched] = nil;
            m_out[v] = nil;
        }
    }
}

void hopcroft_karp_matching::on_edge_remove(const edge& e)
{
    auto first = e.get_a();
    auto second = e.get_b();

    if (m_in[first] == second)
    {
        m_in[first] = nil;
        m_out[second] = nil;
    }

    if (m_in[second] == first)
    {
        m_in[second] = nil;
        m_out[first] = nil;
    }
}

std::vector<edge> hopcroft_karp_matching::calculate_matching()
{
    std::vector<edge> matching;

    while(bfs())
    {
        for (auto& v : m_graph)
        {
            auto id = v.id();
            if (m_out[id] == nil)
                dfs(id);
        }
    }

    // go through all right vertices and get their matched left vertex
    for (auto matched_left : m_out)
    {
        // not matched
        if (matched_left == nil)
            continue;

        auto matched_right = m_in[matched_left];
        matching.emplace_back(matched_left, matched_right);
    }

    return matching;
}

bool hopcroft_karp_matching::bfs() {

    std::queue<vertex::id_type> queue;
    for (const auto& v : m_graph)
    {
        auto id = v.id();
        m_graph[id].color(m_graph[id].color() & (~VISITED_RIGHT));
        m_graph[id].color(m_graph[id].color() & (~VISITED_LEFT));

        // right vertex not matched to a left vertex yet
        if (m_out[id] == nil)
        {
            // free vertex init level 
            m_dist[id] = 0;
            m_graph[id].color(m_graph[id].color() | VISITED_LEFT);
            queue.push(id);
        }
    }

    bool cont = false;
    // bfs
    while(!queue.empty())
    {
        auto current = queue.front();
        queue.pop();

        auto neighbors = m_graph[current].neighbors();
        for (auto neighbor : neighbors)
        {
            if (util::is_marked(m_graph, neighbor, VISITED_RIGHT))
                continue;

            m_graph[neighbor].color(m_graph[neighbor].color() | VISITED_RIGHT);
            // get vertex that neighbour is matched too
            auto matched_vertex = m_in[neighbor];

            // not done
            if (matched_vertex == nil)
                cont = true;
            else
            {
                // increase its distance and add it to the queue
                m_dist[matched_vertex] = m_dist[current] + 1;
                m_graph[matched_vertex].color(m_graph[matched_vertex].color() | VISITED_LEFT);
                queue.push(matched_vertex);
            }
        }
    }

    return cont;
}

bool hopcroft_karp_matching::dfs(vertex::id_type start)
{
    if (start == nil)
    {
        return true;
    }
    for(auto n : m_graph[start].neighbors())
    {
        auto matched_vertex = m_in[n];

        // follow distances set by bfs
        if (matched_vertex == nil || (m_dist[start] < m_dist[matched_vertex] && dfs(matched_vertex)))
        {
            m_in[n] = start;
            m_out[start] = n;
            return true;
        }
    }
    return false;
}

const std::map<vertex::id_type, vertex::id_type>& hopcroft_karp_matching::left_right_mapping() const
{
    return m_left_right;
}

const std::map<vertex::id_type, vertex::id_type>& hopcroft_karp_matching::right_left_mapping() const
{
    return m_right_left;
}

const std::vector<vertex::id_type>& hopcroft_karp_matching::left_right() const
{
    return m_in;
}

const std::vector<vertex::id_type>& hopcroft_karp_matching::right_left() const
{
    return m_out;
}

const std::vector<u_int32_t> hopcroft_karp_matching::distances() const
{
    return m_dist;
}