#include "core/graphics/renderer/renderer.h"

#include <cstdio>

#include "core/graphics/renderer/opengl/opengl_impl.h"

RenderFunctions renderer_init(Renderer* renderer, Window* window)
{
    RenderFunctions render_functions = {};

    switch (renderer->backend_type)
    {
        case RendererBackend::OPENGL:
        {
            gl_CreateContext(renderer, window->window_handle);

            render_functions = {
                .flush = gl_Flush,
                .on_resize = gl_OnResize,
                .begin_frame = gl_BeginFrame,
                .shutdown = gl_Shutdown,
                .load_shader = gl_LoadShader,
            };
        } break;

        default:
        {

        }break;
    }

    return render_functions;
}

void renderer_SubmitRect(Renderer* r, float x, float y, float w, float h, uint32_t material_id)
{
    if (r->command_count >= MAX_RENDER_COMMANDS) return;

    RenderCommand* cmd = &r->commands[r->command_count++];

    cmd->type = RenderCommandType::COMMAND_RECT;
    cmd->material_id = material_id;
    cmd->as.rect = {
        .x = x,
        .y = y,
        .w = w,
        .h = h,
    };
}

void renderer_SubmitCircle(Renderer* r, float x, float y, float radius, uint32_t material_id)
{
    if (r->command_count >= MAX_RENDER_COMMANDS) return;

    RenderCommand* cmd = &r->commands[r->command_count++];

    cmd->type = RenderCommandType::COMMAND_CIRCLE;
    cmd->material_id = material_id;
    cmd->as.rect = {
        .x = x - radius,
        .y = y - radius,
        .w = radius * 2.0f,
        .h = radius * 2.0f,
    };
}

void renderer_SetCamera(Renderer* r, Mat4 matrix) {
    r->camera_matrix = matrix;
}

uint32_t renderer_CreateMaterial(Renderer* r, uint32_t shader_id, Colour colour)
{
    if (r->material_count >= MAX_MATERIALS) return 0;

    uint32_t id = r->material_count++;
    r->materials[id].shader_id = shader_id;
    r->materials[id].texture_id = 0;
    r->materials[id].albedo = colour;

    return id;
}

void renderer_UpdateMaterial(Renderer* r, uint32_t material_id, Colour colour)
{
    r->materials[material_id].albedo = colour;
}
