PROJECT(MAVIS_TESTS)

cmake_minimum_required (VERSION 3.15)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(fp_test main.cpp)
target_link_libraries (fp_test mavis_test_lib)
