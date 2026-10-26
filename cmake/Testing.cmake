if(PROJECT_IS_TOP_LEVEL)
    enable_testing()
    include(GoogleTest)

    consolpp_add_package(
        NAME googletest
        GITHUB_REPOSITORY google/googletest
        VERSION 1.18.0
        OPTIONS "INSTALL_GTEST OFF"
    )
endif()

function(consolpp_add_test name library)
    if(NOT PROJECT_IS_TOP_LEVEL)
        return()
    endif()

    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE Consolpp::options ${library} GTest::gmock GTest::gtest_main)
    gtest_discover_tests(${name})
    set_property(GLOBAL APPEND PROPERTY CONSOLPP_TEST_TARGETS ${name})
endfunction()
