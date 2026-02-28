#ifndef PET_APP_H
#define PET_APP_H

#include <SDL3/SDL.h>

#include "core/graphics/renderer/renderer.h"
#include "core/scene/camera/orthographic.h"

#define MAX_TRAIL_POINTS 10

struct Palette
{
    uint32_t paddle_one_mat;
    uint32_t paddle_two_mat;
    uint32_t ball_mat;
    uint32_t background_mat;
    uint32_t net_mat;
    uint32_t trail_mat;
    uint32_t font_mat;

    uint32_t standard_shader;
    uint32_t effect_shader;
    uint32_t net_shader;
    uint32_t trail_shader;
};

struct GameData
{
    CameraOrthographic camera;
    Palette palette;
    Vec2 paddle[2];
    Vec2 ball_position;
    Vec2 ball_velocity;
    bool ball_served;
    Vec2 ball_history[MAX_TRAIL_POINTS];
    float trail_scale;
    float trail_timer;
    int hit_count;

    int score_player;
    int score_ai;
};

struct App
{
    bool running;
    int32_t window_width, window_height;

    Renderer renderer;
    RenderFunctions gfx;

    uint64_t last_time;
    float delta_time;
    float total_time;

    GameData game;
};

void app_Init(App* app, Window* window);
void app_Update(App* app);
void app_Draw(App* app);
void app_Shutdown(App* app);

#endif