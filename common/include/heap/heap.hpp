//
// Created by Leon Suchy on 07.11.18.
//

#ifndef ALGORITHM_ENGINEERING_HEAP_HPP
#define ALGORITHM_ENGINEERING_HEAP_HPP

#include <string>
#include <stdexcept>
#include <unordered_map>
#include <boost/heap/fibonacci_heap.hpp>

/*
 * Definitions
 * ---------------------------------------------------------------------------------------------------------------------
 */

template <typename T, class Impl>
class heap
{
public:

    typedef Impl heap_implementation_type;
    typedef T value_type;
    typedef const T& const_reference_type;

    typedef typename heap_implementation_type::handle_type handle_type;

    typedef typename heap_implementation_type::size_type size_type;
    typedef typename heap_implementation_type::value_compare compare_type;
    typedef typename heap_implementation_type::ordered_iterator ordered_iterator;

    explicit heap(const compare_type& = compare_type());
    explicit heap(size_type capacity, const compare_type& = compare_type());
    heap(const heap& cpy);
    heap(heap&& mv) noexcept;

    heap<value_type, heap_implementation_type>& operator=(heap<value_type, heap_implementation_type> h);
    // heap<value_type, heap_implementation_type>& operator=(heap<value_type, heap_implementation_type> &&) noexcept = default;

    void swap(heap& other) noexcept;

    template<class I> bool operator==(const heap<value_type, I>& other) const;
    template<class I> bool operator!=(const heap<value_type, I>& other) const;

    // access

    const_reference_type top() const;

    ordered_iterator begin() const;
    ordered_iterator end() const;

    // insert/erase

    bool insert(const_reference_type elem);
    bool erase(const_reference_type v);
    void pop();
    void clear();

    // mutability

    void increase(const_reference_type v);
    void decrease(const_reference_type v);
    void update(const_reference_type v);

    // size

    size_type size() const;
    bool empty() const;

    // memory
    void reserve(size_type);

private:

    heap_implementation_type m_impl;
    std::unordered_map<value_type, handle_type> m_handle_map;
};

/*
 * Instantiations
 * ---------------------------------------------------------------------------------------------------------------------
 */

template <class T>
using fibonacci_heap = heap<T, boost::heap::fibonacci_heap<T>>;

/*
 * Implementations
 * ---------------------------------------------------------------------------------------------------------------------
 */

template<typename T, class Impl>
heap<T, Impl>::heap(const compare_type& compare)
    : m_impl(compare)
{ }

template<typename T, class Impl>
heap<T, Impl>::heap(const heap& cpy)
    : m_impl(cpy.m_impl)
{ }

template<typename T, class Impl>
heap<T, Impl>::heap(heap&& mv) noexcept
    : m_impl(std::move(mv.m_impl))
{ }

template<typename T, class Impl>
heap<T, Impl>& heap<T, Impl>::operator=(heap h)
{
    swap(h);
    return *this;
}

template<typename T, class Impl>
void heap<T, Impl>::swap(heap& other) noexcept
{
    std::swap(m_impl, other.m_impl);
}

template<typename T, class Impl>
template<class I>
bool heap<T, Impl>::operator==(const heap<value_type, I>& other) const
{
    return m_impl.operator==(other.m_impl);
}

template<typename T, class Impl>
template<class I>
bool heap<T, Impl>::operator!=(const heap<value_type, I>& other) const
{
    return !operator==(other);
}

template<typename T, class Impl>
bool heap<T, Impl>::insert(const_reference_type elem)
{
    handle_type handle = m_impl.push(elem);
    m_handle_map.emplace(elem, handle);

    return true; // somehow cannot fail?
}

template<typename T, class Impl>
typename heap<T, Impl>::const_reference_type heap<T, Impl>::top() const
{
    return m_impl.top();
}

template<typename T, class Impl>
void heap<T, Impl>::pop()
{
    m_handle_map.erase(top());
    m_impl.pop();
}

template<typename T, class Impl>
void heap<T, Impl>::clear()
{
    m_impl.clear();
    m_handle_map.clear();
}

template<typename T, class Impl>
typename heap<T, Impl>::size_type heap<T, Impl>::size() const
{
    return m_impl.size();
}

template<typename T, class Impl>
bool heap<T, Impl>::empty() const
{
    return m_impl.empty();
}

template<typename T, class Impl>
void heap<T, Impl>::reserve(size_type size)
{
    // heaps don't have a reserve
    // m_impl.allocator();
}

template<typename T, class Impl>
heap<T, Impl>::heap(size_type capacity, const compare_type& comparator)
    : m_impl(comparator)
{
    reserve(capacity);
}

template<typename T, class Impl>
void heap<T, Impl>::increase(const_reference_type v)
{
    auto it = m_handle_map.find(v);
    if (it == m_handle_map.end())
        return;
        //throw std::invalid_argument("Cannot increase value " + std::to_string(v) + ": Not on the heap");

    m_impl.increase(it->second, v);
}

template<typename T, class Impl>
void heap<T, Impl>::decrease(const_reference_type v)
{
    auto it = m_handle_map.find(v);
    if (it == m_handle_map.end())
        return;
        // throw std::invalid_argument("Cannot decrease value " + std::to_string(v) + ": Not on the heap");

    m_impl.decrease(it->second, v);
}

template<typename T, class Impl>
void heap<T, Impl>::update(const_reference_type v)
{
    auto it = m_handle_map.find(v);
    if (it == m_handle_map.end())
        throw std::invalid_argument("Cannot update value " + std::to_string(v) + ": Not on the heap");

    m_impl.update(it->second, v);
}

template<typename T, class Impl>
bool heap<T, Impl>::erase(const_reference_type v)
{
    auto it = m_handle_map.find(v);
    if (it == m_handle_map.end())
        return false;

    m_impl.erase(it->second);
    m_handle_map.erase(it);

    return true;
}

template<typename T, class Impl>
typename heap<T, Impl>::ordered_iterator heap<T, Impl>::begin() const
{
    return m_impl.ordered_begin();
}

template<typename T, class Impl>
typename heap<T, Impl>::ordered_iterator heap<T, Impl>::end() const
{
    return m_impl.ordered_end();
}

#endif //ALGORITHM_ENGINEERING_HEAP_HPP
