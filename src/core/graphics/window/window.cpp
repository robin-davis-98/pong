#include <cstdio>
#include <SDL3/SDL_init.h>

#include "core/graphics/window/window.h"


bool window_Create(Window* window)
{
    if (!window)
    {
        return false;
    }

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "SDL Failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    window->window_handle = SDL_CreateWindow(
        "Pong",
        800,
        600,
        SDL_WINDOW_OPENGL
        );

    if (!window->window_handle)
    {
        fprintf(stderr, "Window could not be created! SDL Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    return true;
}
