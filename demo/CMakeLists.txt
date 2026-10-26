add_executable(consolpp_demo main.cpp)
target_include_directories(consolpp_demo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(consolpp_demo PRIVATE Consolpp::options Consolpp::console)
# Demo::write reads every member of an object through C++26 reflection.
set_target_properties(consolpp_demo PROPERTIES CXX_STANDARD 26)
target_compile_options(consolpp_demo PRIVATE -freflection)

add_executable(consolpp_demo_single Single/main.cpp)
target_include_directories(consolpp_demo_single PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(consolpp_demo_single PRIVATE Consolpp::options Consolpp::console)
