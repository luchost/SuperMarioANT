#pragma once
#include "Game.h"
#include "TextureManager.h"

class Button {
public:
	SDL_Texture* tex;
	SDL_Rect pos;
	virtual void OnClick() {
	
		std::cout << "Clicked";
	
	}
	Button() {
		pos = { 0,0,0,0 };
		tex = nullptr;
	}
	Button(SDL_Rect pos,TTF_Font* font, std::string text, SDL_Renderer* ren) {
		this->pos = pos;
		this->tex = TextureManager::LoadTextureText(font, text, ren);
		int w, h;
		SDL_QueryTexture(tex, NULL, NULL, &w, &h);
		this->pos.w = w;
		this->pos.h = h;

	}

};