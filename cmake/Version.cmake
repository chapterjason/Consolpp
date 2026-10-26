set(CONSOLPP_GENERATED_DIR "${PROJECT_BINARY_DIR}/generated")

configure_file("${PROJECT_SOURCE_DIR}/cmake/Version.hpp.in" "${CONSOLPP_GENERATED_DIR}/Consolpp/Version.hpp" @ONLY)

add_library(consolpp_version INTERFACE)
add_library(Consolpp::version ALIAS consolpp_version)
target_include_directories(consolpp_version INTERFACE "${CONSOLPP_GENERATED_DIR}")
