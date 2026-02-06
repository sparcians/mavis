PROJECT(MAVIS_TESTS)

cmake_minimum_required (VERSION 3.15)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

file(CREATE_LINK ${CMAKE_SOURCE_DIR}/json ${CMAKE_CURRENT_BINARY_DIR}/json SYMBOLIC)
file(CREATE_LINK ${CMAKE_SOURCE_DIR}/test/basic ${CMAKE_CURRENT_BINARY_DIR}/uarch SYMBOLIC)
file(CREATE_LINK ${CMAKE_SOURCE_DIR}/test/perf/rv64.tset ${CMAKE_CURRENT_BINARY_DIR}/rv64.tset SYMBOLIC)

add_executable(perf_test main.cpp)
target_link_libraries (perf_test mavis_test_lib mavis_test_inst_lib)
