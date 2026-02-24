project(glad)

set(GLAD_VENDOR ${VENDOR_CODE_DIR}glad/)
set(GLAD_SOURCES ${GLAD_VENDOR}src/)

add_library(glad STATIC
        ${GLAD_SOURCES}glad.c
)

target_include_directories(glad
        PUBLIC
            ${GLAD_VENDOR}/include
)