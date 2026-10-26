option(CONSOLPP_CLANG_TIDY "Run clang-tidy while compiling" OFF)

if(CONSOLPP_CLANG_TIDY)
    # clang-tidy reads the compiler flags and rejects GCC-only ones.
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|AppleClang)$")
        message(FATAL_ERROR "clang-tidy requires building with Clang")
    endif()
    find_program(CLANG_TIDY_PROGRAM clang-tidy REQUIRED)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_PROGRAM}")
endif()
