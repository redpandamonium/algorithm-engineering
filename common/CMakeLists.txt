cmake_minimum_required(VERSION 3.10)
project(algorithm-engineering)

add_library(Common
        src/graph_unit.cpp include/graph_unit.hpp include/graph_controller.hpp include/graph_meta_data.hpp
        src/graph_controller.cpp src/graph_meta_data.cpp src/heap_comparators.cpp include/heap/heap.hpp
        include/heap/heap_comparators.hpp src/util.cpp include/util.hpp include/hopcroft_karp_matching.hpp
        src/hopcroft_karp_matching.cpp include/graph_listener.hpp src/fast_set.cpp include/fast_set.hpp)

target_include_directories(Common PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE ../graph/include src)
target_compile_options(Common PRIVATE ${PEDANTIC_WARNINGS} $<$<CONFIG:Release>:${RELEASE_FLAGS}> $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
target_link_libraries(Common Graph)

export(TARGETS Common FILE CommonConfig.cmake)

add_executable(CommonUnitTest test/garbage_collect_test.cpp test/graph_controller_test.cpp test/heap_test.cpp test/hopcroft_karp_matching_test.cpp test/set_test.cpp)
target_link_libraries(CommonUnitTest Common Graph
        GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_compile_options(CommonUnitTest PRIVATE $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
add_test(CommonUnitTest CommonUnitTest)
