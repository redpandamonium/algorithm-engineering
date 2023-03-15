//
// Created by Leon Suchy on 23.01.19.
//

#ifndef ALGORITHM_ENGINEERING_EXHAUSTIVE_INFREQUENT_CONTAINER_HPP
#define ALGORITHM_ENGINEERING_EXHAUSTIVE_INFREQUENT_CONTAINER_HPP

#include "data_reduction_container.hpp"

class exhaustive_infrequent_container : public data_reduction_container
{
public:

    exhaustive_infrequent_container(graph_unit unit, std::vector<std::unique_ptr<data_reduction_node>>&& children, unsigned frequency);

    std::string name() const override;

    bool apply(graph_operation::stack& op_stack) override;

    void accept(const graph_operation::operation_type& op) override;

    void reject(const graph_operation::operation_type& op) override;

    [[nodiscard]] unsigned get_frequency() const noexcept;

private:

    static constexpr const unsigned default_frequency = 10;

    unsigned m_frequency;
};

#endif //ALGORITHM_ENGINEERING_EXHAUSTIVE_INFREQUENT_CONTAINER_HPP
