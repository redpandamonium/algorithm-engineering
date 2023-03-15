//
// Created by Leon Suchy on 27.10.18.
//

#ifndef ALGORITHM_ENGINEERING_VERTEX_COVER_SOLVER_HPP
#define ALGORITHM_ENGINEERING_VERTEX_COVER_SOLVER_HPP

#include <memory> // unique_ptr

#include <graph.hpp>

#include "../../data-reduction/include/data_reduction_factory.hpp"
#include "solver/k_solver.hpp"
#include "configuration.hpp"
#include <graph_unit.hpp>

class vertex_cover_solver
{
public:

    static constexpr const int not_found = -1;

    class bound
    {
    public:

        bound();
        explicit bound(int k);

        bool is_set() const;

        int get() const;

    private:

        static constexpr const int no_bound = -1;
        int m_bound;
    };

    explicit vertex_cover_solver(graph_unit unit, const configuration& config, const data_reduction_factory& reduction_factory, bound b = bound());

    /**
     * @brief Solves the vertex cover. Colors the minimum vertex cover with true.
     * @return
     */
    int solve();

    int operator()();

    int recursive_steps() const;

private:

    graph_unit m_graph;
    const configuration& m_config;
    const data_reduction_factory&  m_reduction_factory;
    std::unique_ptr<k_solver> m_solver;
    bound m_bound;
    //bool m_garbage_collect;
};


#endif //ALGORITHM_ENGINEERING_VERTEX_COVER_SOLVER_HPP
