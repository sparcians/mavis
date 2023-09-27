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

add_executable(Mavis main.cpp)
target_link_libraries (Mavis mavis)

#
# CppCheck.
#
# This conflicts with tools that include Mavis (and use cppcheck)
#
# Add a custom CMake Modules directory
#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
## Setup for HTML report
#set (CPPCHECK_XML_OUTPUT "${PROJECT_BINARY_DIR}/analysis/cppcheck/cppcheck_analysis.xml")
#set (CPPCHECK_EXCLUDES
#  ${CMAKE_BINARY_DIR}
#  ${SIM_BASE}/test
#  )
#find_package (Cppcheck)
