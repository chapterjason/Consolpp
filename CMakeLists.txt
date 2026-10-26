cmake_minimum_required(VERSION 3.25)

project(Consolpp VERSION 0.1.0 LANGUAGES C CXX)

if(NOT CMAKE_SYSTEM_NAME MATCHES "^(Linux|Darwin)$")
    message(FATAL_ERROR "Consolpp supports Linux and macOS only, not ${CMAKE_SYSTEM_NAME}")
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
    message(FATAL_ERROR "Consolpp supports GCC and Clang only, not ${CMAKE_CXX_COMPILER_ID}")
endif()

if(NOT CMAKE_C_COMPILER_ID STREQUAL CMAKE_CXX_COMPILER_ID)
    message(FATAL_ERROR "C and C++ compiler must be the same, not ${CMAKE_C_COMPILER_ID} and ${CMAKE_CXX_COMPILER_ID}")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

include("${PROJECT_SOURCE_DIR}/cmake/Ccache.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/Linker.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/CPM.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/Dependencies.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/CompilerOptions.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/Hardening.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/ClangTidy.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/Coverage.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/Testing.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/Benchmarking.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/Fuzzing.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/Version.cmake")

add_subdirectory(libs/Text)
add_subdirectory(libs/Markup)
add_subdirectory(libs/Console)

if(PROJECT_IS_TOP_LEVEL OR CONSOLPP_FUZZ)
    add_subdirectory(tests)
endif()

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(tools)
endif()

# Only GCC implements the C++26 reflection the demo uses.
if(PROJECT_IS_TOP_LEVEL AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_subdirectory(demo)
endif()

consolpp_coverage_report()
