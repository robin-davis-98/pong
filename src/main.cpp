#include <stdio.h>

#include <SDL3/SDL.h>

#include "core/app.h"
#include "core/graphics/renderer/renderer.h"
#include "core/graphics/window/window.h"
#include "core/scene/camera/orthographic.h"


void setup_Game(App* app, const Window* window)
{
    int32_t w, h;
    SDL_GetWindowSize(window->window_handle, &w, &h);

    app->game.camera = {};
    app->game.camera.position = { 0.0f, 0.0f };
    app->game.camera.zoom = 1.0f;
    app->game.camera.aspect_ratio = (float)w / (float)h;
    camera_UpdateOrtho(&app->game.camera);

    app->game.palette.standard_shader = 0;
    app->game.palette.effect_shader = app->gfx.load_shader(
        &app->renderer,
        "Data/shaders/background.vert",
        "Data/shaders/background.frag"
        );
    app->game.palette.net_shader = app->gfx.load_shader(
        &app->renderer,
        "Data/shaders/net.vert",
        "Data/shaders/net.frag"
        );


    Colour white = colour_from_rgb(255, 255, 255);
    Colour grey = colour_from_rgb(125, 125, 125);
    Colour bg_tint = colour_from_rgb(124, 24, 60);

    app->game.palette.paddle_one_mat = renderer_CreateMaterial(&app->renderer, app->game.palette.standard_shader, white);
    app->game.palette.paddle_two_mat = renderer_CreateMaterial(&app->renderer, app->game.palette.standard_shader, white);
    app->game.palette.ball_mat       = renderer_CreateMaterial(&app->renderer, 1, white);
    app->game.palette.background_mat = renderer_CreateMaterial(&app->renderer, app->game.palette.effect_shader, bg_tint);
    app->game.palette.net_mat        = renderer_CreateMaterial(&app->renderer, app->game.palette.net_shader, grey);

    float aspect = app->game.camera.aspect_ratio;
    app->game.paddle[0] = { -aspect + 0.1f, -0.15f };
    app->game.paddle[1] = {  aspect - 0.15f, -0.15f };
    app->game.ball_position = { 0.0f, 0.0f };
    app->game.ball_velocity = { 0.6f, 0.4f };
}

int main(int argc, char** argv)
{
    Window window = {};
    if (!window_Create(&window))
    {
        fprintf(stderr, "Failed to create window\n");
        return -1;
    }

    App app = {};
    app.game = {};

    app_Init(&app, &window);
    setup_Game(&app, &window);

    while (app.running)
    {
        app_Update(&app);
        app_Draw(&app);

        SDL_GL_SwapWindow(window.window_handle);
        SDL_Delay(1);
    }

    app_Shutdown(&app);
    SDL_DestroyWindow(window.window_handle);
    SDL_Quit();
}