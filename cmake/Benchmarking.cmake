if(PROJECT_IS_TOP_LEVEL)
    consolpp_add_package(
        NAME benchmark
        GITHUB_REPOSITORY google/benchmark
        VERSION 1.10.0
        OPTIONS
            "BENCHMARK_ENABLE_TESTING OFF"
            "BENCHMARK_ENABLE_GTEST_TESTS OFF"
            "BENCHMARK_ENABLE_INSTALL OFF"
            "BENCHMARK_INSTALL_DOCS OFF"
            "BENCHMARK_ENABLE_WERROR OFF"
            "BENCHMARK_ENABLE_LIBPFM OFF"
    )
endif()

function(consolpp_add_benchmark name library)
    if(NOT PROJECT_IS_TOP_LEVEL)
        return()
    endif()

    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE Consolpp::options ${library} benchmark::benchmark)
endfunction()
