#ifndef PET_OPENGL_IMPL_H
#define PET_OPENGL_IMPL_H

#include "core/graphics/renderer/renderer.h"

bool gl_CreateContext(Renderer* r, SDL_Window* window_handle);
void gl_Flush(struct Renderer* r);
void gl_OnResize(Renderer* r, int width, int height);
void gl_BeginFrame(Renderer* r);
void gl_Shutdown(Renderer* r);
uint32_t gl_LoadShader(Renderer* r, const char* vertexPath, const char* fragmentPath);

#endif