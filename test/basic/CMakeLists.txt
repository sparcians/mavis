PROJECT(MAVIS_TESTS)

file(CREATE_LINK ${CMAKE_SOURCE_DIR}/json ${CMAKE_CURRENT_BINARY_DIR}/json SYMBOLIC)
file(CREATE_LINK ${CMAKE_SOURCE_DIR}/test/basic ${CMAKE_CURRENT_BINARY_DIR}/uarch SYMBOLIC)
file(CREATE_LINK ${CMAKE_SOURCE_DIR}/test/basic/golden.out ${CMAKE_CURRENT_BINARY_DIR}/golden.out SYMBOLIC)

add_executable(Mavis main.cpp)
target_link_libraries (Mavis mavis_test_lib)

mavis_test(Mavis_basic_test Mavis bash -c "./Mavis > test.out && diff test.out golden.out")
