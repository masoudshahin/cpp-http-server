cmake_minimum_required(VERSION 3.15)

project(HttpServer)

set(CMAKE_CXX_STANDARD 17)

add_executable(server
    src/main.cpp
)