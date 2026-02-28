#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

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
        "Data/shaders/base.vert",
        "Data/shaders/background.frag"
        );
    app->game.palette.net_shader = app->gfx.load_shader(
        &app->renderer,
        "Data/shaders/base.vert",
        "Data/shaders/net.frag"
        );
    app->game.palette.trail_shader = app->gfx.load_shader(
        &app->renderer,
        "Data/shaders/base.vert",
        "Data/shaders/streak.frag"
        );

    uint32_t font_texture = app->gfx.load_texture("Data/images/font-atlas.png");

    Colour paddle_tint = colour_from_hex("#f2d3ab");
    Colour ball_tint = colour_from_hex("#fbf5ef");
    Colour bg_tint = colour_from_hex("#272744");
    Colour net_tint = colour_from_hex("#494d7e");
    Colour trail_tint = colour_from_hex("#FFFFFF");

    app->game.palette.paddle_one_mat = renderer_CreateMaterial(&app->renderer, app->game.palette.standard_shader, paddle_tint);
    app->game.palette.paddle_two_mat = renderer_CreateMaterial(&app->renderer, app->game.palette.standard_shader, paddle_tint);
    app->game.palette.ball_mat       = renderer_CreateMaterial(&app->renderer, 1, ball_tint);
    app->game.palette.background_mat = renderer_CreateMaterial(&app->renderer, app->game.palette.effect_shader, bg_tint);
    app->game.palette.net_mat        = renderer_CreateMaterial(&app->renderer, app->game.palette.net_shader, net_tint);
    app->game.palette.trail_mat      = renderer_CreateMaterial(&app->renderer, app->game.palette.trail_shader, trail_tint);

    app->game.palette.font_mat = renderer_CreateMaterial(&app->renderer, 2, colour_from_hex("#FFFFFF"));
    app->renderer.materials[app->game.palette.font_mat].texture_id = font_texture;

    float aspect = app->game.camera.aspect_ratio;
    app->game.paddle[0] = { -aspect + 0.1f, -0.15f };
    app->game.paddle[1] = {  aspect - 0.15f, -0.15f };
    app->game.ball_position = { 0.0f, 0.0f };
    app->game.ball_velocity = { 0.6f, 0.4f };
}

void loop_Game(App* app) {
    app->game.ball_position.x += app->game.ball_velocity.x * app->delta_time;
    app->game.ball_position.y += app->game.ball_velocity.y * app->delta_time;

    if (app->game.ball_position.y >= 1.0f || app->game.ball_position.y <= -1.0f)
    {
        app->game.ball_velocity.y *= -1.0f;
    }

    const bool* keys = SDL_GetKeyboardState(NULL);

    float ball_radius = 0.02f;
    float paddle_w = 0.045f;
    float paddle_h = 0.2f;

    float paddle_speed = 1.5f;
    float distance_to_ball = abs((app->game.paddle[0].y + (paddle_h / 2.0f)) - app->game.ball_position.y);

    if (!app->game.ball_served) {
        // Stick the ball to the front of the left paddle
        app->game.ball_position.x = app->game.paddle[0].x + paddle_w + ball_radius;
        app->game.ball_position.y = app->game.paddle[0].y + (paddle_h / 2.0f);

        if (keys[SDL_SCANCODE_SPACE]) {
            app->game.ball_served = true;
            app->game.ball_velocity = { 0.8f, 0.4f }; // Launch!
        }
    }

    if (app->game.ball_position.x < (app->game.paddle[0].x + 0.5f) && distance_to_ball > 0.05f) {
        paddle_speed *= 1.4f; // Function to speed up player as they get closer to ball
    }

    if (keys[SDL_SCANCODE_W]) app->game.paddle[0].y += paddle_speed * app->delta_time;
    if (keys[SDL_SCANCODE_S]) app->game.paddle[0].y -= paddle_speed * app->delta_time;
    app->game.paddle[0].y = SDL_clamp(app->game.paddle[0].y, -1.0f, 1.0f - paddle_h);

    float ai_center = app->game.paddle[1].y + (paddle_h / 2.0f);
    float ai_speed = 1.2f;

    if (app->game.ball_served && app->game.ball_velocity.x > 0) {
        if (ai_center < app->game.ball_position.y - 0.02f) {
            app->game.paddle[1].y += ai_speed * app->delta_time;
        } else if (ai_center > app->game.ball_position.y + 0.02f) {
            app->game.paddle[1].y -= ai_speed * app->delta_time;
        }
    }
    app->game.paddle[1].y = SDL_clamp(app->game.paddle[1].y, -1.0f, 1.0f - paddle_h);

    if (app->game.ball_served) {
        app->game.trail_timer += app->delta_time;
        if (app->game.trail_timer > 0.02f) { // Record every 20ms
            app->game.trail_timer = 0;
            for (int i = MAX_TRAIL_POINTS - 1; i > 0; i--) {
                app->game.ball_history[i] = app->game.ball_history[i-1];
            }
            app->game.ball_history[0] = app->game.ball_position;
        }

        app->game.ball_position.x += app->game.ball_velocity.x * app->delta_time;
        app->game.ball_position.y += app->game.ball_velocity.y * app->delta_time;

        // Bounce Top/Bottom
        if (abs(app->game.ball_position.y) >= 1.0f - ball_radius) {
            app->game.ball_velocity.y *= -1.0f;
        }

        // Paddle Collisions (Simplified AABB)
        for (int i = 0; i < 2; i++) {
            float px = app->game.paddle[i].x;
            float py = app->game.paddle[i].y;

            if (app->game.ball_position.x + ball_radius > px &&
                app->game.ball_position.x - ball_radius < px + paddle_w &&
                app->game.ball_position.y + ball_radius > py &&
                app->game.ball_position.y - ball_radius < py + paddle_h)
            {
                app->game.ball_velocity.x *= -1.05f; // Speed up
                float hit_pos = (app->game.ball_position.y - py) / paddle_h;
                app->game.ball_velocity.y = (hit_pos - 0.5f) * 2.0f;
                app->game.hit_count++;
            }
        }

        // Goal Reset
        float aspect = app->game.camera.aspect_ratio;
        if (app->game.ball_position.x > aspect + 0.1f) {
            app->game.score_player++;
            app->game.ball_served = false;
            app->game.hit_count = 0;     // Reset trail
        }
        else if (app->game.ball_position.x < -aspect - 0.1f) {
            app->game.score_ai++;
            app->game.ball_served = false;
            app->game.hit_count = 0;     // Reset trail
        }
    }
}

int main(int argc, char* argv[])
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

        loop_Game(&app);

        SDL_GL_SwapWindow(window.window_handle);
        SDL_Delay(1);
    }

    app_Shutdown(&app);
    SDL_DestroyWindow(window.window_handle);
    SDL_Quit();
}