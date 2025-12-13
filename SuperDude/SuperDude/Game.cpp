#include "Game.h"
#include "TextureManager.h"
#include "Platform.hpp"
#include"Collision.hpp"
#include<vector>

const int SPRITE_COLS = 5;
const int SPRITE_ROWS = 5;

const int tileW = 1000 / SPRITE_COLS;
const int tileH = 1080 / SPRITE_ROWS;

Mix_Music* music;
SDL_Texture* playertex;
SDL_Texture* blocktex;
SDL_Texture* mariotex;
double pY = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
double pX = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
double cameraX = 0;
double cameraY = 0;

bool Grounded = false;
Platform* platforms[7];
SDL_Rect MarioCrop = { tileW,tileH,tileW,tileH - 40 };
int rotation = 1;
int cycle = 0;
bool pressed = false;
double dY = 0;
Platform fllor;
int score = 0;
vector<Coin*> coins;


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
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		return false;
	}


	blocktex = TextureManager::LoadTexture("assets/blocks.png", renderer);
	SDL_Rect tmp = { 0,SCREEN_HEIGHT-200,SCREEN_WIDTH,40 };
	fllor.setPos(tmp);
	SDL_Rect tm = { 171 + 360,0,160,160 };
	fllor.setCrop(tm);
	fllor.setTex(blocktex);
	platforms[0] = new Platform;
	platforms[1] = new Platform;
	platforms[2] = new Platform;
	platforms[4] = new Platform;
	platforms[5] = new Platform;
	platforms[6] = new Platform;

	platforms[0]->setPos({ 0,300,70,30 });
	platforms[1]->setPos({ 130,250,100,30 });
	platforms[2]->setPos({ 450,350,120,30 });
	platforms[4]->setPos({ 1800,300,70,30 });
	platforms[5]->setPos({ 3000,250,100,30 });
	platforms[6]->setPos({ 920,350,120,30 });

	platforms[0]->setTex(blocktex);
	platforms[1]->setTex(blocktex);
	platforms[2]->setTex(blocktex);
	platforms[0]->setCrop(tm);
	platforms[1]->setCrop(tm);
	platforms[2]->setCrop(tm);
	platforms[4]->setCrop(tm);
	platforms[5]->setCrop(tm);
	platforms[6]->setCrop(tm);
	platforms[4]->setTex(blocktex);
	platforms[5]->setTex(blocktex);
	platforms[6]->setTex(blocktex);

	platforms[3] = new QuestionBox(coins);
	platforms[3]->setTex(blocktex);
	platforms[3]->setCrop({ 171 + 760,0,160,160 });
	platforms[3]->setPos({ 1000,SCREEN_HEIGHT-390,80,80 });

	playertex = TextureManager::LoadTexture("assets/iu_.png", renderer);

	music = Mix_LoadMUS("assets/Song.wav");

	mariotex = TextureManager::LoadTexture("assets/Mario.png", renderer);

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
		cameraY -= PADDLE_SPEED;
	}
	if (state[SDL_SCANCODE_S] && pY + PADDLE_HEIGHT < SCREEN_HEIGHT) {
		//dY += PADDLE_SPEED;
	}
	if (state[SDL_SCANCODE_A] && pX > 0) {
		pX -= PADDLE_SPEED;
		cameraX -= PADDLE_SPEED;
		rotation = -1;
		pressed = true;
		if (dY >= 0) {
			if (cycle < 21) {
				if (cycle % 7 == 0) {
					MarioCrop.x += tileW;
				}
				cycle++;
			}
			else {
				cycle = 0;
				MarioCrop.x = tileW;
			}
		}
	}
	
	if (state[SDL_SCANCODE_D] && pX +PADDLE_WIDTH < SCREEN_WIDTH) {
		pX += PADDLE_SPEED;
		cameraX += PADDLE_SPEED;
		rotation = 1;
		pressed = true;
		if (dY >= 0) {
			if (cycle < 21) {
				if (cycle % 7 == 0) {
					MarioCrop.x += tileW;
				}
				cycle++;
			}
			else {
				cycle = 0;
				MarioCrop.x = tileW;
			}
		}
	}
}

void Game::update()
{
	if (Grounded) {
		MarioCrop.y = tileH;
	
	}
	
	if (Mix_PlayingMusic() == 0)
	{
		Mix_PlayMusic(music, -1);
	}
	if (pressed) {
		pressed = false;
	}
	else {
		cycle = 0;
		MarioCrop.x = tileW;
	}

	//cameraY += dY ;

	Grounded = false;
	pY += dY;
	dY += 0.25;
	if (dY < 0) {
		MarioCrop = { tileW,tileH * 2,tileW,tileH -40 };
	}
	SDL_Rect paddle1 = { pX, pY, PADDLE_WIDTH, PADDLE_HEIGHT };
		
	for (int i = 0; i < sizeof(platforms)/sizeof(platforms[0]); i++) {
		if (Collision::CollideOnTop(paddle1, platforms[i]->getPos())) {

			if (platforms[i]->getPos().y < paddle1.y) {
				platforms[i]->OnCollision();
				if (platforms[i]->getCrop().y > 800) {
					delete platforms[i];
					platforms[i] = new Platform;
				}
				if (dY < 0) { dY = 0; }
				if (pY < platforms[i]->getPos().y+ platforms[i]->getPos().h) {

					pY = platforms[i]->getPos().y + platforms[i]->getPos().h;
				}	
			}
			else {
				Grounded = true;
				if (dY > 0) { dY = 0; }
				if (pY > platforms[i]->getPos().y - PADDLE_HEIGHT) {

					pY = platforms[i]->getPos().y - PADDLE_HEIGHT;
				}
			}

		}
		else if (Collision::Collide(paddle1, platforms[i]->getPos())) {
			if (pX - platforms[i]->getPos().x >= 0)
			{
				pX = platforms[i]->getPos().x + platforms[i]->getPos().w;
			}
			else {
				pX = platforms[i]->getPos().x - PADDLE_WIDTH;
			
			}
		}
		
		
	}
	if (Collision::Collide(paddle1, fllor.getPos())) {
		Grounded = true;
		if (dY > 0) { dY = 0; }
		if (pY > fllor.getPos().y - PADDLE_HEIGHT) {

			pY = fllor.getPos().y - PADDLE_HEIGHT;
		}
	}
	for (int i = 0; i < coins.size(); i++) {
		if (!coins[i]->Move()) {
			score += 100;
			coins.erase(coins.begin() + i);
		}
	}
}

void Game::render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, playertex, NULL, NULL);
	for (int i = 0; i < coins.size(); i++) {
		SDL_RenderCopy(renderer, blocktex, &coins[i]->crop, &coins[i]->pos);
	}
	for (int i = 0; i < sizeof(platforms) / sizeof(platforms[0]); i++) {
		SDL_Rect tmp = platforms[i]->getPos();
		SDL_Rect crop = platforms[i]->getCrop();

		
		SDL_RenderCopy(renderer, platforms[i]->getTex(), &crop, &tmp);
		
	}
	SDL_Rect tmp = fllor.getPos();
	SDL_Rect crop = fllor.getCrop();
	SDL_RenderCopy(renderer, fllor.getTex(), &crop, &tmp);

	SDL_Rect paddle1 = {pX, pY, PADDLE_WIDTH, PADDLE_HEIGHT};
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	if (rotation == -1) {
		SDL_RenderCopyEx(renderer, mariotex, &MarioCrop, &paddle1, 0, NULL, SDL_FLIP_HORIZONTAL);
	}
	else {
		SDL_RenderCopyEx(renderer, mariotex, &MarioCrop, &paddle1, 0, NULL, SDL_FLIP_NONE);
	}


	SDL_RenderPresent(renderer);
	
}

void Game::quit() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
