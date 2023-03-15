//
// Created by Leon Suchy on 18.11.18.
//

#ifndef ALGORITHM_ENGINEERING_DEPTH_FIRST_SEARCH_HPP
#define ALGORITHM_ENGINEERING_DEPTH_FIRST_SEARCH_HPP

#include <set>
#include <stack>
#include <iterator>
#include <exception>

#include "vertex.hpp"
#include "graph.hpp"

/**
 * @brief Iterator over a graph in depth first search order
 * @tparam VertexT Vertex type
 * @tparam GraphT Graph type
 */
template<typename VertexT, typename GraphT>
class depth_first_search_iterator
    : public std::iterator<std::forward_iterator_tag, VertexT, long, VertexT*, VertexT&>
{
public:

    typedef std::size_t size_type;

    typedef depth_first_search_iterator<VertexT, GraphT> this_type;

    typedef VertexT value_type;
    typedef value_type& value_reference;
    typedef value_type* value_pointer;
    typedef typename value_type::id_type id_type;

    typedef GraphT graph_type;
    typedef graph_type* graph_pointer;
    typedef graph_type& graph_reference;

    static const id_type invalid_id = value_type::invalid_id;

    /**
     * @brief Begin iterator constructor
     * @param graph Graph
     * @param start Start vertex
     */
    explicit depth_first_search_iterator(graph_reference graph, id_type start);

    /**
     * @brief End iterator constructor
     */
    depth_first_search_iterator();

    /**
     * @brief Dereference
     * @return Current vertex
     */
    value_reference operator*() const;

    /**
     * @brief Dereference
     * @return Current vertex
     */
    value_pointer operator->() const;

    /**
     * @brief Pre-increment
     * @return this
     */
    depth_first_search_iterator& operator++();

    /**
     * @brief Post-increment
     * @param offset Dummy, ignored
     * @return New iterator
     * @note For the love of dog, don't use this
     * @complexity Too high
     */
    const depth_first_search_iterator operator++(int offset) const;

    /**
     * @brief Equality comparison
     * @tparam VertexU Other vertex type. Needs to have id() getter and a comparable ID type
     * @tparam GraphU Other graph type. No requirements.
     * @param rhs Compare to this iterator
     * @return True if both iterators are on the same vertex
     */
    template<typename VertexU, typename GraphU>
    bool operator==(const depth_first_search_iterator<VertexU, GraphU>& rhs) const;

    /**
     * @brief Inequality comparison
     * @tparam VertexU Other vertex type. Needs to have id() getter and a comparable ID type
     * @tparam GraphU Other graph type. No requirements.
     * @param rhs Compare to this iterator
     * @return False if both iterators are on the same vertex
     */
    template<typename VertexU, typename GraphU>
    bool operator!=(const depth_first_search_iterator<VertexU, GraphU>& rhs) const;

private:

    graph_pointer m_graph;
    id_type m_current;
    std::stack<id_type> m_stack;
    std::set<id_type> m_visited;
};

/**
 * @brief Represents a search
 * @tparam VertexT Vertex type
 * @tparam GraphT Graph type
 * This class acts like a fake container so ranged loops work on this.
 */
template<typename VertexT, typename GraphT>
class depth_first_search
{
public:

    typedef std::size_t size_type;
    typedef size_type index_type;

    typedef VertexT value_type;
    typedef typename value_type::id_type id_type;

    typedef GraphT graph_type;
    typedef graph_type& graph_reference;
    typedef graph_type* graph_pointer;

    typedef depth_first_search_iterator<value_type, graph_type> iterator;

    explicit depth_first_search(graph_reference g, id_type begin);

    /**
     * @brief Begin iterator
     * @return Iterator on the start vertex
     */
    iterator begin();

    /**
     * @brief End iterator
     * @return Finished DFS
     */
    iterator end();

private:

    id_type m_begin;
    graph_pointer m_graph;
};

inline depth_first_search<const class vertex, const class graph> dfs(const graph& graph, vertex::id_type start)
{
    return depth_first_search<const class vertex, const class graph>(graph, start);
}

inline depth_first_search<class vertex, class graph> dfs(graph& graph, vertex::id_type start)
{
    return depth_first_search<class vertex, class graph>(graph, start);
}

/*
* depth_first_search_iterator
* ---------------------------------------------------------------------------------------------------------------------
*/

template<typename VertexT, typename GraphT>
depth_first_search_iterator<VertexT, GraphT>::depth_first_search_iterator(graph_reference graph, id_type start)
        : m_graph(&graph), m_current(invalid_id), m_stack(), m_visited()
{
    if (start != invalid_id)
    {
        m_stack.push(start);
        m_visited.insert(start);
        operator++();
    }
}

template<typename VertexT, typename GraphT>
depth_first_search_iterator<VertexT, GraphT>::depth_first_search_iterator()
        : m_graph(nullptr), m_current(invalid_id), m_stack(), m_visited()
{ }

template<typename VertexT, typename GraphT>
typename depth_first_search_iterator<VertexT, GraphT>::value_reference
depth_first_search_iterator<VertexT, GraphT>::operator*() const
{
    if (m_current == invalid_id)
        throw std::out_of_range("Cannot dereference end iterator");

    return (*m_graph)[m_current];
}

template<typename VertexT, typename GraphT>
typename depth_first_search_iterator<VertexT, GraphT>::value_pointer
depth_first_search_iterator<VertexT, GraphT>::operator->() const
{
    return m_current == invalid_id ? nullptr : &operator*();
}

template<typename VertexT, typename GraphT>
depth_first_search_iterator<VertexT, GraphT>&
depth_first_search_iterator<VertexT, GraphT>::operator++()
{
    // reached the end
    if (m_stack.empty())
    {
        m_current = invalid_id;
        return *this;
    }

    m_current = m_stack.top();
    m_stack.pop();

    auto neighbors = (*m_graph)[m_current].neighbors();
    for (auto neighbor : neighbors)
    {
        // not visited
        if (m_visited.find(neighbor) == m_visited.end())
        {
            m_stack.push(neighbor);
            m_visited.insert(neighbor);
        }
    }

    return *this;
}

template<typename VertexT, typename GraphT>
const depth_first_search_iterator<VertexT, GraphT> depth_first_search_iterator<VertexT, GraphT>::operator++(int offset) const
{
    this_type copy(*this);
    ++copy;
    return copy;
}

template<typename VertexT, typename GraphT>
template<typename VertexU, typename GraphU>
bool
depth_first_search_iterator<VertexT, GraphT>::operator==(
        const depth_first_search_iterator<VertexU, GraphU>& rhs
) const
{
    return m_current == rhs.m_current;
}

template<typename VertexT, typename GraphT>
template<typename VertexU, typename GraphU>
bool depth_first_search_iterator<VertexT, GraphT>::operator!=(
        const depth_first_search_iterator<VertexU, GraphU>& rhs
) const
{
    return !operator==(rhs);
}

/*
 * depth_first_search
 * ---------------------------------------------------------------------------------------------------------------------
 */

template<typename VertexT, typename GraphT>
depth_first_search<VertexT, GraphT>::depth_first_search(graph_reference g, id_type begin)
    : m_begin(begin), m_graph(&g)
{ }

template<typename VertexT, typename GraphT>
typename depth_first_search<VertexT, GraphT>::iterator depth_first_search<VertexT, GraphT>::begin()
{
    return iterator(*m_graph, m_begin);
}

template<typename VertexT, typename GraphT>
typename depth_first_search<VertexT, GraphT>::iterator depth_first_search<VertexT, GraphT>::end()
{
    return iterator();
}

#endif //ALGORITHM_ENGINEERING_DEPTH_FIRST_SEARCH_HPP
