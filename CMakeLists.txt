cmake_minimum_required(VERSION 4.1)
project(pong)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_CODE_DIR ${CMAKE_SOURCE_DIR}/src/)
set(INCLUDE_CODE_DIR ${CMAKE_SOURCE_DIR}/include/)
set(VENDOR_CODE_DIR ${CMAKE_SOURCE_DIR}/vendors/)

set(CORE_CODE_DIR ${SOURCE_CODE_DIR}core/)
set(GRAPHICS_CODE_DIR ${CORE_CODE_DIR}graphics/)

set(GRAPHICS_SOURCES
    ${GRAPHICS_CODE_DIR}window/window.cpp
    ${GRAPHICS_CODE_DIR}renderer/renderer.cpp
)

set(PLATFORM_SOURCES
    ${GRAPHICS_CODE_DIR}renderer/opengl/opengl_impl.cpp
)


set(GEN_DIR "${CMAKE_BINARY_DIR}/generated_shaders")
file(MAKE_DIRECTORY ${GEN_DIR})

function(embed_shader SHADER_FILE HEADER_FILE VARIABLE_NAME)
    if (EXISTS ${SHADER_FILE})
        file(READ ${SHADER_FILE} SHADER_CONTENTS)
        set(HEADER_CONTENTS "/* Generated from ${SHADER_FILE} */\n#pragma once\n\nconst char* ${VARIABLE_NAME} = R\"(\n${SHADER_CONTENTS}\n)\";")
        file(WRITE ${HEADER_FILE} "${HEADER_CONTENTS}")
    endif()
endfunction()

set(ACTIVE_BACKENDS "opengl")

if (WIN32)
    list(APPEND ACTIVE_BACKENDS)
endif()

foreach(BACKEND ${ACTIVE_BACKENDS})
    set(SHADER_SEARCH_DIR "${GRAPHICS_CODE_DIR}renderer/${BACKEND}/shaders")

    file(GLOB SHADER_FILES "${SHADER_SEARCH_DIR}/*.vert" "${SHADER_SEARCH_DIR}/*.frag")

    foreach(SHADER_PATH ${SHADER_FILES})
        get_filename_component(FILE_NAME ${SHADER_PATH} NAME_WLE)
        get_filename_component(FILE_EXT ${SHADER_PATH} EXT)
        string(SUBSTRING ${FILE_EXT} 1 -1 EXT_NAME)

        set(VAR_NAME "${BACKEND}_${FILE_NAME}_${EXT_NAME}")
        set(OUT_FILE "${GEN_DIR}/${VAR_NAME}.h")

        embed_shader(${SHADER_PATH} ${OUT_FILE} ${VAR_NAME})
        list(APPEND SHADER_HEADERS ${OUT_FILE})
    endforeach()
endforeach()

add_subdirectory(vendors)

add_executable(pong
        ${SOURCE_CODE_DIR}main.cpp

        ${GRAPHICS_SOURCES}
        ${PLATFORM_SOURCES}
)

target_include_directories(pong
    PUBLIC ${INCLUDE_CODE_DIR}
    PRIVATE
        ${SOURCE_CODE_DIR}
        ${GEN_DIR}
)

target_link_libraries(pong PRIVATE
        SDL3::SDL3-static
        glad
)