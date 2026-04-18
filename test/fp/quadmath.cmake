include(ExternalProject)

include(CMakePushCheckState)

add_library(libquadmath INTERFACE)

cmake_push_check_state(RESET)
    include(CheckIncludeFile)
    set(CMAKE_REQUIRED_QUIET True)

    check_include_file(quadmath.h SYSTEM_HAS_QUADMATH)

    if(NOT SYSTEM_HAS_QUADMATH)
        set(BUILD_QUADMATH_FROM_SOURCE True)
    endif()
cmake_pop_check_state()

if(BUILD_QUADMATH_FROM_SOURCE)
    set(MAVIS_PATCHES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/patches)
    
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_subdirectory(stubs)
        set(QUADMATH_PATCH_COMMAND git am --whitespace=nowarn ${MAVIS_PATCHES_DIR}/0001-Patch-to-enable-building-quadmath-with-clang.patch ${MAVIS_PATCHES_DIR}/0002-Patch-to-silence-unsupported-option-print-multi-os-d.patch)
        set(QUADMATH_CPPFLAGS "-include ${CMAKE_CURRENT_SOURCE_DIR}/stubs/quadmath_stubs.h -Wno-unknown-warning-option")
        set(QUADMATH_CFLAGS "-Wno-invalid-gnu-asm-cast -fheinous-gnu-extensions")
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
        libquadmath_build
        DOWNLOAD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/clone_libquadmath
        UPDATE_DISCONNECTED True
        PATCH_COMMAND ${QUADMATH_PATCH_COMMAND}
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} CPPFLAGS=${QUADMATH_CPPFLAGS} CFLAGS=${QUADMATH_CFLAGS} LDFLAGS=${QUADMATH_LDFLAGS} LIBS=${QUADMATH_LIBS} <SOURCE_DIR>/libquadmath/configure --disable-multilib --prefix <INSTALL_DIR>
        BUILD_COMMAND $(MAKE) libquadmath.la
        INSTALL_COMMAND $(MAKE) install-exec
        DEPENDS ${QUADMATH_DEPENDS}
    )
    
    ExternalProject_Get_Property(libquadmath_build SOURCE_DIR INSTALL_DIR)
    
    add_dependencies(libquadmath libquadmath_build)
    target_include_directories(libquadmath INTERFACE ${SOURCE_DIR}/libquadmath)
    target_link_libraries(libquadmath INTERFACE ${INSTALL_DIR}/lib/libquadmath.a)
    target_compile_options(libquadmath INTERFACE -Wno-c99-extensions)
    
    unset(SOURCE_DIR)
    unset(INSTALL_DIR)
else()
    target_link_libraries(libquadmath INTERFACE quadmath)
endif()
