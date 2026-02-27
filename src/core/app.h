#ifndef PET_APP_H
#define PET_APP_H

#include <SDL3/SDL.h>

#include "core/graphics/renderer/renderer.h"
#include "core/scene/camera/orthographic.h"

struct Palette
{
    uint32_t paddle_one_mat;
    uint32_t paddle_two_mat;
    uint32_t ball_mat;
    uint32_t background_mat;
    uint32_t net_mat;
    uint32_t ui_mat;

    uint32_t standard_shader;
    uint32_t effect_shader;
    uint32_t net_shader;
};

struct GameData
{
    CameraOrthographic camera;
    Vec2 paddle[2];
    Vec2 ball_position;
    Vec2 ball_velocity;
    Palette palette;
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