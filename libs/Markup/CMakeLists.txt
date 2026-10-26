add_library(consolpp_markup INTERFACE)
add_library(Consolpp::markup ALIAS consolpp_markup)
target_include_directories(consolpp_markup INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(consolpp_markup INTERFACE Consolpp::text)
