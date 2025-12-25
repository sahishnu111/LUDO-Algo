cmake_minimum_required(VERSION 3.27)
project(Ludo_Game)

set(CMAKE_CXX_STANDARD 20)
find_package(Threads REQUIRED)

add_executable(Ludo_Server server.cpp
        Player.cpp
        Player.h
        Game.cpp
        Game.h
        Board.cpp
        Board.h
        Computer.cpp
        Computer.h
        GameManager.cpp
        GameManager.h)
        
target_link_libraries(Ludo_Server PRIVATE Threads::Threads)

# Copy web directory to build folder
add_custom_command(TARGET Ludo_Server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/web
    ${CMAKE_BINARY_DIR}/web)

# Benchmark Target
add_executable(Ludo_Benchmark benchmark.cpp
        Player.cpp
        Game.cpp
        Board.cpp
        GameManager.cpp)
target_link_libraries(Ludo_Benchmark PRIVATE Threads::Threads)
