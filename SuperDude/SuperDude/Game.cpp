#include "Game.h"
#include "TextureManager.h"
#include "Platform.hpp"
#include"Collision.hpp"
#include "Button.hpp"
#include"Enemy.h"
#include<vector>
#include<fstream>

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
SDL_Texture* tex;


double pY = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
double pX = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;

double cameraX = 0;
double cameraY = 0;

bool Grounded = false;
vector<Platform*> platforms;
SDL_Rect MarioCrop = { tileW,tileH,tileW,tileH - 40 };
SDL_Rect MarioPos = { pX,pY,PADDLE_WIDTH,PADDLE_HEIGHT };
int rotation = 1;
int MarioHp = 1;
int cycle = 0;
bool pressed = false;
double dY = 0;
Platform fllor;
int score = 0;
int startTicks = SDL_GetTicks();
bool cooldown = false;
vector<PowerUp*> Items;
TTF_Font* font;

vector<Enemy*> enemies;

SDL_Scancode Jump = SDL_SCANCODE_W;
SDL_Scancode Left = SDL_SCANCODE_A;
SDL_Scancode Right = SDL_SCANCODE_D;
SDL_Scancode Dash = SDL_SCANCODE_LSHIFT;
int Volume = 0;
int InvincabilityFrames=0;


Game::Game() {

}
Game::~Game() {

}
void Game::loadSettings() {
	ifstream ReadFile("Settings.txt");
	string tmp;
	ReadFile >> tmp;
	Jump = SDL_GetScancodeFromName(tmp.c_str());
	ReadFile >> tmp;
	Left = SDL_GetScancodeFromName(tmp.c_str());
	ReadFile >> tmp;
	Right = SDL_GetScancodeFromName(tmp.c_str());
	ReadFile >> tmp;
	Dash = SDL_GetScancodeFromName(tmp.c_str());
	ReadFile >> Volume;
	Mix_Volume(-1,Volume);
	Mix_VolumeMusic(Volume);
}
void Game :: writeSettings() {
	ofstream File("Settings.txt");
	File << SDL_GetScancodeName(Jump)<<endl<<SDL_GetScancodeName(Left) << endl << SDL_GetScancodeName(Right) << endl;
	File << SDL_GetScancodeName(Dash) << endl;
	File << Volume;
	File.close();
}
void Game::SettingsMenu() {
	SDL_Event event;
	bool start = false;
	Button VolumeButton({ 200,100,100,10 }, font, "Volume", renderer);
	SDL_Rect volumeBar({220,200,128*3,5});
	SDL_Rect volumePin({ Volume*3+220,180,40,40 });
	Button Exit({ 200,800,100,10 }, font, "Quit", renderer);
	Button JumpBt({ 200,300,100,10 }, font, "Jump", renderer);
	Button LeftBt({ 400,300,100,10 }, font, "Left", renderer);
	Button RightBt({ 600,300,100,10 }, font, "Right", renderer);
	Button DashBt({ 800,300,100,10 }, font, "Dash", renderer);
	Button* JumpBt2 = new Button({ 200,350,100,10 }, font, SDL_GetScancodeName(Jump), renderer);
	Button* LeftBt2 = new Button({ 400,350,100,10 }, font, SDL_GetScancodeName(Left), renderer);
	Button* RightBt2 = new Button({ 600,350,100,10 }, font, SDL_GetScancodeName(Right), renderer);
	Button* DashBt2 = new Button({ 800,350,100,10 }, font, SDL_GetScancodeName(Dash), renderer);

	int x, y;
	while (!start) {
		do {
			SDL_PollEvent(&event);
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			SDL_RenderClear(renderer);

			SDL_RenderCopy(renderer, VolumeButton.tex, NULL, &VolumeButton.pos);
			SDL_RenderCopy(renderer, Exit.tex, NULL, &Exit.pos);
			SDL_RenderCopy(renderer, JumpBt.tex, NULL, &JumpBt.pos);
			SDL_RenderCopy(renderer, LeftBt.tex, NULL, &LeftBt.pos);
			SDL_RenderCopy(renderer, RightBt.tex, NULL, &RightBt.pos);
			SDL_RenderCopy(renderer, DashBt.tex, NULL, &DashBt.pos);
			SDL_RenderCopy(renderer, JumpBt2->tex, NULL, &JumpBt2->pos);
			SDL_RenderCopy(renderer, LeftBt2->tex, NULL, &LeftBt2->pos);
			SDL_RenderCopy(renderer, RightBt2->tex, NULL, &RightBt2->pos);
			SDL_RenderCopy(renderer, DashBt2->tex, NULL, &DashBt2->pos);

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderFillRect(renderer, &volumeBar);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderFillRect(renderer, &volumePin);
			SDL_RenderPresent(renderer);
		} while (event.type != SDL_MOUSEBUTTONDOWN);
		if (event.button.button == SDL_BUTTON_LEFT) {
			SDL_GetMouseState(&x, &y);
			SDL_Rect tmp = { x,y,0,0 };

			if (Collision::Collide(tmp, VolumeButton.pos)) {
				do {
					SDL_PollEvent(&event);
				} while (event.type != SDL_MOUSEBUTTONDOWN);
				if (event.button.button == SDL_BUTTON_LEFT) {
					SDL_GetMouseState(&x, &y);
					if (x > 220 + 128 * 3) { x = 220 + 128 * 3; };
					if (x < 220 ) { x = 220; };
					volumePin.x = x;
					Volume = (x - 220) / 3;
					Mix_Volume(-1, (x - 220) / 3);
					Mix_VolumeMusic((x - 220) / 3);
				}
			}
			if (Collision::Collide(tmp, JumpBt.pos)|| Collision::Collide(tmp, JumpBt2->pos)) {
				bool pres = false;
				do {
					SDL_PollEvent(&event);
					if (event.type == SDL_KEYDOWN) {
						SDL_Scancode letter = event.key.keysym.scancode;
						if (letter == Jump || letter == Left || letter == Right || letter == Dash) {
							SDL_Window* Popup= SDL_CreateWindow("Warning", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 100, SDL_WINDOW_SHOWN);;
							SDL_Renderer* rendererPop = SDL_CreateRenderer(Popup, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
							
							Button Error({ 50,50,200,10 }, font, "This Button is already in use", rendererPop);
								SDL_SetRenderDrawColor(rendererPop, 255, 255, 255, 255);
								SDL_RenderClear(rendererPop);
								SDL_RenderCopy(rendererPop, Error.tex, NULL, &Error.pos);
								SDL_RenderPresent(rendererPop);


								SDL_Delay(1000);
							SDL_DestroyRenderer(rendererPop);
							SDL_DestroyWindow(Popup);
						}
						Jump = event.key.keysym.scancode;
						pres = true;
						string q = SDL_GetScancodeName(letter);
						JumpBt2 = new Button({ 200,350,100,10 }, font, q, renderer);
					}


				}while (!pres);
			}
			if (Collision::Collide(tmp, LeftBt.pos)|| Collision::Collide(tmp, LeftBt2->pos)) {
				bool pres = false;
				do {
					SDL_PollEvent(&event);
					if (event.type == SDL_KEYDOWN) {
						SDL_Scancode letter = event.key.keysym.scancode;
						if (letter == Jump || letter == Left || letter == Right || letter == Dash) {
							SDL_Window* Popup = SDL_CreateWindow("Warning", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 100, SDL_WINDOW_SHOWN);;
							SDL_Renderer* rendererPop = SDL_CreateRenderer(Popup, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

							Button Error({ 50,50,200,10 }, font, "This Button is already in use", rendererPop);
							SDL_SetRenderDrawColor(rendererPop, 255, 255, 255, 255);
							SDL_RenderClear(rendererPop);
							SDL_RenderCopy(rendererPop, Error.tex, NULL, &Error.pos);
							SDL_RenderPresent(rendererPop);


							SDL_Delay(1000);
							SDL_DestroyRenderer(rendererPop);
							SDL_DestroyWindow(Popup);
						}

						Left = event.key.keysym.scancode;
						pres = true;
						string q = SDL_GetScancodeName(letter);
						LeftBt2 = new Button({ 400,350,100,10 }, font, q, renderer);
					}


				} while (!pres);
			}
			if (Collision::Collide(tmp, RightBt.pos)|| Collision::Collide(tmp, RightBt2->pos)) {
				bool pres = false;
				do {
					SDL_PollEvent(&event);
					if (event.type == SDL_KEYDOWN) {
						SDL_Scancode letter = event.key.keysym.scancode;
						if (letter == Jump || letter == Left || letter == Right || letter == Dash) {
							SDL_Window* Popup = SDL_CreateWindow("Warning", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 100, SDL_WINDOW_SHOWN);;
							SDL_Renderer* rendererPop = SDL_CreateRenderer(Popup, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

							Button Error({ 50,50,200,10 }, font, "This Button is already in use", rendererPop);
							SDL_SetRenderDrawColor(rendererPop, 255, 255, 255, 255);
							SDL_RenderClear(rendererPop);
							SDL_RenderCopy(rendererPop, Error.tex, NULL, &Error.pos);
							SDL_RenderPresent(rendererPop);


							SDL_Delay(1000);
							SDL_DestroyRenderer(rendererPop);
							SDL_DestroyWindow(Popup);
						}

						Right = event.key.keysym.scancode;
						pres = true;
						string q = SDL_GetScancodeName(letter);
						RightBt2 = new Button({ 600,350,100,10 }, font, q, renderer);
					}


				} while (!pres);
			}
			if (Collision::Collide(tmp, DashBt.pos)|| Collision::Collide(tmp, DashBt2->pos)) {
				bool pres = false;
				do {
					SDL_PollEvent(&event);
					if (event.type == SDL_KEYDOWN) {
						SDL_Scancode letter = event.key.keysym.scancode;
						if (letter == Jump || letter == Left || letter == Right || letter == Dash) {
							SDL_Window* Popup = SDL_CreateWindow("Warning", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 100, SDL_WINDOW_SHOWN);;
							SDL_Renderer* rendererPop = SDL_CreateRenderer(Popup, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

							Button Error({ 50,50,200,10 }, font, "This Button is already in use", rendererPop);
							SDL_SetRenderDrawColor(rendererPop, 255, 255, 255, 255);
							SDL_RenderClear(rendererPop);
							SDL_RenderCopy(rendererPop, Error.tex, NULL, &Error.pos);
							SDL_RenderPresent(rendererPop);


							SDL_Delay(1000);
							SDL_DestroyRenderer(rendererPop);
							SDL_DestroyWindow(Popup);
						}

						Dash = letter;
						pres = true;
						string q = SDL_GetScancodeName(letter);
						DashBt2 = new Button({ 800,350,100,10 }, font, q, renderer);
					}


				} while (!pres);
			}


			if (Collision::Collide(tmp, Exit.pos)) {
				writeSettings();
				return;
			}
		}
	}

}
void Game::LevelEditor() {
	SDL_Event event;
	bool end = false;
	int id = 0;
	int xoffset = 0;
	platforms.clear();
	enemies.clear();

	while (!end) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_RenderCopy(renderer, playertex, NULL, NULL);
		for (int i = 0; i < Items.size(); i++) {
			SDL_RenderCopy(renderer, tex, NULL, &Items[i]->pos);
		}
		for (int i = 0; i < platforms.size(); i++) {
			SDL_Rect tmp = platforms[i]->getPos();
			SDL_Rect crop = platforms[i]->getCrop();
			tmp.x -= xoffset;

			SDL_RenderCopy(renderer, platforms[i]->getTex(), &crop, &tmp);

		}
		for (int i = 0; i < enemies.size(); i++) {
			SDL_Rect tmp = enemies[i]->pos;
			tmp.x -= xoffset;
			SDL_RenderCopy(renderer, enemies[i]->tex, &enemies[i]->crop, &tmp);
		}
		SDL_Rect textpos = { 0,0,100,30 };
		SDL_Rect tmp = fllor.getPos();
		SDL_Rect crop = fllor.getCrop();
		SDL_RenderCopy(renderer, fllor.getTex(), &crop, &tmp);

		SDL_RenderCopy(renderer, test, NULL, &textpos);

		//SDL_Rect paddle1 = {MarioPos.x,MarioPos.y , PADDLE_WIDTH, PADDLE_HEIGHT};
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		if (rotation == -1) {
			SDL_RenderCopyEx(renderer, mariotex, &MarioCrop, &MarioPos, 0, NULL, SDL_FLIP_HORIZONTAL);
		}
		else {
			SDL_RenderCopyEx(renderer, mariotex, &MarioCrop, &MarioPos, 0, NULL, SDL_FLIP_NONE);
		}


		SDL_RenderPresent(renderer);

		const Uint8* state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_RETURN]) {
			end = true;
		}
		if (state[SDL_SCANCODE_A]) {
			xoffset -= 10;
		}
		if (state[SDL_SCANCODE_D]) {
			xoffset += 10;
		}
		if (state[SDL_SCANCODE_1]) {
				id = 1;
		}
		if (state[SDL_SCANCODE_2]) {
			id = 2;
		}
		if (state[SDL_SCANCODE_0]) {
			id = 0;
		}
		if (state[SDL_SCANCODE_3]) {
			id = 3;
		}
		
			SDL_PollEvent(&event);
		
		if(event.type == SDL_MOUSEBUTTONDOWN){
			if (event.button.button == SDL_BUTTON_LEFT) {
				int x, y;
				SDL_GetMouseState(&x, &y);
				switch (id) {

				case 0: {
					Platform* platform = new Platform();
					platform->setPos({ x-25 + xoffset,y-25,50,50 });
					platform->setTex(blocktex);
					platform->setCrop({ 171 + 360,0,160,160 });
					platforms.push_back(platform);
				}
					  break;
				case 1:
				{
					QuestionBox* platform = new QuestionBox(Items, &MarioHp, &MarioPos);
					platform->setPos({ x - 25 + xoffset,y - 25,50,50 });
					platform->setTex(blocktex);
					platform->setCrop({ 171 + 760,0,160,160 });
					platforms.push_back(platform);
				}
				break;
				case 2:
				{
					Enemy* enemy = new Enemy({ x + xoffset - 25,y - 25,50,50 }, { 1,0,193,161 }, TextureManager::LoadTexture("assets/Goomba.png", renderer), 2);
					enemies.push_back(enemy);
				}
				break;
				case 3:
				{
					Ghost* enemy = new Ghost({ x + xoffset - 25,y - 25,50,50 }, { 0,0,820,636, }, TextureManager::LoadTexture("assets/Boo.png", renderer), 2, &MarioPos, &rotation);
					enemies.push_back(enemy);
				}
				break;
				}

			}
		}


	}
}
void Game::StartMenu() {
	SDL_Event event;
	bool start = false;
	int x, y;
	Button startButton({ SCREEN_WIDTH/2-250 ,SCREEN_HEIGHT/2-100 ,100,10 }, font, "Start", renderer);
	Button settingsButton({ SCREEN_WIDTH / 2 - 250 ,SCREEN_HEIGHT / 2 ,100,10 }, font, "Settings", renderer);
	Button quitButton({ SCREEN_WIDTH / 2 - 250 ,SCREEN_HEIGHT / 2 + 200 ,100,10 }, font, "Quit", renderer);
	Button LevelButton({ SCREEN_WIDTH / 2 - 250 ,SCREEN_HEIGHT / 2+ 100 ,100,10 }, font, "Editor", renderer);
	while (!start) {
		do {
			SDL_PollEvent(&event);
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			SDL_RenderClear(renderer);

			SDL_RenderCopy(renderer, startButton.tex, NULL, &startButton.pos);
			SDL_RenderCopy(renderer, settingsButton.tex, NULL, &settingsButton.pos);
			SDL_RenderCopy(renderer, quitButton.tex, NULL, &quitButton.pos);
			SDL_RenderCopy(renderer, LevelButton.tex, NULL, &LevelButton.pos);
			SDL_RenderPresent(renderer);
		} while (event.type != SDL_MOUSEBUTTONDOWN);
		
		if (event.button.button == SDL_BUTTON_LEFT) {
			SDL_GetMouseState(&x, &y);
			SDL_Rect tmp = { x,y,0,0 };
			if (Collision::Collide(tmp, startButton.pos)) {
				startButton.OnClick();
				start = true;
			}
			else if (Collision::Collide(tmp, settingsButton.pos)) {
				settingsButton.OnClick();
				SettingsMenu();
			}
			else if(Collision::Collide(tmp, quitButton.pos)){
				isRunning = false;
				return;
			}
			else if (Collision::Collide(tmp, LevelButton.pos)) {
				LevelEditor();
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

	platforms.push_back(new QuestionBox(Items,&MarioHp,&MarioPos));
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
	tex = TextureManager::LoadTexture("assets/guba.png", renderer);
	Enemy* em1=new Enemy({ 130,230,40,20 }, { 1,0,193,161 }, TextureManager::LoadTexture("assets/Goomba.png", renderer), 2);
	Enemy* em2 = new Ghost({ 130,230,80,60 }, {0,0,820,636}, TextureManager::LoadTexture("assets/Boo.png", renderer), 2, &MarioPos,&rotation);
	;
	enemies.push_back(em1);
	enemies.push_back(em2);

	//Mix_VolumeMusic(20);

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

	if (state[SDL_SCANCODE_ESCAPE]) {
		SettingsMenu();
	}
	if (state[Jump] && pY > 0 && Grounded) {
		dY -= PADDLE_SPEED*2;
		cameraY -= PADDLE_SPEED;
		Mix_PlayChannel(-1, pasta, 0);
	}
	if (state[SDL_SCANCODE_S] && pY + PADDLE_HEIGHT < SCREEN_HEIGHT) {
		//dY += PADDLE_SPEED;
	}
	if (state[Left] && pX > 0) {
		for (int i = 0; i < platforms.size(); i++) {
			platforms[i]->setPos({ platforms[i]->getPos().x + int(PADDLE_SPEED),platforms[i]->getPos().y,platforms[i]->getPos().w ,platforms[i]->getPos().h });
		}
		for (int i = 0; i < enemies.size(); i++) {
			enemies[i]->pos.x += int(PADDLE_SPEED);
		}
		for (int i = 0; i < Items.size(); i++) {
			Items[i]->pos.x += int(PADDLE_SPEED);
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
	
	if (state[Right] && pX +PADDLE_WIDTH < SCREEN_WIDTH) {
		for (int i = 0; i < platforms.size(); i++) {
			platforms[i]->setPos({ platforms[i]->getPos().x - int(PADDLE_SPEED),platforms[i]->getPos().y,platforms[i]->getPos().w ,platforms[i]->getPos().h });
		}
		for (int i = 0; i < enemies.size(); i++) {
			enemies[i]->pos.x -= int(PADDLE_SPEED);
		}
		for (int i = 0; i < Items.size(); i++) {
			Items[i]->pos.x -= int(PADDLE_SPEED);
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

		if (state[Left] && state[Dash] && pX > 0) {
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

		if (state[Right] && state[Dash] && pX + PADDLE_WIDTH < SCREEN_WIDTH) {
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
	MarioPos.y += dY;
	dY += 0.25;
	if (dY < 0) {
		MarioCrop = { tileW,tileH * 2,tileW,tileH -40 };
	}	
	for (int i = 0; i < platforms.size(); i++) {
		if (Collision::CollideOnTop(MarioPos, platforms[i]->getPos())) {

			if (platforms[i]->getPos().y < MarioPos.y) {
				platforms[i]->OnCollision();
				if (MarioHp>=2) {
					platforms.erase(platforms.begin() + i);
				}
				if (dY < 0) { dY = 0; }
				if (MarioPos.y < platforms[i]->getPos().y+ platforms[i]->getPos().h) {

					MarioPos.y = platforms[i]->getPos().y + platforms[i]->getPos().h+2;
				}	
			}
			else {
				Grounded = true;
				if (dY > 0) { dY = 0; }
				if (MarioPos.y > platforms[i]->getPos().y - PADDLE_HEIGHT) {

					MarioPos.y = platforms[i]->getPos().y - PADDLE_HEIGHT;
				}
			}

		}
		else if (Collision::Collide(MarioPos, platforms[i]->getPos())) {
			if (MarioPos.x - platforms[i]->getPos().x >= 0)
			{
				MarioPos.x = platforms[i]->getPos().x + platforms[i]->getPos().w;
			}
			else {
				MarioPos.x = platforms[i]->getPos().x - PADDLE_WIDTH;
			
			}
		}
		
		
	}
	if (Collision::Collide(MarioPos, fllor.getPos())) {
		Grounded = true;
		if (dY > 0) { dY = 0; }
		if (MarioPos.y > fllor.getPos().y - PADDLE_HEIGHT) {

			MarioPos.y = fllor.getPos().y - PADDLE_HEIGHT;
		}
	}
	for (int i = 0; i < enemies.size(); i++) {
		if (!enemies[i]->checkGround(platforms)) {
			enemies[i]->rotation *= -1;
		}
		if (enemies[i]->FloatToDeath()) {
			enemies[i]->onHurt();
			enemies.erase(enemies.begin() + i);
			continue;
		}
		enemies[i]->Move();
		if (Collision::Collide(MarioPos, enemies[i]->pos)) {
			if (MarioPos.y+MarioPos.h < enemies[i]->pos.y+10) {
				enemies[i]->onHurt();
				enemies.erase(enemies.begin() + i);
			}
			else if(InvincabilityFrames==0){
				if (MarioHp == 2) { MarioPos.h /= 2; }
				MarioHp--;
				InvincabilityFrames = 30;
			}
			
		}
		
	
	}
	for (int i = 0; i < Items.size(); i++) {

		if (Collision::Collide(MarioPos, Items[i]->pos)) {
			Items[i]->Effect();
			Items.erase(Items.begin() + i);
		}
	}
	if (MarioHp <= 0) {
		quit();
	}
	if (InvincabilityFrames > 0) {
		InvincabilityFrames--;
	}
}

void Game::render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, playertex, NULL, NULL);
	for (int i = 0; i < Items.size(); i++) {
		SDL_RenderCopy(renderer, tex, NULL, &Items[i]->pos);
	}
	for (int i = 0; i < platforms.size(); i++) {
		SDL_Rect tmp = platforms[i]->getPos();
		SDL_Rect crop = platforms[i]->getCrop();

		
		SDL_RenderCopy(renderer, platforms[i]->getTex(), &crop, &tmp);
		
	}
	for (int i = 0; i < enemies.size(); i++) {
	
		SDL_RenderCopy(renderer, enemies[i]->tex, &enemies[i]->crop, &enemies[i]->pos);
	}
	SDL_Rect textpos = { 0,0,100,30 };
	SDL_Rect tmp = fllor.getPos();
	SDL_Rect crop = fllor.getCrop();
	SDL_RenderCopy(renderer, fllor.getTex(), &crop, &tmp);

	SDL_RenderCopy(renderer, test,NULL, &textpos);

	//SDL_Rect paddle1 = {MarioPos.x,MarioPos.y , PADDLE_WIDTH, PADDLE_HEIGHT};
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	if (rotation == -1) {
		SDL_RenderCopyEx(renderer, mariotex, &MarioCrop, &MarioPos, 0, NULL, SDL_FLIP_HORIZONTAL);
	}
	else {
		SDL_RenderCopyEx(renderer, mariotex, &MarioCrop, &MarioPos, 0, NULL, SDL_FLIP_NONE);
	}


	SDL_RenderPresent(renderer);
	
}

void Game::quit() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
