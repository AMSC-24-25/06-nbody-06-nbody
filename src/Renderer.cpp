#include "Renderer.hpp"

#include <iostream>
#include <array>
#include <cassert>
#include <cstdlib>
#include <cmath>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "ShaderProgram.hpp"

constexpr unsigned int N = 5;

bool Renderer::init()
{
    return _initialized = _init() && _loadShaders();
}

void Renderer::run()
{
    assert(_initialized);

    _allocBuffers();
    _setupScene();

    // Quick and dirty simulation to test rendering (BAD CODE FOLLOWS)

    std::array<std::array<float, 4>, N> pos; // pos[i][3] holds mass
    std::array<std::array<float, 2>, N> vel;
    std::array<std::array<float, 2>, N> forces;
    const float dt = 1e-4;

    for (unsigned int i = 0; i < N; ++i) {
        pos[i][0] = (rand() % 800 - 400) / 500.0;
        pos[i][1] = (rand() % 800 - 400) / 500.0;
        pos[i][2] = 0.0;
        pos[i][3] = 1.0f;
        // Setting intial velocity to be
        // perpendicular to position
        float vx = -pos[i][1];
        float vy = pos[i][0];
        float mag = sqrt(vx * vx + vy * vy);
        vel[i][0] = vx / mag * 100;
        vel[i][1] = vy / mag * 100;
    }

    pos[0][0] = 0.0f;
    pos[0][1] = 0.0f;
    vel[0][0] = 0.0f;
    vel[0][1] = 0.0f;
    pos[0][3] = 500.0f;
    pos[1][3] = 2.0f;

    while (_running) {
        _handleEvents();

        for (unsigned int i = 0; i < N; ++i) {

            forces[i][0] = 0.0;
            forces[i][1] = 0.0;

            for (unsigned int j = 0; j < N; ++j) {
                if (i == j) continue;

                float dist_x = pos[j][0] - pos[i][0];
                float dist_y = pos[j][1] - pos[i][1];

                float dist = sqrt(dist_x * dist_x + dist_y * dist_y) + 1e-12;
                float rad_x = dist_x / dist;
                float rad_y = dist_y / dist;

                forces[i][0] += 100.0 * pos[i][3] * pos[j][3] /
                    dist * dist * rad_x;
                forces[i][1] += 100.0 * pos[i][3] * pos[j][3] /
                    dist * dist * rad_y;
            }
        }

        for (unsigned int i = 0; i < N; ++i) {
            // Backwards euler
            vel[i][0] += forces[i][0] / pos[i][3] * dt;
            vel[i][1] += forces[i][1] / pos[i][3] * dt;

            pos[i][0] += vel[i][0] * dt;
            pos[i][1] += vel[i][1] * dt;
        }

        // Copying updated positions to GPU memory.
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     sizeof(pos),
                     pos.data(),
                     GL_DYNAMIC_DRAW);

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
// and to store particles positions
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

    // Sample particles data
    /*
    const std::array<float, 12>
    particles_data = {
         0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 1.0f,
         0.0f,  0.5f, 0.0f, 1.0f,
    };
    */

    // Creating a Shader Storage Buffer Object to store particles data.
    glGenBuffers(1, &_particles_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _particles_ssbo);
    // Copying particles data to GPU memory.
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _particles_ssbo);
    /*
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 sizeof(particles_data),
                 particles_data.data(),
                 GL_DYNAMIC_DRAW);
    */
}

// Sets OpenGL rendering options, arranges the scene
void Renderer::_setupScene()
{
    // Defines the rendering target area with
    // respect to the window coordinates
    glViewport(0, 0, _window_width, _window_height);
    // Sets clear color
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    // Enabling transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, N);

    SDL_GL_SwapWindow(_window);
}

Renderer::~Renderer()
{
    SDL_DestroyWindow(_window);
    SDL_Quit();
}
