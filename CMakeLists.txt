cmake_minimum_required(VERSION 4.1)
project(pong)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_CODE_DIR ${CMAKE_SOURCE_DIR}/src/)
set(INCLUDE_CODE_DIR ${CMAKE_SOURCE_DIR}/include/)
set(VENDOR_CODE_DIR ${CMAKE_SOURCE_DIR}/vendors/)

set(COMMON_CODE_DIR ${SOURCE_CODE_DIR}common/)

set(COMMON_SOURCES
    ${COMMON_CODE_DIR}petrichor_math.cpp
)

set(CORE_CODE_DIR ${SOURCE_CODE_DIR}core/)
set(GRAPHICS_CODE_DIR ${CORE_CODE_DIR}graphics/)

set(GRAPHICS_SOURCES
    ${GRAPHICS_CODE_DIR}window/window.cpp
    ${GRAPHICS_CODE_DIR}renderer/renderer.cpp
)

set(PLATFORM_SOURCES
    ${GRAPHICS_CODE_DIR}renderer/opengl/opengl_impl.cpp
)

set(SCENE_CODE_DIR ${CORE_CODE_DIR}scene/)
set(CAMERA_CODE_DIR ${SCENE_CODE_DIR}camera/)

set(SCENE_SOURCES
    ${CAMERA_CODE_DIR}orthographic.cpp
    ${CAMERA_CODE_DIR}perspective.cpp
)

set(CORE_SOURCES
        ${CORE_CODE_DIR}app.cpp
        ${GRAPHICS_SOURCES}
        ${PLATFORM_SOURCES}

        ${SCENE_SOURCES}
)


# SHADER GENERATION FOR PRIMITIVES =======================================
set(GEN_DIR "${CMAKE_BINARY_DIR}/generated_shaders")
file(MAKE_DIRECTORY ${GEN_DIR})

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

        add_custom_command(
                OUTPUT "${OUT_FILE}"
                COMMAND ${CMAKE_COMMAND}
                    -DINPUT_FILE=${SHADER_PATH}
                    -DOUTPUT_FILE=${OUT_FILE}
                    -DVAR_NAME=${VAR_NAME}
                    -P "${CMAKE_SOURCE_DIR}/cmake/EmbedShader.cmake"
                DEPENDS "${SHADER_PATH}" "${CMAKE_SOURCE_DIR}/cmake/EmbedShader.cmake"
                COMMENT "Embedding shader ${FILE_NAME}${FILE_EXT}"
                VERBATIM
        )

        list(APPEND SHADER_HEADERS ${OUT_FILE})
    endforeach()
endforeach()

# ========================================================================

add_subdirectory(vendors)

if(WIN32)
    add_executable(pong WIN32
        ${SOURCE_CODE_DIR}main.cpp

        ${COMMON_SOURCES}
        ${CORE_SOURCES}

        ${SHADER_HEADERS}
    )
else()
    add_executable(pong
        ${SOURCE_CODE_DIR}main.cpp

        ${COMMON_SOURCES}
        ${CORE_SOURCES}

        ${SHADER_HEADERS}
    )
endif()

target_include_directories(pong
    PUBLIC
        ${INCLUDE_CODE_DIR}
        ${VENDOR_CODE_DIR}
    PRIVATE
        ${SOURCE_CODE_DIR}
        ${GEN_DIR}
)

target_link_libraries(pong PRIVATE
        SDL3::SDL3-static
        glad
)


# ASSET DEPLOYMENT =======================================================
set(DATA_SRC_DIR "${CMAKE_SOURCE_DIR}/Data")
set(DATA_DST_DIR "$<TARGET_FILE_DIR:pong>/Data")

# 1. Create a custom target for the assets
add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${DATA_SRC_DIR}"
        "${DATA_DST_DIR}"
        COMMENT "Syncing Data folder to build directory..."
        VERBATIM
)

add_dependencies(pong copy_assets)
# ========================================================================