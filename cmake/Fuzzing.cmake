option(CONSOLPP_FUZZ "Build the fuzz targets, needs Clang for libFuzzer" OFF)

if(CONSOLPP_FUZZ)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(FATAL_ERROR "Fuzzing needs Clang for libFuzzer, not ${CMAKE_CXX_COMPILER_ID}")
    endif()

    # Global, so the fuzzer sees the coverage of the code under test and not only of the fuzz targets.
    add_compile_options(-fsanitize=fuzzer-no-link)
    add_link_options(-fsanitize=fuzzer-no-link)
endif()

function(consolpp_add_fuzz name library)
    if(NOT CONSOLPP_FUZZ)
        return()
    endif()

    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE Consolpp::options ${library})
    target_link_options(${name} PRIVATE -fsanitize=fuzzer)
    add_test(NAME ${name} COMMAND ${name} -max_total_time=10)
endfunction()
