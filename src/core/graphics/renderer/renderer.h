#ifndef PONG_RENDERER_H
#define PONG_RENDERER_H

#define MAX_RENDER_COMMANDS 4096
#define MAX_SHADERS 16
#define MAX_MATERIALS 8

#include <SDL3/SDL_video.h>

#include "common/util/colour.h"
#include "core/graphics/window/window.h"
#include "petrichor_math.h"

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
    // The command type which
    COMMAND_RECT,
    COMMAND_CIRCLE,
};

struct RenderCommand
{
    union
    {
        struct {float x, y, w, h; } rect;
    } as;

    RenderCommandType type;
    uint32_t material_id;
    uint32_t _pad[2];
};

struct RenderTarget
{
    uint32_t handle;
    uint32_t texture;
    int32_t width, height;
};

struct Texture
{

};

struct Material
{
    alignas(16) Colour albedo;
    uint32_t shader_id;
    uint32_t texture_id;
    uint32_t _pad[2];
};

struct OpenGLData
{
    uint32_t shader_programs[MAX_SHADERS];
    uint32_t shader_count;
    uint32_t material_ubo;
    uint32_t vao;
    uint32_t command_vbo;
    uint32_t index_count;
};

struct Renderer
{
    Material materials[MAX_MATERIALS];
    uint32_t material_count;

    RenderCommand commands[MAX_RENDER_COMMANDS];
    uint32_t command_count;

    Mat4 camera_matrix;

    float total_time;
    int32_t window_width, window_height;

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
    void (*begin_frame)(struct Renderer* r);
    void (*shutdown)(struct Renderer* r);

    uint32_t (*load_shader)(Renderer* r, const char* vertexPath, const char* fragmentPath);
    void (*set_render_target)(Renderer* r, RenderTarget* target);
};

RenderFunctions renderer_init(Renderer* renderer, Window* window);

void renderer_SetCamera(Renderer* r, Mat4 matrix);

// Primitives
void renderer_SubmitRect(Renderer* r, float x, float y, float w, float h, uint32_t material_id);
void renderer_SubmitCircle(Renderer* r, float x, float y, float radius, uint32_t material_id);

uint32_t renderer_CreateMaterial(Renderer* r, uint32_t shader_id, Colour colour);
void renderer_UpdateMaterial(Renderer* r, uint32_t material_id, Colour colour);

#endif