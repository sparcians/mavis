add_library(quadmath_stubs quadmath_stubs.cpp)
target_compile_options(quadmath_stubs PRIVATE -fno-lto)
set_property(TARGET quadmath_stubs PROPERTY POSITION_INDEPENDENT_CODE ON)
