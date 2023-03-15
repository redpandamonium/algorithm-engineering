cmake_minimum_required(VERSION 3.10)
project(algorithm-engineering)

# c++ standard
set(CMAKE_CXX_STANDARD 17)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fpermissive)
endif()

# warnings
set(PEDANTIC_WARNINGS -Wpedantic -Wall -Wextra -Wno-unused-parameter)

# profile dependent build options
set(DEBUG_FLAGS -Og -g)
set(RELEASE_FLAGS -O3 -march=native)

# setup testing globally
enable_testing()

# libraries
set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     ON)  # link Boost Static libraries
find_package(Boost REQUIRED COMPONENTS system graph program_options thread filesystem graph)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_subdirectory(graph)
add_subdirectory(common)
add_subdirectory(data-reduction)
add_subdirectory(vertex_cover)
add_subdirectory(tools)
# add_subdirectory(vc_heuristic)

