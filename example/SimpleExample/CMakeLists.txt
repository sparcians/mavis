project(SIMPLE_EXAMPLE)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SimpleExample
    main.cpp
    )

target_link_libraries(SimpleExample mavis Boost::json)
