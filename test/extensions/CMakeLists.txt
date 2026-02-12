PROJECT(MAVIS_TESTS)

cmake_minimum_required (VERSION 3.15)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

file(CREATE_LINK ${CMAKE_SOURCE_DIR}/json ${CMAKE_CURRENT_BINARY_DIR}/json SYMBOLIC)
file(CREATE_LINK ${CMAKE_SOURCE_DIR}/test/extensions/hello ${CMAKE_CURRENT_BINARY_DIR}/hello SYMBOLIC)
file(CREATE_LINK ${CMAKE_SOURCE_DIR}/test/extensions/hello_stripped ${CMAKE_CURRENT_BINARY_DIR}/hello_stripped SYMBOLIC)
file(CREATE_LINK ${CMAKE_SOURCE_DIR}/test/basic ${CMAKE_CURRENT_BINARY_DIR}/uarch SYMBOLIC)

add_executable(ExtensionManager main.cpp)
target_link_libraries (ExtensionManager mavis_test_lib mavis_test_inst_lib)
