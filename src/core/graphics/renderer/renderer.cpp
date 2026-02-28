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
                .load_texture = gl_LoadTexture,
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
    cmd->rotation = 0.0f;
}

void renderer_SubmitRectRotated(Renderer* r, float x, float y, float w, float h, float rotation, uint32_t material_id) {
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
    cmd->rotation = rotation;
}

void renderer_SubmitRectUV(Renderer *r, float x, float y, float w, float h, float ux, float uy, float uw, float uh, uint32_t material_id) {
    if (r->command_count >= MAX_RENDER_COMMANDS) return;

    RenderCommand* cmd = &r->commands[r->command_count++];
    cmd->type = RenderCommandType::COMMAND_RECT;
    cmd->material_id = material_id;
    cmd->as.rect = {x, y, w, h};
    cmd->uv = {ux, uy, uw, uh};
    cmd->rotation = 0.0f;
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
    cmd->rotation = 0.0f;
}

void renderer_SubmitText(Renderer *r, const char *text, float x, float y, float size, uint32_t material_id) {
    float cur_x = x;
    const float cell = 1.0f / 16.0f;

    for (int i = 0; text[i] != '\0'; i++) {
        unsigned char c = (unsigned char)text[i];
        if (c == ' ') { cur_x += size * 0.5f; continue; }

        float u = (float)(c % 16) * cell;

        // FLIP THE V COORDINATE HERE
        // Instead of starting from the top, we start from the bottom
        // Logic: 1.0 - (row + 1) * cell_height
        float v = 1.0f - ((float)(c / 16) * cell) - cell;

        if (r->command_count < MAX_RENDER_COMMANDS) {
            RenderCommand* cmd = &r->commands[r->command_count++];
            cmd->type = RenderCommandType::COMMAND_TEXT;
            cmd->material_id = material_id;
            cmd->as.rect = { cur_x, y, size, size };
            cmd->uv = { u, v, cell, cell }; // UVs: X, Y, Width, Height
            cmd->rotation = 0.0f;
        }
        cur_x += size * 0.45f;
    }
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
