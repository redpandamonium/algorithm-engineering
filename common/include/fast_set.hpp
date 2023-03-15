//
// Created by synis on 2/17/19.
//

#ifndef ALGORITHM_ENGINEERING_SET_HPP
#define ALGORITHM_ENGINEERING_SET_HPP

#include <cstddef>
#include <vector>

class fast_set {
public:

    explicit fast_set(size_t size);
    explicit fast_set(size_t size, size_t set_id);

    /**
     * @brief Add an element to the set and return whether it was already present
     * @param elem to add
     * @return true if elem was in the set, false otherwise
     */
    bool add (size_t elem);

    /**
     * @brief Check if an element is in the set
     * @param elem to check for membership
     * @return if elem is in the set
     */
    [[nodiscard]] bool is_member (size_t elem) const;

    /**
     * @brief Remove an element from the set
     * @param elem to remove
     */
    void remove (size_t elem);

    /**
     * @brief Clear the elements in the set
     */
    void clear ();

    [[nodiscard]] bool operator[](size_t elem);

private:

    size_t m_set_id;
    size_t m_size;
    std::vector<size_t> m_set;

    static constexpr const size_t initial_set_id = 1;
    static constexpr const size_t initial_set_value = 0;

    static constexpr const size_t m_max_id = std::numeric_limits<size_t>::max();
};


#endif //ALGORITHM_ENGINEERING_SET_HPP
