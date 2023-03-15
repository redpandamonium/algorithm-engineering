//
// Created by Leon Suchy on 22.11.18.
//

#ifndef ALGORITHM_ENGINEERING_COMPONENTS_HPP
#define ALGORITHM_ENGINEERING_COMPONENTS_HPP

#include <vector>

#include "graph.hpp"
#include "subgraph.hpp"

class components
{
public:

    [[nodiscard]] static std::vector<std::unique_ptr<subgraph>> subgraphs(graph& g);

private:

    static constexpr const vertex::color_type marked = 0x2;

    static void mark(vertex& v);
    static bool is_marked(const vertex& v);
    static void unmark(graph& g);
};

#endif //ALGORITHM_ENGINEERING_COMPONENTS_HPP
