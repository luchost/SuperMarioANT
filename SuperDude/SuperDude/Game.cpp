#include "Game.h"
#include "TextureManager.h"
#include "Platform.hpp"
#include"Collision.hpp"
#include "Button.hpp"
#include<vector>

const int SPRITE_COLS = 5;
const int SPRITE_ROWS = 5;

const int tileW = 1000 / SPRITE_COLS;
const int tileH = 1080 / SPRITE_ROWS;

Mix_Music* music;
Mix_Chunk* coin;
Mix_Chunk* jump;
Mix_Chunk* pasta;
Mix_Chunk* dash;

SDL_Texture* test;
SDL_Texture* playertex;
SDL_Texture* blocktex;
SDL_Texture* mariotex;


double pY = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
double pX = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;

double cameraX = 0;
double cameraY = 0;

bool Grounded = false;
vector<Platform*> platforms;
SDL_Rect MarioCrop = { tileW,tileH,tileW,tileH - 40 };
int rotation = 1;
int cycle = 0;
bool pressed = false;
double dY = 0;
Platform fllor;
int score = 0;
int startTicks = SDL_GetTicks();
bool cooldown = false;
vector<Coin*> coins;
TTF_Font* font;

Game::Game() {

}
Game::~Game() {

}
void Game::StartMenu() {
	SDL_Event event;
	bool start = false;
	int x, y;
	Button startButton({ SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2,100,10 }, font, "Start", renderer);
	while (!start) {
		do {
			SDL_PollEvent(&event);
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			SDL_RenderClear(renderer);

			SDL_RenderCopy(renderer, startButton.tex, NULL, &startButton.pos);
			SDL_RenderPresent(renderer);
		} while (event.type != SDL_MOUSEBUTTONDOWN);
		
		if (event.button.button == SDL_BUTTON_LEFT) {
			SDL_GetMouseState(&x, &y);
			SDL_Rect tmp = { x,y,0,0 };
			if (Collision::Collide(tmp, startButton.pos)) {
				startButton.OnClick();
				start = true;
			}


		}


	}
}

bool Game::init()
{


	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	window = SDL_CreateWindow("SuperDude", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (window == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return false;
	}

	Mix_Init(MIX_INIT_MP3);
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		return false;
	}

	TTF_Init();

	font = TTF_OpenFont("assets/Font.ttf", 30);

	blocktex = TextureManager::LoadTexture("assets/blocks.png", renderer);
	SDL_Rect tmp = { 0,SCREEN_HEIGHT-220,SCREEN_WIDTH,40 };
	fllor.setPos(tmp);
	SDL_Rect tm = { 171 + 360,0,160,160 };
	fllor.setCrop(tm);
	fllor.setTex(blocktex);
	platforms.push_back( new Platform);
	platforms.push_back(new Platform);
	platforms.push_back(new Platform);
	platforms.push_back(new Platform);
	platforms.push_back(new Platform);
	platforms.push_back(new Platform);

	platforms[0]->setPos({ 0,300,70,30 });
	platforms[1]->setPos({ 130,250,100,30 });
	platforms[2]->setPos({ 450,350,120,30 });
	platforms[4]->setPos({ 1800,300,70,30 });
	platforms[5]->setPos({ 3000,250,100,30 });
	platforms[3]->setPos({ 920,350,120,30 });

	platforms[0]->setTex(blocktex);
	platforms[1]->setTex(blocktex);
	platforms[2]->setTex(blocktex);
	platforms[0]->setCrop(tm);
	platforms[1]->setCrop(tm);
	platforms[2]->setCrop(tm);
	platforms[4]->setCrop(tm);
	platforms[5]->setCrop(tm);
	platforms[3]->setCrop(tm);
	platforms[4]->setTex(blocktex);
	platforms[5]->setTex(blocktex);
	platforms[3]->setTex(blocktex);

	platforms.push_back(new QuestionBox(coins));
	platforms[6]->setTex(blocktex);
	platforms[6]->setCrop({ 171 + 760,0,160,160 });
	platforms[6]->setPos({ 1000,SCREEN_HEIGHT-390,80,80 });

	platforms.push_back(new Platform);
	platforms[7]->setTex(blocktex);
	platforms[7]->setCrop({ 171 + 760,0,160,160 });
	platforms[7]->setPos({ 100,SCREEN_HEIGHT - 300,80,80 });

	
	playertex = TextureManager::LoadTexture("assets/iu_.png", renderer);

	music = Mix_LoadMUS("assets/Jinglle.mp3");

	coin = Mix_LoadWAV("assets/Coin(1).wav");
	jump = Mix_LoadWAV("assets/Jump.wav");
	dash = Mix_LoadWAV("assets/Dash.wav");
	pasta = Mix_LoadWAV("assets/Pasta.wav");

	test = TextureManager::LoadTextureText(font, "HEllo its a me mario", renderer);
	mariotex = TextureManager::LoadTexture("assets/Mario.png", renderer);
	Mix_VolumeMusic(20);

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
		dY -= PADDLE_SPEED*2;
		cameraY -= PADDLE_SPEED;
		Mix_PlayChannel(-1, pasta, 0);
	}
	if (state[SDL_SCANCODE_S] && pY + PADDLE_HEIGHT < SCREEN_HEIGHT) {
		//dY += PADDLE_SPEED;
	}
	if (state[SDL_SCANCODE_A] && pX > 0) {
		for (int i = 0; i < platforms.size(); i++) {
			platforms[i]->setPos({ platforms[i]->getPos().x + int(PADDLE_SPEED),platforms[i]->getPos().y,platforms[i]->getPos().w ,platforms[i]->getPos().h });
		}
		rotation = -1;
		pressed = true;
		if (dY == 0) {
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
		for (int i = 0; i < platforms.size(); i++) {
			platforms[i]->setPos({ platforms[i]->getPos().x - int(PADDLE_SPEED),platforms[i]->getPos().y,platforms[i]->getPos().w ,platforms[i]->getPos().h });
		}
		rotation = 1;
		pressed = true;
		if (dY == 0) {
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
	if (!cooldown) {

		if (state[SDL_SCANCODE_A] && state[SDL_SCANCODE_LSHIFT] && pX > 0) {
			cooldown = true;
			startTicks = SDL_GetTicks();
			for (int i = 0; i < platforms.size(); i++) {
				platforms[i]->setPos({ platforms[i]->getPos().x + int(PADDLE_SPEED * 10),platforms[i]->getPos().y,platforms[i]->getPos().w ,platforms[i]->getPos().h });
			}
			rotation = -1;
			pressed = true;
			if (dY == 0) {
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
			Mix_PlayChannel(-1, dash, 0);
		}

		if (state[SDL_SCANCODE_D] && state[SDL_SCANCODE_LSHIFT] && pX + PADDLE_WIDTH < SCREEN_WIDTH) {
			cooldown = true;
			startTicks = SDL_GetTicks();

			for (int i = 0; i < platforms.size(); i++) {
				platforms[i]->setPos({ platforms[i]->getPos().x - int(PADDLE_SPEED * 10),platforms[i]->getPos().y,platforms[i]->getPos().w ,platforms[i]->getPos().h });
			}
			rotation = 1;
			pressed = true;
			if (dY == 0) {
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
			Mix_PlayChannel(-1, dash, 0);
		}
	}
}

void Game::update()
{

	int CurrTicks = SDL_GetTicks();

	if (CurrTicks - startTicks >= 2000) {
		cooldown = false;
	
	}
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
		
	for (int i = 0; i < platforms.size(); i++) {
		if (Collision::CollideOnTop(paddle1, platforms[i]->getPos())) {

			if (platforms[i]->getPos().y < paddle1.y) {
				platforms[i]->OnCollision();
				if (platforms[i]->getCrop().y > 800) {
					delete platforms[i];
					platforms[i] = new Platform;
				}
				if (dY < 0) { dY = 0; }
				if (pY < platforms[i]->getPos().y+ platforms[i]->getPos().h) {

					pY = platforms[i]->getPos().y + platforms[i]->getPos().h+2;
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
		Mix_PlayChannel(-1, coin, 0);
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
	for (int i = 0; i < platforms.size(); i++) {
		SDL_Rect tmp = platforms[i]->getPos();
		SDL_Rect crop = platforms[i]->getCrop();

		
		SDL_RenderCopy(renderer, platforms[i]->getTex(), &crop, &tmp);
		
	}
	SDL_Rect textpos = { 0,0,100,30 };
	SDL_Rect tmp = fllor.getPos();
	SDL_Rect crop = fllor.getCrop();
	SDL_RenderCopy(renderer, fllor.getTex(), &crop, &tmp);

	SDL_RenderCopy(renderer, test,NULL, &textpos);

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
