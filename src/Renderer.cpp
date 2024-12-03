#include "Renderer.hpp"

#include <iostream>
#include <array>
#include <cassert>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "ShaderProgram.hpp"

	
bool Renderer::init()
{
    return _initialized = _init() && _loadShaders();
}

void Renderer::run()
{
    assert(_initialized);

    _allocBuffers();
    _setupScene();

    while (_running) {
        _handleEvents();

        // Step simulation here

        _renderFrame();
    }
}

void Renderer::quit()
{
    _running = false;
}

// Initializes the SDL window and the attached OpenGL context
bool Renderer::_init()
{
    // TODO: create utility to handle errors and logging

    // Initializing SDL2 video subsystem.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << SDL_GetError() << std::endl;
        return false;
    }

    // Using OpenGL 4.3 core profile.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    // Using double buffering.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Creating SDL2 window with OpenGL support.
    if ((_window = SDL_CreateWindow(
        _window_title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        _window_width,
        _window_height,
        SDL_WINDOW_OPENGL)) == nullptr) {
        std::cout << SDL_GetError() << std::endl;
        return false;
    }

    // Creating OpenGL context.
    if (SDL_GL_CreateContext(_window) == nullptr) {
        std::cout << SDL_GetError() << std::endl;
        return false;
    }

    // Loading OpenGL function pointers.
    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        std::cout << glewGetErrorString(glew_err) << std::endl;
        return false;
    }

    // Enabling v-sync.
    SDL_GL_SetSwapInterval(1);

    return true;
}

// Loads, compiles and links OpenGL shaders
bool Renderer::_loadShaders()
{
    _shader_program = std::make_unique<ShaderProgram>();

    return _shader_program->loadShader("shaders/particle.vert",
                                      GL_VERTEX_SHADER) &&
           _shader_program->loadShader("shaders/particle.frag",
                                      GL_FRAGMENT_SHADER) &&
           _shader_program->link();
}

// Allocates OpenGL buffers to draw a quadrilateral
void Renderer::_allocBuffers()
{
    // Vertices and indices used to draw a quad.
    const std::array<float, 12> 
    quad_vertices = {
        //  x,     y,    z
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };

    const std::array<unsigned int, 6>
    quad_indices = {
        0, 1, 2,
        0, 2, 3
    };

    // Creating a Vertex Array Object to handle quad vertices
    glGenVertexArrays(1, &_quad_vao);
    glBindVertexArray(_quad_vao);
    // Creating a Vertex Buffer Object to store vertices data.
    GLuint quad_vbo;
    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    // Copying vertices to GPU memory.
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(quad_vertices),
                 quad_vertices.data(),
                 GL_STATIC_DRAW);
    // Telling OpenGL to interpret the data stream as vertices of 3 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);

    // Creating a Element Buffer Object to store quad indices.
    GLuint quad_ebo;
    glGenBuffers(1, &quad_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ebo);
    // Copying indices to GPU memory.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(quad_indices),
                 quad_indices.data(),
                 GL_STATIC_DRAW);
}


// Sets OpenGL rendering options, arranges the scene
void Renderer::_setupScene()
{
    // Defines the rendering target area with
    // respect to the window coordinates
    glViewport(0, 0, _window_width, _window_height);
    // Sets clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}


// Handles keyboard and mouse inputs
void Renderer::_handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // ESC key press
        if ((event.type == SDL_WINDOWEVENT &&
             event.window.event == SDL_WINDOWEVENT_CLOSE) ||
            (event.type == SDL_KEYUP &&
             event.key.keysym.sym == SDLK_ESCAPE)
        ) {
            quit();
        }
    }
}

// Renders a single frame to the window
void Renderer::_renderFrame()
{
    _shader_program->use();

    glClear(GL_COLOR_BUFFER_BIT);
    // glBindVertexArray(_quad_vao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 100);

    SDL_GL_SwapWindow(_window);
}

Renderer::~Renderer()
{
    SDL_DestroyWindow(_window);
    SDL_Quit();
}
