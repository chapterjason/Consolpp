include(CheckIPOSupported)

option(CONSOLPP_NATIVE "Optimise for the CPU of the build machine" OFF)
set(CONSOLPP_SANITIZERS "" CACHE STRING "Sanitizers to build with: address, undefined, thread, memory, leak")

check_ipo_supported(RESULT CONSOLPP_IPO_SUPPORTED OUTPUT CONSOLPP_IPO_ERROR LANGUAGES CXX)
if(NOT CONSOLPP_IPO_SUPPORTED)
    message(FATAL_ERROR "Link time optimisation is not supported: ${CONSOLPP_IPO_ERROR}")
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)

# Applied to Consolpp's own targets only, so dependencies keep their own flags.
add_library(consolpp_options INTERFACE)
add_library(Consolpp::options ALIAS consolpp_options)

target_compile_options(consolpp_options INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    # Members are always accessed through this->, so parameters and locals may share a member's name.
    $<$<CXX_COMPILER_ID:GNU>:-Wshadow=local>
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wshadow-field>
    -Wconversion
    -Wsign-conversion
    -Wold-style-cast
    -Wnon-virtual-dtor
    -Woverloaded-virtual
    -Wcast-align
    -Wcast-qual
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
    -Wmissing-declarations
    -Wundef
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast>
    -Werror

    $<$<CONFIG:Debug>:-g3 -fno-omit-frame-pointer>
    $<$<CONFIG:RelWithDebInfo>:-fno-omit-frame-pointer>
    $<$<CONFIG:Release,RelWithDebInfo>:-ffunction-sections -fdata-sections>
    $<$<AND:$<CONFIG:Release,RelWithDebInfo>,$<PLATFORM_ID:Linux>>:-fno-plt>
)

target_compile_definitions(consolpp_options INTERFACE
    $<$<CONFIG:Debug>:_GLIBCXX_ASSERTIONS _LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG>
)

target_link_options(consolpp_options INTERFACE
    $<$<AND:$<CONFIG:Release,RelWithDebInfo>,$<PLATFORM_ID:Linux>>:LINKER:-O1,--as-needed,--gc-sections>
    $<$<AND:$<CONFIG:Release,RelWithDebInfo>,$<PLATFORM_ID:Darwin>>:LINKER:-dead_strip>
)

if(CONSOLPP_NATIVE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
        target_compile_options(consolpp_options INTERFACE -mcpu=native)
    else()
        target_compile_options(consolpp_options INTERFACE -march=native)
    endif()
endif()

set(CONSOLPP_KNOWN_SANITIZERS address undefined thread memory leak)
foreach(sanitizer IN LISTS CONSOLPP_SANITIZERS)
    if(NOT sanitizer IN_LIST CONSOLPP_KNOWN_SANITIZERS)
        message(FATAL_ERROR "Unknown sanitizer: ${sanitizer}")
    endif()
endforeach()

if("thread" IN_LIST CONSOLPP_SANITIZERS AND ("address" IN_LIST CONSOLPP_SANITIZERS OR "leak" IN_LIST CONSOLPP_SANITIZERS OR "memory" IN_LIST CONSOLPP_SANITIZERS))
    message(FATAL_ERROR "The thread sanitizer cannot be combined with the address, leak or memory sanitizer")
endif()

if("memory" IN_LIST CONSOLPP_SANITIZERS)
    if("address" IN_LIST CONSOLPP_SANITIZERS OR "leak" IN_LIST CONSOLPP_SANITIZERS)
        message(FATAL_ERROR "The memory sanitizer cannot be combined with the address or leak sanitizer")
    endif()
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
        message(FATAL_ERROR "The memory sanitizer requires Clang on Linux")
    endif()
endif()

# Global, so dependencies are instrumented too; the thread and memory sanitizers need that.
if(CONSOLPP_SANITIZERS)
    list(JOIN CONSOLPP_SANITIZERS "," sanitizers)
    add_compile_options(
        -fsanitize=${sanitizers}
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls
    )
    add_link_options(-fsanitize=${sanitizers})
    if("undefined" IN_LIST CONSOLPP_SANITIZERS)
        add_compile_options(-fno-sanitize-recover=undefined)
    endif()
    if("memory" IN_LIST CONSOLPP_SANITIZERS)
        add_compile_options(-fsanitize-memory-track-origins=2)
    endif()
endif()
