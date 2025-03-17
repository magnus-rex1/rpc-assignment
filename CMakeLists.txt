cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(rpc CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
# set(CMAKE_CXX_STANDARD 11)

# add_compile_options(-Wall -Wextra -pedantic -fdiagnostics-color -fanalyzer)
add_compile_options(-Wall -Wextra -pedantic -fdiagnostics-color)

add_executable(
    server # target. 'server' is the executable
    serverApp.cpp
    socket.cpp
    utils.cpp
    Message.cpp
    RPCMessage.cpp
    Server.cpp
)

add_executable(
    client # target 'client' is the executable
    clientApp.cpp
    socket.cpp
    utils.cpp
    Message.cpp
    RPCMessage
    Client.cpp
)

