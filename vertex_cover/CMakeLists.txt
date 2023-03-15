cmake_minimum_required(VERSION 3.10)
project(algorithm-engineering)

set(CMAKE_CXX_STANDARD 17)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fpermissive)
endif()

set(SOURCES src/vertex_cover_solver.cpp include/vertex_cover_solver.hpp
        include/solver/k_solver.hpp
        src/solver/k_solver.cpp src/solver/max_degree_neighbor_rule_solver.cpp
        include/solver/max_degree_neighbor_rule_solver.hpp
        src/solver/random_edge_solver.cpp include/solver/random_edge_solver.hpp
        src/bounds/clique_cover_lb.cpp include/bounds/clique_cover_lb.hpp
        src/configuration.cpp include/configuration.hpp include/konig_lp_solver.hpp
        src/solver/max_degree_neighbor_rule_extend_solver.cpp include/solver/max_degree_neighbor_rule_extend_solver.hpp
        ../dep/nlohmann/json.hpp src/vertex_cover.cpp include/vertex_cover.hpp
        src/mirrors.cpp include/mirrors.hpp src/bounds/cycle_cover_lb.cpp include/bounds/cycle_cover_lb.hpp)

# library for the solver
add_library(VertexCoverLib ${SOURCES})
target_include_directories(VertexCoverLib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE ../graph/include src)
target_compile_options(VertexCoverLib PRIVATE ${PEDANTIC_WARNINGS} $<$<CONFIG:Release>:${RELEASE_FLAGS}> $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
target_link_libraries(
        VertexCoverLib Graph Common DataReduction
        Boost::boost Boost::thread Boost::system Boost::graph Boost::program_options Boost::filesystem
)

# executable solver
add_executable(VertexCover ${SOURCES} src/main.cpp)
target_compile_options(VertexCover PRIVATE ${PEDANTIC_WARNINGS} $<$<CONFIG:Release>:${RELEASE_FLAGS}> $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
target_link_libraries(
        VertexCover VertexCoverLib Graph Common DataReduction
        Boost::boost Boost::thread Boost::system Boost::graph Boost::program_options Boost::filesystem
)

# install resources
file(GLOB RESOURCES res/*)
install(FILES ${RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/res)

export(TARGETS VertexCoverLib FILE VertexCoverLibConfig.cmake)

# unit tests
add_executable(VertexCoverUnitTest
        test/src/clique_cover_test.cpp test/src/configuration_test.cpp test/src/mirror_test.cpp
        test/src/edge_writer_test.cpp test/src/cycle_cover_test.cpp)
target_link_libraries(VertexCoverUnitTest VertexCoverLib Common Graph GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_compile_options(VertexCoverUnitTest PRIVATE $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
add_test(VertexCoverUnitTest VertexCoverUnitTest)

# integration tests
add_executable(VertexCoverIntegrationTest
        test/src/vc_correctness_test.cpp)
target_link_libraries(VertexCoverIntegrationTest VertexCoverLib Common
        Graph GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_compile_options(VertexCoverIntegrationTest PRIVATE $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
add_test(VertexCoverIntegrationTest VertexCoverIntegrationTest)

file(GLOB RESOURCES test/res/*)
install(FILES ${RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/res)