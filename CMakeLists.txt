cmake_minimum_required(VERSION 3.15)
project(Mavis)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(.)

add_library(mavis
  impl/ExtractorRegistry.cpp
  impl/FormRegistry.cpp
)

add_subdirectory(test EXCLUDE_FROM_ALL)

#
# CppCheck
#
# Add a custom CMake Modules directory
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
# Setup for HTML report
set (CPPCHECK_XML_OUTPUT "${PROJECT_BINARY_DIR}/analysis/cppcheck/cppcheck_analysis.xml")
set (CPPCHECK_EXCLUDES
  ${CMAKE_BINARY_DIR}
  ${SIM_BASE}/test
)
find_package (Cppcheck)
