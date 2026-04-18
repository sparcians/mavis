include(ExternalProject)

ExternalProject_Add(
    simde_build
    GIT_REPOSITORY "https://github.com/simd-everywhere/simde.git"
    GIT_TAG v0.8.4-rc3
    UPDATE_DISCONNECTED True
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

ExternalProject_Get_Property(simde_build INSTALL_DIR)

add_library(simde INTERFACE)
add_dependencies(simde simde_build)
target_include_directories(simde INTERFACE ${INSTALL_DIR}/include)

unset(INSTALL_DIR)
