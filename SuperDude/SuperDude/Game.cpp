#include "Game.h"
#include "TextureManager.h"
#include"Collision.hpp"
SDL_Texture* playertex;
double pY = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
double pX = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
bool Grounded = false;
double dY = 0;



Game::Game() {

}
Game::~Game() {

}

bool Game::init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return false;
	}

	playertex = TextureManager::LoadTexture("assets/rickroll.png", renderer);
	return isRunning = true;
}

void Game::handleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	default:
		break;
	}

	// Move paddles
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_SPACE] && pY > 0 && Grounded) {
		dY -= PADDLE_SPEED;
	}
	if (state[SDL_SCANCODE_S] && pY + PADDLE_HEIGHT < SCREEN_HEIGHT) {
		pY += PADDLE_SPEED;
	}
	if (state[SDL_SCANCODE_A] && pX > 0) {
		pX -= PADDLE_SPEED;
	}
	if (state[SDL_SCANCODE_D] && pX + PADDLE_HEIGHT < SCREEN_HEIGHT) {
		pX += PADDLE_SPEED;
	}
}

void Game::update()
{
	Grounded = false;
	pY += dY;
	dY += 0.25;
	SDL_Rect paddle1 = { pX, pY, PADDLE_WIDTH, PADDLE_HEIGHT };
	SDL_Rect floor = {0,300,SCREEN_WIDTH,10};

	if (Collision::Collide(paddle1, floor)) {
		Grounded = true;
		if (dY > 0) { dY = 0; }
		if (pY > floor.y - PADDLE_HEIGHT) {
		
			pY = floor.y - PADDLE_HEIGHT;
		}

	}
	
}

void Game::render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, playertex, NULL, NULL);

	SDL_Rect paddle1 = {pX, pY, PADDLE_WIDTH, PADDLE_HEIGHT};
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &paddle1);



	SDL_RenderPresent(renderer);
}

void Game::quit() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
