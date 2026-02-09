#pragma once

// For __cpp_lib_format
#include <version>
#ifdef __cpp_lib_format
#include <format>
#define MAVIS_HAS_STD_FORMAT
#else
#include <boost/format.hpp>
#endif
