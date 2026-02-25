#include <stdio.h>

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "core/graphics/renderer/renderer.h"
#include "core/graphics/window/window.h"

struct player_pos {
    float x, y;
};

struct target
{
    float x, y;
};

void run(bool& running, Window& window, Renderer& renderer, RenderFunctions& gfx)
{
    SDL_Event e;
    player_pos current_pos = { 0.0f, 0.0f };
    target target_pos = {
        0.0f, 0.0f
    };

    float lerp_speed = 5.0f;

    uint64_t last_time = SDL_GetTicks();

    int w, h;
    SDL_GetWindowSize(window.window_handle, &w, &h);

    while (running)
    {
        uint64_t now = SDL_GetTicks();
        float dt = (now - last_time) / 1000.0f; // Convert milliseconds to seconds
        last_time = now;

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                running = false;
            } else if (e.type == SDL_EVENT_WINDOW_RESIZED)
            {
                gfx.on_resize(&renderer, e.window.data1, e.window.data2);
                w = e.window.data1;
                h = e.window.data2;
            } else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                // Update the TARGET, not the current position
                target_pos.x = (e.button.x / (float)w) * 2.0f - 1.0f;
                target_pos.y = 1.0f - (e.button.y / (float)h) * 2.0f;

                // Center the square on the cursor
                target_pos.x -= 0.025f;
                target_pos.y -= 0.025f;
            }
        }

        current_pos.x += (target_pos.x - current_pos.x) * lerp_speed * dt;
        current_pos.y += (target_pos.y - current_pos.y) * lerp_speed * dt;

        glClearColor(
          (float)renderer.clear_colour.r / 255.0f,
          (float)renderer.clear_colour.g / 255.0f,
          (float)renderer.clear_colour.b / 255.0f,
          1.0f
        );
        glClear(GL_COLOR_BUFFER_BIT);

        renderer_SubmitRect(&renderer, current_pos.x, current_pos.y, 0.05f, 0.05f, {255, 255, 255, 255}); // Left Paddle

        gfx.flush(&renderer);
        SDL_GL_SwapWindow(window.window_handle);
    }
}

int main(int argc, char** argv)
{

    Window window = {};
    if (!window_Create(&window))
    {
        fprintf(stderr, "Failed to create window\n");
    }

    Renderer renderer = {};
    renderer.backend_type = RendererBackend::OPENGL;
    renderer.clear_colour = {50, 50, 45, 255};

    RenderFunctions gfx = renderer_init(&renderer, &window);

    bool isRunning = true;

    run(isRunning, window, renderer, gfx);

    SDL_DestroyWindow(window.window_handle);
    SDL_Quit();
}