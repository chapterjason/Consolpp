add_executable(consolpp_screen
    main.cpp
    Screen/Emulator.cpp
    Screen/Record.cpp
    Screen/Replay.cpp
)
target_include_directories(consolpp_screen PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(consolpp_screen PRIVATE Consolpp::options Consolpp::console)

# forkpty() lives in libutil on Linux and in libc on macOS.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(consolpp_screen PRIVATE util)
endif()
