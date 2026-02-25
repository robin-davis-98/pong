#ifndef PONG_OPENGL_IMPL_H
#define PONG_OPENGL_IMPL_H

#include "core/graphics/renderer/renderer.h"

bool gl_CreateContext(Renderer* r, SDL_Window* window_handle);
void gl_Flush(struct Renderer* r);
void gl_OnResize(Renderer* r, int width, int height);

#endif