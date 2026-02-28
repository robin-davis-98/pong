#include "app.h"

#include <cstdio>
#include <cstdlib> // For rand() and RAND_MAX
#include <ctime>   // For time() to seed the RNG

void app_Init(App* app, Window* window)
{
    srand((unsigned int)time(NULL));
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

    Mat4 render_matrix = app->game.camera.projection;
    renderer_SetCamera(&app->renderer, render_matrix);

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

    char score_l[8];
    char score_r[8];
    snprintf(score_l, sizeof(score_l), "%d", app->game.score_player);
    snprintf(score_r, sizeof(score_r), "%d", app->game.score_ai);

    // ... inside app_Draw ...

    float score_y = 0.7f;
    float score_size = 0.25f;
    float offset_from_center = 0.15f;

    // 1. Check your renderer.cpp! What is cur_x adding?
    // If your renderer.cpp uses `cur_x += size * 0.7f;`, this MUST be 0.7f.
    // If your renderer.cpp uses `cur_x += size * 0.5f;`, change this to 0.5f.
    float kerning_step = score_size * 0.7f;

    // 2. Calculate the TRUE visual width of the string
    int len = strlen(score_l);
    float true_width_l = (len - 1) * kerning_step + score_size;

    // 3. Anchor it perfectly
    float x_l = -offset_from_center - true_width_l;

    // Draw Left Score
    renderer_SubmitText(&app->renderer, score_l, x_l, score_y, score_size, app->game.palette.font_mat);

    // Draw Right Score
    renderer_SubmitText(&app->renderer, score_r, offset_from_center, score_y, score_size, app->game.palette.font_mat);

    if (app->game.ball_served && app->game.hit_count >= 2) {
        int growth = (app->game.hit_count - 2) / 2 + 1;
        int visible_segments = (growth < MAX_TRAIL_POINTS) ? growth : MAX_TRAIL_POINTS - 1;

        for (int i = 0; i < visible_segments; i++) {
            Vec2 p1 = app->game.ball_history[i];
            Vec2 p2 = app->game.ball_history[i+1];

            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            float dist = sqrtf(dx*dx + dy*dy);

            // Skip tiny segments to avoid flickering
            if (dist < 0.001f) continue;

            float angle = atan2f(dy, dx);

            // THICKNESS: Taper the whole ribbon
            // i=0 is at the ball (thickest), i=visible_segments is the tip (thinnest)
            float life_percent = (float)i / (float)visible_segments;
            float thickness = 0.03f * (1.0f - life_percent);

            renderer_SubmitRectRotated(&app->renderer,
                p1.x, p1.y,
                dist, thickness,
                angle,
                app->game.palette.trail_mat);
        }
    }

    // Draw Ball
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