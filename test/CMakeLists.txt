
add_subdirectory(basic)

# Look for boost po.  If not found, no directed test
set (_BOOST_COMPONENTS program_options)
find_package (Boost 1.74.0 REQUIRED COMPONENTS ${_BOOST_COMPONENTS})
if (Boost_FOUND)
  include_directories (SYSTEM ${Boost_INCLUDE_DIRS})
  message (STATUS "Using BOOST ${Boost_VERSION_STRING}")
  add_subdirectory(directed)
else()
  message (WARNING "Directed mavis test skipped due to no Boost Program Options")
endif()
