#include "Visualizer.hpp"

#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>

bool Visualizer::init()
{
	return _initSDL() && _initGL();
}

void Visualizer::run()
{
	while (_running) {
		_handleEvents();
		
		// Step simulation here

		_renderFrame();
	}
}

void Visualizer::quit()
{
	_running = false;
}

// Initializes the SDL window and the attached OpenGL context
bool Visualizer::_initSDL()
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
bool Visualizer::_initGL()
{
	return true;
}

// Handles keyboard and mouse inputs
void Visualizer::_handleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		// ESC key press
		if (event.type == SDL_WINDOWEVENT &&
			event.window.event == SDL_WINDOWEVENT_CLOSE
		) {
			quit();
		}
	}
}

// Renders a single frame to the window
void Visualizer::_renderFrame()
{
	// OpenGL draw calls here

	SDL_GL_SwapWindow(_window);
}

Visualizer::~Visualizer()
{
	SDL_DestroyWindow(_window);
	SDL_Quit();
}
