PROJECT(MAVIS_TESTS)

file(CREATE_LINK ${CMAKE_SOURCE_DIR}/json ${CMAKE_CURRENT_BINARY_DIR}/json SYMBOLIC)
file(CREATE_LINK ${CMAKE_SOURCE_DIR}/test/basic ${CMAKE_CURRENT_BINARY_DIR}/uarch SYMBOLIC)
file(CREATE_LINK ${CMAKE_SOURCE_DIR}/test/basic/golden.out ${CMAKE_CURRENT_BINARY_DIR}/golden.out SYMBOLIC)

file(GLOB TSET_FILES "${CMAKE_SOURCE_DIR}/test/basic/*.tset")
foreach(FILE_PATH ${TSET_FILES})
  get_filename_component(FILENAME ${FILE_PATH} NAME)
  file(CREATE_LINK ${FILE_PATH} "${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}" SYMBOLIC)
endforeach()

add_executable(Mavis main.cpp)
target_link_libraries (Mavis mavis_test_lib)

mavis_test(Mavis_basic_test Mavis bash -c "./Mavis > test.out && diff test.out golden.out")
