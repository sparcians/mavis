PROJECT(MAVIS_TESTS)

cmake_minimum_required (VERSION 3.15)

set (CMAKE_CXX_STANDARD 20)
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

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
add_executable(fp_test main.cpp)
target_link_libraries (fp_test mavis Boost::json)
