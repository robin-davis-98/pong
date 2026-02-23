cmake_minimum_required(VERSION 4.1)
project(pong)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_CODE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_executable(pong
        ${SOURCE_CODE_DIR}main.cpp
)
