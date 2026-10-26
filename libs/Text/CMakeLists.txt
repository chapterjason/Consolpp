add_library(consolpp_text INTERFACE)
add_library(Consolpp::text ALIAS consolpp_text)
target_include_directories(consolpp_text INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
