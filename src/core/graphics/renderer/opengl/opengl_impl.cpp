#include <cstdio>
#include <cstddef>
#include <glad/glad.h>
#include <SDL3/SDL.h>

#include "core/graphics/renderer/opengl/opengl_impl.h"

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aUnitPos; // Quad (0,0 to 1,1)
    layout (location = 1) in vec4 aRect;    // Command: x, y, w, h
    layout (location = 2) in vec4 aColour;  // Command: r, g, b, a

    out vec4 vColor;

    void main() {
        // Apply transform: (UnitCoord * Size) + Position
        vec2 worldPos = (aUnitPos * aRect.zw) + aRect.xy;

        // Convert to NDC (-1 to 1) - You'll want a projection matrix here later!
        gl_Position = vec4(worldPos, 0.0, 1.0);
        vColor = aColour;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec4 vColor;
    out vec4 FragColor;
    void main() {
        FragColor = vColor;
    }
)";

static GLuint gl_CreateShaderProgram(const char* vertexSource, const char* fragmentSource)
{
    auto compileShader = [](GLenum type, const char* source) -> GLuint
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        return shader;
    };

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
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
    glGenBuffers(1, &gl->command_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gl->command_vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_RENDER_COMMANDS * sizeof(RenderCommand), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(RenderCommand), (void*)offsetof(RenderCommand, as.rect));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RenderCommand), (void*)offsetof(RenderCommand, colour));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
}

bool gl_CreateContext(Renderer* r, SDL_Window* window_handle)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
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

    gl_InitQuad(gl);
    gl_InitCommandBuffer(gl);

    gl->shader_program = gl_CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

    return true;
}

void gl_Flush(struct Renderer* r)
{
    if (r->command_count == 0) return;

    OpenGLData* gl = &r->backend_data.gl;

    glUseProgram(gl->shader_program);
    glBindVertexArray(gl->vao);

    glBindBuffer(GL_ARRAY_BUFFER, gl->command_vbo);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(RenderCommand) * r->command_count,
        r->commands
        );

    glDrawElementsInstanced(
        GL_TRIANGLES,
        gl->index_count,
        GL_UNSIGNED_SHORT,
        0,
        r->command_count
        );

    r->command_count = 0;
}

void gl_OnResize(Renderer* r, int width, int height)
{
    glViewport(0, 0, width, height);
}