#include "app.h"

#include <cstdio>

void app_Init(App* app, Window* window)
{
    app->renderer = {};
    app->renderer.backend_type = RendererBackend::OPENGL;
    app->renderer.material_count = 0;
    app->gfx = renderer_init(&app->renderer, window);

    app->last_time = SDL_GetTicks();
    app->running = true;
}

void app_Tick(App* app)
{
    uint64_t now = SDL_GetTicks();
    app->delta_time = (now - app->last_time) / 1000.0f;

    if (app->delta_time > 0.1f) app->delta_time = 0.1f;

    app->total_time += app->delta_time;
    app->last_time = now;

    app->renderer.total_time = app->total_time;
}

void app_Update(App* app)
{
    app_Tick(app);

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_EVENT_QUIT)
        {
            app->running = false;
        }

        if (e.type == SDL_EVENT_WINDOW_RESIZED)
        {
            app->gfx.on_resize(&app->renderer, e.window.data1, e.window.data2);
            app->window_width = e.window.data1;
            app->window_height = e.window.data2;

            app->game.camera.aspect_ratio = (float)e.window.data1 / (float)e.window.data2;
            camera_UpdateOrtho(&app->game.camera);
        }
    }
}

void app_Draw(App* app)
{
    app->gfx.begin_frame(&app->renderer);
    /*
    //TODO: BACKGROUND TARGET MISSING
    app->gfx.set_render_target(&app->renderer, &app->renderer.background_target);
    */
    renderer_SetCamera(&app->renderer, app->game.camera.projection);

    float bg_width = app->game.camera.aspect_ratio * 2.0f;
    renderer_SubmitRect(&app->renderer,
        -app->game.camera.aspect_ratio, -1.0f,
        bg_width, 2.0f,
        app->game.palette.background_mat);

    renderer_SubmitRect(&app->renderer,
        -0.005f, -1.0f,
        0.01f, 2.0f,
        app->game.palette.net_mat);

    // Paddle 1 (Left)
    renderer_SubmitRect(&app->renderer,
        app->game.paddle[0].x, app->game.paddle[0].y,
        0.045f, 0.2f,
        app->game.palette.paddle_one_mat);

    // Paddle 2 (Right)
    renderer_SubmitRect(&app->renderer,
        app->game.paddle[1].x, app->game.paddle[1].y,
        0.045f, 0.2f,
        app->game.palette.paddle_two_mat);

    // Draw Ball
    //TODO: REPLACE WITH SUBMIT CIRCLE WHEN ADDED
    renderer_SubmitCircle(&app->renderer,
        app->game.ball_position.x, app->game.ball_position.y,
        0.02f,
        app->game.palette.ball_mat);

    app->gfx.flush(&app->renderer);
}

void app_Shutdown(App* app)
{
    if (app->gfx.shutdown)
    {
        app->gfx.shutdown(&app->renderer);
    }

    printf("App shutdown cleanly after %.2f seconds \n", app->total_time);
}