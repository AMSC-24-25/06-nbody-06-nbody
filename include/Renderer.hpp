#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <memory>

#include <SDL2/SDL.h>

#include "ShaderProgram.hpp"

class Renderer
{
public:
    Renderer(int window_width,
             int window_height) :
        _window_width(window_width),
        _window_height(window_height),
        _window_title("nbody") {}

    bool init();
    void run();
    void quit();

    ~Renderer();

private:
    bool _init();
    bool _loadShaders();

    void _allocBuffers();
    void _setupScene();

    void _handleEvents();
    void _renderFrame();

    bool _running = true;
    bool _initialized = false;

    // TODO: create separate Window class
    SDL_Window *_window;
    int _window_width;
    int _window_height;
    std::string _window_title;

    GLuint _quad_vao;
    GLuint _particles_ssbo;

    std::unique_ptr<ShaderProgram> _shader_program;
};

#endif
