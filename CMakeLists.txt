cmake_minimum_required(VERSION 3.15)
project(Mavis)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package (Boost 1.74.0 REQUIRED COMPONENTS program_options json)
message (STATUS "Using BOOST ${Boost_VERSION_STRING}")

include_directories(.)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/elfio ${Boost_INCLUDE_DIRS})

add_library(mavis
  impl/ExtractorRegistry.cpp
  impl/FormRegistry.cpp
  impl/forms/CommonForms.cpp
  impl/forms/CompressedForms.cpp
  impl/forms/VectorForms.cpp
)
add_subdirectory(example EXCLUDE_FROM_ALL)
add_subdirectory(test EXCLUDE_FROM_ALL)

set(CMAKE_CXX_FLAGS_DEBUG "-U_FORTIFY_SOURCE -O0 -g")
set(CMAKE_CXX_FLAGS_FASTDEBUG "-O3 -g")

if(NOT CLANGFORMAT_EXECUTABLE)
  set(CLANGFORMAT_EXECUTABLE clang-format)
endif()

if(PROJECT_IS_TOP_LEVEL)
  add_custom_target(clangformat
    git ls-files "*.hpp" "*.cpp" "*.h" "*.tcc" > clang-format-files.out
    COMMAND
       ${CLANGFORMAT_EXECUTABLE} -i --files=clang-format-files.out
    COMMAND
       rm -f clang-format-files.out
    WORKING_DIRECTORY
       ${CMAKE_SOURCE_DIR}
    COMMENT
       "Formatting with ${CLANGFORMAT_EXECUTABLE} ..."
    )
endif()
