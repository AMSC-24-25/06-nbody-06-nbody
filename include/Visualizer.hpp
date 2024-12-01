#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>

#include <SDL2/SDL.h>

class Visualizer
{
public:
	Visualizer(int window_width,
			   int window_height,
			   const std::string window_title) :
		_window_width(window_width),
		_window_height(window_height),
		_window_title(window_title) {}

	bool init();
	void run();
	void quit();

	~Visualizer();

private:
	bool _initSDL();
	bool _initGL();

	void _handleEvents();
	void _renderFrame();

	bool _running = true;

	// TODO: create separate Window class
	SDL_Window *_window;
	int _window_width;
	int _window_height;
	std::string _window_title;
};

#endif
