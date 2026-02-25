#include "core/graphics/renderer/renderer.h"
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
            };
        } break;

        default:
        {

        }break;
    }

    return render_functions;
}

void renderer_SubmitRect(Renderer* r, float x, float y, float w, float h, Colour colour)
{
    if (r->command_count >= MAX_RENDER_COMMANDS) return;

    RenderCommand* cmd = &r->commands[r->command_count++];

    cmd->type = RenderCommandType::COMMAND_RECT;
    cmd->colour = colour;
    cmd->as.rect = {
        .x = x,
        .y = y,
        .w = w,
        .h = h,
    };
}