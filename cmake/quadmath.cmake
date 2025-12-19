include(ExternalProject)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_subdirectory(stubs)
    set(QUADMATH_PATCH_COMMAND git am --whitespace=nowarn ${CMAKE_CURRENT_SOURCE_DIR}/patches/0001-Patch-to-enable-building-quadmath-with-clang.patch)
    set(QUADMATH_CPPFLAGS "-include ${CMAKE_CURRENT_SOURCE_DIR}/stubs/quadmath_stubs.h")
    set(QUADMATH_CFLAGS -Wno-invalid-gnu-asm-cast)
    set(QUADMATH_LDFLAGS -L${CMAKE_CURRENT_BINARY_DIR}/stubs)
    set(QUADMATH_LIBS -lquadmath_stubs)
    set(QUADMATH_DEPENDS quadmath_stubs)
endif()

if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
    set(QUADMATH_CFLAGS "${QUADMATH_CFLAGS} -O0 -g")
    set(QUADMATH_CXXFLAGS "${QUADMATH_CXXFLAGS} -O0 -g")
    set(QUADMATH_LDFLAGS "${QUADMATH_LDFLAGS} -Wl,-O0")
endif()

# This sets up all of the paths we need to clone, build, and use libquadmath, but doesn't actually do
# anything until we run make
ExternalProject_Add(
    libquadmath
    DOWNLOAD_COMMAND ""
    UPDATE_DISCONNECTED True
    PATCH_COMMAND ${QUADMATH_PATCH_COMMAND}
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} CPPFLAGS=${QUADMATH_CPPFLAGS} CFLAGS=${QUADMATH_CFLAGS} LDFLAGS=${QUADMATH_LDFLAGS} LIBS=${QUADMATH_LIBS} <SOURCE_DIR>/libquadmath/configure --disable-multilib --prefix <INSTALL_DIR>
    BUILD_COMMAND $(MAKE) libquadmath.la
    INSTALL_COMMAND $(MAKE) install-exec
    DEPENDS ${QUADMATH_DEPENDS}
)

ExternalProject_Get_Property(libquadmath SOURCE_DIR INSTALL_DIR)
set(QUADMATH_SOURCE_DIR ${SOURCE_DIR})
set(QUADMATH_LIB_DIR ${INSTALL_DIR}/lib)
set(QUADMATH_INCLUDE_DIR ${QUADMATH_SOURCE_DIR}/libquadmath)

unset(SOURCE_DIR)
unset(INSTALL_DIR)

# This clones the libquadmath sources when we run CMake so that the includes are guaranteed to be available
# at build time. This will be re-run every time we run CMake, but the clone_libquadmath script will do
# nothing if the repo has already been cloned successfully
execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/clone_libquadmath
    WORKING_DIRECTORY ${QUADMATH_SOURCE_DIR}/../
    COMMAND_ERROR_IS_FATAL ANY
)
