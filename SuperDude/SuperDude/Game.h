#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <SDL_ttf.h>
#include <iostream>
#include <vector>

// constants

// system core
const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

// game elements
const int PADDLE_WIDTH = 70;
const int PADDLE_HEIGHT = 80;
const double PADDLE_SPEED = 6.8;

const int BALL_SIZE = 10;
const int BALL_SPEED = 5;

class Game {
public:
	Game();
	~Game();

	bool init();
	void quit();
	
	void handleEvents();
	void update();
	void render();
	

	bool running() {
		return isRunning;
	}
private:
	bool isRunning;
	SDL_Window* window;
	SDL_Renderer* renderer;
};