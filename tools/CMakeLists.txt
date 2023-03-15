cmake_minimum_required(VERSION 3.10)
project(algorithm-engineering)

install(FILES benchmark.py DESTINATION ${CMAKE_BINARY_DIR}/vertex_cover PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)