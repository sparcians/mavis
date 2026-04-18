include(ExternalProject)

set(SOFTFLOAT_SPECIALIZE_TYPE "RISCV")
set(SOFTFLOAT_BUILD_CONFIG "Linux-x86_64-GCC")
set(SOFTFLOAT_BUILD_CONFIG_DIR build/${SOFTFLOAT_BUILD_CONFIG})

ExternalProject_Add(
    softfloat_build
    GIT_REPOSITORY "https://github.com/ucb-bar/berkeley-softfloat-3.git"
    GIT_TAG a0c6494cdc11865811dec815d5c0049fba9d82a8
    UPDATE_DISCONNECTED True
    CONFIGURE_COMMAND ""
    BUILD_COMMAND $(MAKE) -C <SOURCE_DIR>/${SOFTFLOAT_BUILD_CONFIG_DIR} SPECIALIZE_TYPE=${SOFTFLOAT_SPECIALIZE_TYPE}
    BUILD_IN_SOURCE True
    INSTALL_COMMAND cp <SOURCE_DIR>/${SOFTFLOAT_BUILD_CONFIG_DIR}/softfloat.a <INSTALL_DIR>
)

ExternalProject_Get_Property(softfloat_build SOURCE_DIR)
ExternalProject_Get_Property(softfloat_build INSTALL_DIR)

add_library(softfloat INTERFACE)
add_dependencies(softfloat softfloat_build)
target_link_libraries(softfloat INTERFACE ${INSTALL_DIR}/softfloat.a)
target_compile_definitions(softfloat INTERFACE SOFTFLOAT_FAST_INT64)
target_compile_definitions(softfloat INTERFACE LITTLEENDIAN=1)
target_compile_definitions(softfloat INTERFACE INLINE=inline)
target_compile_definitions(softfloat INTERFACE SOFTFLOAT_BUILTIN_CLZ=1)
target_compile_definitions(softfloat INTERFACE SOFTFLOAT_INTRINSIC_INT128=1)
target_include_directories(softfloat INTERFACE ${SOURCE_DIR}/source/${SOFTFLOAT_SPECIALIZE_TYPE} ${SOURCE_DIR}/source/include ${SOURCE_DIR}/${SOFTFLOAT_BUILD_CONFIG_DIR})

unset(SOURCE_DIR)
unset(INSTALL_DIR)
