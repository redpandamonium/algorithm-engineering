cmake_minimum_required(VERSION 3.10)
project(algorithm-engineering)

set(SOURCES include/edge_writer.hpp lib/edge_writer.cpp include/open_wbo_writer.hpp lib/open_wbo_writer.cpp
        lib/open_wbo_reader.cpp include/open_wbo_reader.hpp include/graph.hpp lib/graph.cpp include/graph_reader.hpp
        lib/graph_reader.cpp include/vertex.hpp lib/vertex.cpp include/graph_data.hpp include/graph_io.hpp
        include/graph_writer.hpp lib/graph_writer.cpp lib/bipartite_graph.cpp include/bipartite_graph.hpp
        lib/graph_iterator.hpp lib/graph_iterator.cpp include/depth_first_search.hpp include/vertex.hpp
        include/graph_search.hpp include/breadth_first_search.hpp include/components.hpp lib/components.cpp
        include/subgraph.hpp lib/subgraph.cpp lib/util.hpp include/random_graph.hpp lib/random_graph.cpp
        include/pace_graph_reader.hpp lib/pace_graph_reader.cpp include/pace_solution_writer.hpp lib/pace_solution_writer.cpp)

add_library(Graph ${SOURCES} include/edge.hpp lib/egde.cpp include/types.hpp lib/neighbors_iterator.cpp include/neighbors_iterator.hpp lib/neighbor_range.cpp include/neighbor_range.hpp)
target_link_libraries(Graph Boost::boost)

target_compile_options(Graph PRIVATE ${PEDANTIC_WARNINGS} $<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>)
target_include_directories(Graph PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE lib)

export(TARGETS Graph FILE GraphConfig.cmake)

add_executable(GraphUnitTest
        test/bipartite_graph_test.cpp test/breadth_first_search_test.cpp test/depth_first_search_test.cpp
        test/edge_count_test.cpp test/graph_iterator_test.cpp test/graph_reader_test.cpp
        test/graph_test.cpp test/partition_test.cpp test/subgraph_test.cpp test/vertex_test.cpp
        test/pace_graph_io_test.cpp)
target_link_libraries(GraphUnitTest Graph
        GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(GraphUnitTest GraphUnitTest)