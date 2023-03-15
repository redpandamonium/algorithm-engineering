cmake_minimum_required(VERSION 3.10)
project(algorithm-engineering)

# Heuristic Lib
add_library(HeuristicLib src/maximal_matching.cpp include/maximal_matching.hpp src/max_degree_heuristic.cpp include/max_degree_heuristic.hpp)
target_include_directories(HeuristicLib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE src)
target_link_libraries(HeuristicLib Graph Common)

export(TARGETS HeuristicLib FILE HeuristicLibConfig.cmake)

# Heuristic Executable
add_executable(Heuristic src/main.cpp)
target_link_libraries(Heuristic HeuristicLib Graph Common)

# Unit tests
add_executable(HeuristicUnitTest test/heuristic_test.cpp)
target_link_libraries(HeuristicUnitTest HeuristicLib Graph Common gtest_main gmock)
add_test(HeuristicUnitTest HeuristicUnitTest)