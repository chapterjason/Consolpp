option(CONSOLPP_CCACHE "Cache compilations with ccache or sccache" ON)

if(CONSOLPP_CCACHE)
    find_program(CCACHE_PROGRAM NAMES ccache sccache REQUIRED)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()
