PROJECT(MAVIS_TESTS)

cmake_minimum_required (VERSION 3.15)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options (-g -Werror -fPIC
  -Wall -Wextra -Winline -Winit-self -Wno-unused-function
  -Wuninitialized -Wno-sequence-point -Wno-inline -Wno-unknown-pragmas
  -Wno-unused-parameter -Wno-missing-field-initializers -pipe
  -Winvalid-pch)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(-Wpedantic -Wno-gnu-zero-variadic-macro-arguments)
endif ()

if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
  add_compile_options (-O0)
endif()

file(CREATE_LINK ${CMAKE_SOURCE_DIR}/json ${CMAKE_CURRENT_BINARY_DIR}/json SYMBOLIC)
file(CREATE_LINK ${CMAKE_SOURCE_DIR}/test/basic ${CMAKE_CURRENT_BINARY_DIR}/uarch SYMBOLIC)
file(CREATE_LINK ${CMAKE_SOURCE_DIR}/test/basic/golden.out ${CMAKE_CURRENT_BINARY_DIR}/golden.out SYMBOLIC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
add_executable(Mavis main.cpp)
target_link_libraries (Mavis PRIVATE mavis Boost::program_options Boost::json)
