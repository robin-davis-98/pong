#ifndef PONG_RENDERER_H
#define PONG_RENDERER_H

#define MAX_RENDER_COMMANDS 4096

#include <SDL3/SDL_video.h>

#include "common/util/colour.h"
#include "core/graphics/window/window.h"

enum class RendererBackend : uint32_t
{
    OPENGL = 0,
    VULKAN,
    DX11,
    DX12,
    SOFTWARE,
    NOT_IMPLEMENTED,
    COUNT
};

enum class RenderCommandType
{
    COMMAND_RECT,
};

struct RenderCommand
{
    RenderCommandType type;
    Colour colour;

    union
    {
        struct {float x, y, w, h; } rect;
    } as;
};

struct OpenGLData
{
    uint32_t shader_program;
    uint32_t vao;
    uint32_t command_vbo;
    uint32_t index_count;
};

struct Renderer
{
    RenderCommand commands[MAX_RENDER_COMMANDS];
    uint32_t command_count;

    Colour clear_colour;

    union
    {
        OpenGLData gl;
    } backend_data;

    RendererBackend backend_type;
};

struct RenderFunctions
{
    void (*flush)(struct Renderer* r);
    void (*on_resize)(struct Renderer* r, int width, int height);
};

RenderFunctions renderer_init(Renderer* renderer, Window* window);
void renderer_SubmitRect(Renderer* r, float x, float y, float w, float h, Colour colour);

#endif