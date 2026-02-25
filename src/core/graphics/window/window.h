#ifndef PONG_WINDOW_H
#define PONG_WINDOW_H
#include <SDL3/SDL_video.h>

struct WindowProperties
{
    int32_t width, height;
    uint32_t flags;
};

struct Window
{
    WindowProperties properties;
    SDL_Window* window_handle;
};

bool window_Create(Window* window);

#endif