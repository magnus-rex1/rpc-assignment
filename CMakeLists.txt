cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(rpc CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_STANDARD 11)

add_compile_options(-Wall -Wextra -pedantic -fdiagnostics-color)

add_executable(server server.cpp socket.cpp)
add_executable(client client.cpp socket.cpp)
