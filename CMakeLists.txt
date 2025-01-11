cmake_minimum_required(VERSION 3.15)
project(Mavis)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(.)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/elfio)

add_library(mavis
  impl/ExtractorRegistry.cpp
  impl/FormRegistry.cpp
  impl/forms/CommonForms.cpp
  impl/forms/CompressedForms.cpp
  impl/forms/VectorForms.cpp
)
add_subdirectory(test EXCLUDE_FROM_ALL)

set(CMAKE_CXX_FLAGS_DEBUG "-U_FORTIFY_SOURCE -O0 -g")
set(CMAKE_CXX_FLAGS_FASTDEBUG "-O3 -g")
