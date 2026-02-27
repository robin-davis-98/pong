#include <cstdio>
#include <cstddef>
#include <glad/glad.h>
#include <SDL3/SDL.h>

#include "core/graphics/renderer/opengl/opengl_impl.h"

#include "opengl_quad_frag.h"
#include "opengl_quad_vert.h"
#include "opengl_circle_frag.h"
#include "opengl_circle_vert.h"

static GLuint gl_CreateShaderProgram(const char* vertexSource, const char* fragmentSource)
{
    auto compileShader = [](GLenum type, const char* source) -> GLuint
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            fprintf(stderr, "Shader Error: %s\n", infoLog);
        }

        return shader;
    };

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        fprintf(stderr, "Shader Linking Error: %s\n", infoLog);
        return 0; // Return 0 so we know it failed
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

uint32_t gl_LoadShader(Renderer* r, const char* vertexPath, const char* fragmentPath)
{
    auto readFile = [](const char* path) -> char* {
        FILE* file = fopen(path, "rb");
        if (!file) {
            fprintf(stderr, "Failed to open shader file: %s\n", path);
            return nullptr;
        }
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        char* buffer = (char*)malloc(length + 1);
        fread(buffer, 1, length, file);
        buffer[length] = '\0';
        fclose(file);
        return buffer;
    };

    char* vertSrc = readFile(vertexPath);
    char* fragSrc = readFile(fragmentPath);

    if (!vertSrc || !fragSrc) {
        free(vertSrc); free(fragSrc);
        return 0; // Fallback to default shader
    }

    GLuint program = gl_CreateShaderProgram(vertSrc, fragSrc);
    free(vertSrc);
    free(fragSrc);

    OpenGLData* gl = &r->backend_data.gl;

    // Find next slot or use a count
    uint32_t id = 0;
    for(int i = 0; i < MAX_SHADERS; i++) {
        if(gl->shader_programs[i] == 0) {
            id = i;
            break;
        }
    }

    gl->shader_programs[id] = program;
    return id;
}

void gl_InitQuad(OpenGLData* gl)
{
    float quad_vertices[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };
    uint16_t quad_indices[] = { 0, 1, 2, 2, 3, 0};

    uint32_t quad_vbo, quad_ebo;
    glGenVertexArrays(1, &gl->vao);
    glGenBuffers(1, &quad_vbo);
    glGenBuffers(1, &quad_ebo);

    glBindVertexArray(gl->vao);

    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void gl_InitCommandBuffer(OpenGLData* gl)
{
    glBindVertexArray(gl->vao);

    glGenBuffers(1, &gl->command_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gl->command_vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_RENDER_COMMANDS * sizeof(RenderCommand), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(
        1, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(RenderCommand),
        (void*)offsetof(RenderCommand, as.rect)
        );
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glVertexAttribIPointer(
        2, 1,
        GL_UNSIGNED_INT, sizeof(RenderCommand),
        (void*)offsetof(RenderCommand, material_id)
        );
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
}

bool gl_CreateContext(Renderer* r, SDL_Window* window_handle)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GLContext context = SDL_GL_CreateContext(window_handle);
    if (!context) {
        fprintf(stderr, "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_MakeCurrent(window_handle, context);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return false;
    }

    OpenGLData* gl = &r->backend_data.gl;
    gl->index_count = 6;

    glGenBuffers(1, &gl->material_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, gl->material_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, gl->material_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    gl->shader_programs[0] = gl_CreateShaderProgram(opengl_quad_vert, opengl_quad_frag);
    gl->shader_programs[1] = gl_CreateShaderProgram(opengl_circle_vert, opengl_circle_frag);

    gl_InitQuad(gl);
    gl_InitCommandBuffer(gl);


    return true;
}

void gl_Flush(struct Renderer* r)
{
    if (r->command_count == 0) return;
    OpenGLData* gl = &r->backend_data.gl;

    //UPLOAD MATERIALS
    glBindBuffer(GL_UNIFORM_BUFFER, gl->material_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0,
        sizeof(Material) * r->material_count, r->materials);
    glBindVertexArray(gl->vao);

    //UPLOAD COMMANDS
    glBindBuffer(GL_ARRAY_BUFFER, gl->command_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
        sizeof(RenderCommand) * r->command_count, r->commands);

    glBindVertexArray(gl->vao);

    uint32_t start = 0;
    while (start < r->command_count)
    {
        uint32_t mat_id = r->commands[start].material_id;
        uint32_t shader_id = r->materials[mat_id].shader_id;

        int32_t batch_size = 0;
        for (uint32_t i = start; i < r->command_count; i++)
        {
            if (r->materials[r->commands[i].material_id].shader_id != shader_id)
            {
                break;
            }

            batch_size++;
        }

        glUseProgram(gl->shader_programs[shader_id]);

        int proj_loc = glGetUniformLocation(gl->shader_programs[shader_id], "uProjection");
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, r->camera_matrix.m);

        int time_loc = glGetUniformLocation(gl->shader_programs[shader_id], "uTime");
        if (time_loc != -1) {
            glUniform1f(time_loc, r->total_time);
        }

        uint32_t texture_handle = r->materials[mat_id].texture_id;
        if (texture_handle != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_handle);

            int text_loc = glGetUniformLocation(gl->shader_programs[shader_id], "uTexture");
            if (text_loc != -1) glUniform1i(text_loc, 0);
        }

        glDrawElementsInstancedBaseInstance(
            GL_TRIANGLES, 6,
            GL_UNSIGNED_SHORT, 0,
            batch_size, start);

        start += batch_size;
    }

    r->command_count = 0;
}

void gl_OnResize(Renderer* r, int width, int height)
{
    r->window_width = width;
    r->window_height = height;

    glViewport(0, 0, width, height);
}

void gl_BeginFrame(Renderer* r)
{
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gl_Shutdown(Renderer* r)
{
    OpenGLData* gl = &r->backend_data.gl;

    glDeleteBuffers(1, &gl->material_ubo);
    glDeleteBuffers(1, &gl->command_vbo);

    glDeleteVertexArrays(1, &gl->vao);

    for (int i = 0; i < MAX_SHADERS; i++)
    {
        if (gl->shader_programs[i] != 0)
        {
            glDeleteProgram(gl->shader_programs[i]);
        }
    }
}

void gl_SetRenderTarget(Renderer* r, RenderTarget* target)
{
    uint32_t fbo = target ? target->handle : 0;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    if (target)
    {
        glViewport(0, 0, target->width, target->height);
    } else {
        glViewport(0, 0, r->window_width, r->window_height);
    }
}

void gl_CreateRenderTarget(RenderTarget* target, int w, int h)
{
    target->width = w;
    target->height = h;

    glGenFramebuffers(1, &target->handle);
    glBindFramebuffer(GL_FRAMEBUFFER, target->handle);

    glGenTextures(1, &target->texture);
    glBindTexture(GL_TEXTURE_2D, target->texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target->texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void gl_DestroyRenderTarget(RenderTarget* target)
{
    glDeleteFramebuffers(1, &target->handle);
    glDeleteTextures(1, &target->texture);
}