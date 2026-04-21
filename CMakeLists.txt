cmake_minimum_required(VERSION 3.15)
project(Mavis)

if(NOT DEFINED MAVIS_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
else()
    set(CMAKE_CXX_STANDARD ${MAVIS_CXX_STANDARD})
endif()

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package (Boost 1.75.0 REQUIRED COMPONENTS program_options json)
message (STATUS "Using BOOST ${Boost_VERSION_STRING}")

# Copied from https://cliutils.gitlab.io/modern-cmake/chapters/projects/submodule.html
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/elfio/CMakeLists.txt")
    message(FATAL_ERROR "The elfio submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_subdirectory(elfio)

add_library(mavis
  impl/ExtractorRegistry.cpp
  impl/FormRegistry.cpp
  impl/InstMetaData.cpp
  impl/forms/CommonForms.cpp
  impl/forms/CompressedForms.cpp
  impl/forms/VectorForms.cpp
)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/softfloat.cmake)
target_include_directories(mavis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mavis PUBLIC elfio Boost::json softfloat)

include(CheckSourceCompiles)
include(CMakePushCheckState)

cmake_push_check_state(RESET)
    set(CMAKE_REQUIRED_QUIET True)

    # Check if std::bitset can be constexpr
    check_source_compiles(CXX [[
        #include <bitset>
        #ifndef __cpp_lib_constexpr_bitset
        #error "no constexpr bitset support"
        #endif

        int main()
        {
        }
        ]] COMPILER_SUPPORTS_CONSTEXPR_BITSET)
cmake_pop_check_state()

# Bring in the bitset2 library if std::bitset isn't constexpr
if(NOT COMPILER_SUPPORTS_CONSTEXPR_BITSET)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/bitset2.cmake)
    target_include_directories(mavis PUBLIC "${bitset2_SOURCE_DIR}")
endif()

# Setup common compiler options for the tests and examples
add_library(mavis_test_lib INTERFACE)
target_link_libraries (mavis_test_lib INTERFACE mavis)
target_compile_options (mavis_test_lib INTERFACE -g -Werror -fPIC
  -Wall -Wextra -Winline -Winit-self -Wno-unused-function
  -Wuninitialized -Wno-sequence-point -Wno-inline -Wno-unknown-pragmas
  -Wno-unused-parameter -Wno-missing-field-initializers -pipe
  -Winvalid-pch -Woverloaded-virtual)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(mavis_test_lib INTERFACE -Wpedantic -Wno-gnu-zero-variadic-macro-arguments)
endif ()

if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
  target_compile_options (mavis_test_lib INTERFACE -O0)
endif()

# For tests that use the Inst class in test/basic/Inst.h
add_library(mavis_test_inst_lib INTERFACE)
target_include_directories(mavis_test_inst_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/test/basic)

add_subdirectory(example EXCLUDE_FROM_ALL)
add_subdirectory(test EXCLUDE_FROM_ALL)

target_compile_options(mavis PUBLIC -fPIC)
set(CMAKE_CXX_FLAGS_DEBUG "-U_FORTIFY_SOURCE -O0 -g")
set(CMAKE_CXX_FLAGS_FASTDEBUG "-O3 -g")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/internal.cmake OPTIONAL)

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
