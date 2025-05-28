project(SIMPLE_EXAMPLE)

add_executable(SimpleExample
    main.cpp
    )

target_link_libraries (SimpleExample PRIVATE mavis Boost::program_options Boost::json)
