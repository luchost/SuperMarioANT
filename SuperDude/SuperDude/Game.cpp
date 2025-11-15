#include "Game.h"
#include "TextureManager.h"
#include "Platform.hpp"
#include"Collision.hpp"

#include<vector>
SDL_Texture* playertex;
SDL_Texture* blocktex;
double pY = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
double pX = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
bool Grounded = false;
Platform platforms[3];
//blok x =171 y=0;	160 160
//Mario 0 491 140 200
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

	window = SDL_CreateWindow("SuperDude", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return false;
	}

	blocktex = TextureManager::LoadTexture("assets/blocks.png", renderer);
		platforms[0].setPos({ 0,300,70,30 });
	platforms[1].setPos({ 130,250,100,30 });
	platforms[2].setPos({ 450,350,120,30 });
	platforms[0].setTex(blocktex);
	platforms[1].setTex(blocktex); 
	platforms[2].setTex(blocktex);
	SDL_Rect tmp = { 171 + 360,0,160,160 };
	platforms[0].setCrop(tmp);
	platforms[1].setCrop(tmp);
	platforms[2].setCrop(tmp);
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
	if (state[SDL_SCANCODE_W] && pY > 0 && Grounded) {
		dY -= PADDLE_SPEED;
	}
	if (state[SDL_SCANCODE_S] && pY + PADDLE_HEIGHT < SCREEN_HEIGHT) {
		pY += PADDLE_SPEED;
	}
	if (state[SDL_SCANCODE_A] && pX > 0) {
		pX -= PADDLE_SPEED;
	}
	if (state[SDL_SCANCODE_D] && pX +PADDLE_WIDTH < SCREEN_WIDTH) {
		pX += PADDLE_SPEED;
	}
}

void Game::update()
{
	Grounded = false;
	pY += dY;
	dY += 0.25;
	SDL_Rect paddle1 = { pX, pY, PADDLE_WIDTH, PADDLE_HEIGHT };
	SDL_Rect floor = {0,SCREEN_HEIGHT,SCREEN_WIDTH,1};
	for (int i = 0; i < 3; i++) {
		if (Collision::CollideOnTop(paddle1, platforms[i].getPos())) {
			Grounded = true;
			if (dY > 0) { dY = 0; }
			if (pY > platforms[i].getPos().y - PADDLE_HEIGHT) {

				pY = platforms[i].getPos().y - PADDLE_HEIGHT;
			}

		}
		else if (Collision::Collide(paddle1, platforms[i].getPos())) {
			if (pX - platforms[i].getPos().x >= 0)
			{
				pX = platforms[i].getPos().x + platforms[i].getPos().w;
			}
			else {
				pX = platforms[i].getPos().x - PADDLE_WIDTH;
			
			}
		}
	}
	if (Collision::Collide(paddle1, floor)) {
		isRunning = false;
	
	}
	
}

void Game::render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, playertex, NULL, NULL);
	for (int i = 0; i < 3; i++) {
		SDL_Rect tmp = platforms[i].getPos();
		SDL_Rect crop = platforms[i].getCrop();
		SDL_RenderCopy(renderer, platforms[i].getTex(), &crop, &tmp);
	}

	SDL_Rect paddle1 = {pX, pY, PADDLE_WIDTH, PADDLE_HEIGHT};
	SDL_Rect mario = { 0,491,140,200 };
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderCopy(renderer,blocktex, &mario,&paddle1);



	SDL_RenderPresent(renderer);
}

void Game::quit() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
