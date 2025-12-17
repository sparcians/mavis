cmake_minimum_required(VERSION 3.15)
project(Mavis)

if(NOT DEFINED MAVIS_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
else()
    set(CMAKE_CXX_STANDARD ${MAVIS_CXX_STANDARD})
endif()

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckIncludeFile)
include(CMakePushCheckState)

find_package (Boost 1.75.0 REQUIRED COMPONENTS program_options json)
message (STATUS "Using BOOST ${Boost_VERSION_STRING}")

if(NOT DISABLE_MAVIS_FLOAT128_SUPPORT)
    cmake_push_check_state(RESET)
        if(CMAKE_CXX_STANDARD GREATER_EQUAL 23)
            include(CheckSourceCompiles)
            check_source_compiles(CXX [[
                #include <format>
                #include <iostream>
                #include <stdfloat>

                int main()
                {
                    std::float128_t val{1.0};
                    std::cout << std::format("{}", val) << std::endl;
                }
            ]] HAS_NATIVE_STD_FLOAT_128_SUPPORT)
        endif()

        if(NOT HAS_NATIVE_STD_FLOAT_128_SUPPORT)
            check_include_file(quadmath.h SYSTEM_HAS_QUADMATH)

            if(NOT SYSTEM_HAS_QUADMATH)
                set(BUILD_QUADMATH_FROM_SOURCE True)
            endif()
        endif()
    cmake_pop_check_state()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/elfio ${Boost_INCLUDE_DIRS})

add_library(mavis
  impl/ExtractorRegistry.cpp
  impl/FormRegistry.cpp
  impl/InstMetaData.cpp
  impl/forms/CommonForms.cpp
  impl/forms/CompressedForms.cpp
  impl/forms/VectorForms.cpp
)

target_include_directories(mavis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/int128/include ${CMAKE_CURRENT_SOURCE_DIR}/simde)

function(mavis_dependencies target)
    if(BUILD_QUADMATH_FROM_SOURCE)
        add_dependencies(${target} libquadmath)
    endif()
endfunction()

if(DISABLE_MAVIS_FLOAT128_SUPPORT)
    message("-- Quad float support explicitly disabled")
    add_compile_definitions(DISABLE_MAVIS_FLOAT128_SUPPORT)
elseif(NOT HAS_NATIVE_STD_FLOAT_128_SUPPORT)
    if(BUILD_QUADMATH_FROM_SOURCE)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/quadmath.cmake)
        target_include_directories(mavis PUBLIC ${QUADMATH_INCLUDE_DIR})
        target_link_directories(mavis PUBLIC ${QUADMATH_LIB_DIR})
        mavis_dependencies(mavis)
        target_link_libraries(mavis ${QUADMATH_LIB_DIR}/libquadmath.a)
    else()
        target_link_libraries(mavis PUBLIC quadmath)
    endif()
endif()

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
