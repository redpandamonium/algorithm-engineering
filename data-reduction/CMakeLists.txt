cmake_minimum_required(VERSION 3.10)
project(algorithm-engineering)

add_library(DataReduction
        include/data_reduction_node.hpp src/data_reduction_node.cpp
        include/data_reduction_factory.hpp src/data_reduction_factory.cpp include/data_reductions.hpp
        src/data_reductions.cpp include/degree_0_reduction.hpp src/degree_0_reduction.cpp
        include/degree_1_reduction.hpp src/degree_1_reduction.cpp include/degree_2_folding.hpp
        src/degree_2_folding.cpp include/triangle_reduction.hpp src/triangle_reduction.cpp
        include/degree_greater_k_reduction.hpp src/degree_greater_k_reduction.cpp
        include/dominate_reduction.hpp src/dominate_reduction.cpp include/graph_operation.hpp
        src/graph_operation.cpp include/unconfined_reduction.hpp src/unconfined_reduction.cpp
        include/data_reduction_options.hpp include/container/data_reduction_container.hpp
        src/container/data_reduction_container.cpp include/container/exhaustive_container.hpp
        src/container/exhaustive_container.cpp include/container/exhaustive_infrequent_container.hpp
        src/container/exhaustive_infrequent_container.cpp src/lp_reduction.cpp include/lp_reduction.hpp
        src/degree_3_independent_set_reduction.cpp include/degree_3_independent_set_reduction.hpp
        src/degree_4_crossbow_reduction.cpp include/degree_4_crossbow_reduction.hpp
        src/degree_4_path_reduction.cpp include/degree_4_path_reduction.hpp src/degree_4_trebuchet_reduction.cpp
        include/degree_4_trebuchet_reduction.hpp src/neighbor_clique_partition_reduction.cpp
        include/neighbor_clique_partition_reduction.hpp include/container/once_container.hpp src/container/once_container.cpp
        include/loop_reduction.hpp src/loop_reduction.cpp src/container/exhaustive_iterative_container.cpp
        include/container/exhaustive_iterative_container.hpp src/funnel_reduction.cpp include/funnel_reduction.hpp
        src/desk_reduction.cpp include/desk_reduction.hpp)

target_include_directories(DataReduction PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE ../graph/include src)
target_compile_options(DataReduction PRIVATE ${PEDANTIC_WARNINGS} $<$<CONFIG:Release>:${RELEASE_FLAGS}> $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
target_link_libraries(DataReduction Common Graph)

export(TARGETS DataReduction FILE DataReductionConfig.cmake)

add_executable(DataReductionUnitTest
        test/degree_0_test.cpp test/degree_1_test.cpp test/degree_2_test.cpp
        test/degree_greater_k_test.cpp test/dominate_test.cpp test/neighbor_clique_partition_test.cpp
        test/container_test.cpp test/degree_three_independent_set_test.cpp test/degree_four_path_test.cpp
        test/degree_four_crossbow_test.cpp test/lp_test.cpp test/lp_test.cpp test/loop_reduction_test.cpp
        test/degree_four_trebuchet_test.cpp)
target_link_libraries(DataReductionUnitTest DataReduction Common Graph gtest_main gmock)
target_compile_options(DataReductionUnitTest PRIVATE $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
add_test(DataReductionUnitTest DataReductionUnitTest)