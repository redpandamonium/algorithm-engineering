cmake_minimum_required(VERSION 3.10)
project(algorithm-engineering)

# setup testing globally
configure_file(CMakeLists.txt.in ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY googletest-download)
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(
        ${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL
)
if (APPLE)

include_directories("${gtest_SOURCE_DIR}/include"
                    "${gmock_SOURCE_DIR}/include"
                    "/usr/local/include")
else ()

include_directories("${gtest_SOURCE_DIR}/include"
                    "${gmock_SOURCE_DIR}/include")

endif ()

include_directories(${PROJECT_SOURCE_DIR}/dep)

# c++ standard
set(CMAKE_CXX_STANDARD 17)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fpermissive)
endif()

# libraries
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system graph program_options)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

enable_testing()

# warnings
set(PEDANTIC_WARNINGS -Wpedantic -Wall -Wextra -Wno-unused-parameter)

# profile dependent build options
set(DEBUG_FLAGS -Og -g)
set(RELEASE_FLAGS -O3 -march=native)

add_subdirectory(graph)
add_subdirectory(common)
add_subdirectory(data-reduction)
add_subdirectory(vertex_cover)
add_subdirectory(tools)
add_subdirectory(vc_heuristic)

