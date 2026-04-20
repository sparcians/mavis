PROJECT(MAVIS_TESTS)

cmake_minimum_required (VERSION 3.15)

if(NOT DEFINED MAVIS_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
else()
    set(CMAKE_CXX_STANDARD ${MAVIS_CXX_STANDARD})
endif()

set (CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    message("-- Skipping fp_test on Apple")
else()
    include(CheckSourceCompiles)
    include(CMakePushCheckState)

    include(${CMAKE_CURRENT_SOURCE_DIR}/simde.cmake)

    cmake_push_check_state(RESET)
        set(CMAKE_REQUIRED_QUIET True)

        if(CMAKE_CXX_STANDARD GREATER_EQUAL 23)
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

        # Newer GCC and Clang versions provide a __bf16 type
        check_source_compiles(CXX [[

            int main()
            {
                __bf16 val{1.0};
                (void)val;
            }
            ]] COMPILER_SUPPORTS_BF16)
    cmake_pop_check_state()

    add_executable(fp_test main.cpp)

    target_link_libraries (fp_test mavis_test_lib simde)

    if(COMPILER_SUPPORTS_BF16)
        target_compile_definitions(fp_test PRIVATE COMPILER_SUPPORTS_BF16)
    endif()

    if(NOT HAS_NATIVE_STD_FLOAT_128_SUPPORT)
        if(APPLE)
            message("-- Quad float not supported by this platform")
        else()
            include(${CMAKE_CURRENT_SOURCE_DIR}/quadmath.cmake)
            target_link_libraries(fp_test libquadmath)
        endif()
    endif()
endif()

add_executable(dump_fli_table dump_fli_table.cpp)

target_link_libraries (dump_fli_table mavis_test_lib)
