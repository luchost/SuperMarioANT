#pragma once

#include "Game.h"
using namespace std;
class TextureManager {
public:
	static SDL_Texture* LoadTexture(const char* fileName, SDL_Renderer* ren);
	static SDL_Texture* LoadTextureText(TTF_Font* font, string text, SDL_Renderer* ren);
};