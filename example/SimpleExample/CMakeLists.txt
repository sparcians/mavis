project(SIMPLE_EXAMPLE)

add_executable(SimpleExample
    main.cpp
    )

target_link_libraries(SimpleExample mavis boost_json)
