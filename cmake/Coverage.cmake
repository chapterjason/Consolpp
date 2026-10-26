option(CONSOLPP_COVERAGE "Instrument for code coverage" OFF)

if(CONSOLPP_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        find_program(GCOVR_PROGRAM gcovr REQUIRED)
        target_compile_options(consolpp_options INTERFACE --coverage)
        target_link_options(consolpp_options INTERFACE --coverage)
    else()
        # The raw profile format changes between LLVM versions, so the tools must come with the compiler.
        if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            execute_process(
                COMMAND xcrun --find clang++
                OUTPUT_VARIABLE compiler
                OUTPUT_STRIP_TRAILING_WHITESPACE
                COMMAND_ERROR_IS_FATAL ANY
            )
        else()
            set(compiler "${CMAKE_CXX_COMPILER}")
        endif()
        file(REAL_PATH "${compiler}" compiler)
        cmake_path(GET compiler PARENT_PATH compiler_directory)
        find_program(LLVM_PROFDATA_PROGRAM llvm-profdata HINTS "${compiler_directory}" NO_DEFAULT_PATH REQUIRED)
        find_program(LLVM_COV_PROGRAM llvm-cov HINTS "${compiler_directory}" NO_DEFAULT_PATH REQUIRED)
        unset(compiler)
        unset(compiler_directory)
        set(CONSOLPP_PROFILE_DIR "${CMAKE_BINARY_DIR}/profraw")
        target_compile_options(consolpp_options INTERFACE
            -fprofile-instr-generate=${CONSOLPP_PROFILE_DIR}/%p.profraw
            -fcoverage-mapping
        )
        target_link_options(consolpp_options INTERFACE -fprofile-instr-generate=${CONSOLPP_PROFILE_DIR}/%p.profraw)
    endif()
endif()

function(consolpp_coverage_report)
    if(NOT CONSOLPP_COVERAGE)
        return()
    endif()

    set(report_dir "${CMAKE_BINARY_DIR}/report")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(template [=[
file(GLOB_RECURSE data "@CMAKE_BINARY_DIR@/*.gcda")
if(NOT data)
    message(FATAL_ERROR "No coverage data in @CMAKE_BINARY_DIR@, run the program first")
endif()
file(REMOVE_RECURSE "@report_dir@")
file(MAKE_DIRECTORY "@report_dir@")
execute_process(
    COMMAND "@GCOVR_PROGRAM@" --root "@PROJECT_SOURCE_DIR@" --filter libs/ --html-details "@report_dir@/index.html" "@CMAKE_BINARY_DIR@"
    WORKING_DIRECTORY "@PROJECT_SOURCE_DIR@"
    COMMAND_ERROR_IS_FATAL ANY
)
message(STATUS "Coverage report: @report_dir@/index.html")
]=])
    else()
        get_property(tests GLOBAL PROPERTY CONSOLPP_TEST_TARGETS)
        set(objects "")
        foreach(target IN LISTS ARGN tests)
            string(APPEND objects " \"--object=$<TARGET_FILE:${target}>\"")
        endforeach()
        set(profdata "${CMAKE_BINARY_DIR}/coverage.profdata")
        set(template [=[
file(GLOB profiles "@CONSOLPP_PROFILE_DIR@/*.profraw")
if(NOT profiles)
    message(FATAL_ERROR "No coverage data in @CONSOLPP_PROFILE_DIR@, run the program first")
endif()
file(REMOVE_RECURSE "@report_dir@")
execute_process(
    COMMAND "@LLVM_PROFDATA_PROGRAM@" merge -sparse ${profiles} -o "@profdata@"
    COMMAND_ERROR_IS_FATAL ANY
)
execute_process(
    COMMAND "@LLVM_COV_PROGRAM@" show@objects@ "--instr-profile=@profdata@" --format=html "--output-dir=@report_dir@" "--sources=@PROJECT_SOURCE_DIR@/libs"
    COMMAND_ERROR_IS_FATAL ANY
)
message(STATUS "Coverage report: @report_dir@/index.html")
]=])
    endif()

    string(CONFIGURE "${template}" script @ONLY)
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/coverage-report.cmake" CONTENT "${script}")

    add_custom_target(coverage-report
        COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/coverage-report.cmake"
        VERBATIM
    )
endfunction()
