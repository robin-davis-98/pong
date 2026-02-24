#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <stdio.h>

#include "SDL3/SDL_opengl.h"

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
)";

const char* backgroundFragSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }
)";

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "SDL Failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow(
        "Pong",
        800,
        600,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
        );

    if (!window)
    {
        fprintf(stderr, "Window could not be created! SDL Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context)
    {
        fprintf(stderr, "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
    }

    SDL_GL_MakeCurrent(window, context);
    SDL_GL_SwapWindow(window);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return 1;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint backgroundFragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(backgroundFragShader, 1, &backgroundFragSource, NULL);
    glCompileShader(backgroundFragShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


    GLuint backgroundShader = glCreateProgram();
    glAttachShader(backgroundShader, vertexShader);
    glAttachShader(backgroundShader, backgroundFragShader);
    glLinkProgram(backgroundShader);

    float vertices[] = {
        -0.05f,  0.2f, 0.0f,
         0.05f,  0.2f, 0.0f,
         0.05f, -0.2f, 0.0f,

        -0.05f,  0.2f, 0.0f,
         0.05f,  -0.2f, 0.0f,
        -0.05f, -0.2f, 0.0f,
    };

    float background[] = {
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
    };

    GLuint backgroundArray, backgroundBuffer;
    glGenVertexArrays(1, &backgroundArray);
    glGenBuffers(1, &backgroundBuffer);

    glBindVertexArray(backgroundArray);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundBuffer);
    glBufferData(GL_ARRAY_BUFFER,sizeof(background), background, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    bool isRunning = true;
    SDL_Event e;

    while (isRunning)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                isRunning = false;
            }
        }

        uint8_t r = 50;
        uint8_t g = 50;
        uint8_t b = 45;

        glClearColor(((float)r/255.0f), ((float)g/255.0f), ((float)b/255.0f), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(backgroundShader);
        glBindVertexArray(backgroundArray);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}