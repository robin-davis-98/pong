cmake_minimum_required(VERSION 4.1)
project(pong)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_CODE_DIR ${CMAKE_SOURCE_DIR}/src/)
set(VENDOR_CODE_DIR ${CMAKE_SOURCE_DIR}/vendors/)

add_subdirectory(vendors)

add_executable(pong
        ${SOURCE_CODE_DIR}main.cpp
)

target_link_libraries(pong PRIVATE
        SDL3::SDL3-static
        glad
)
