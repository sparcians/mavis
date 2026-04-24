PROJECT(MAVIS_TESTS)

file(CREATE_LINK ${CMAKE_SOURCE_DIR}/json ${CMAKE_CURRENT_BINARY_DIR}/json SYMBOLIC)
file(CREATE_LINK ${CMAKE_SOURCE_DIR}/test/basic ${CMAKE_CURRENT_BINARY_DIR}/uarch SYMBOLIC)

add_executable(mavis_decode main.cpp)

target_link_libraries (mavis_decode mavis_test_lib mavis_test_inst_lib Boost::program_options)
