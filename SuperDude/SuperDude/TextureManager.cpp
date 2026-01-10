#include "TextureManager.h"
using namespace std;
SDL_Texture* TextureManager::LoadTexture(const char* fileName, SDL_Renderer* ren) {
	SDL_Surface* tempSurface = IMG_Load(fileName);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, tempSurface);
	SDL_FreeSurface(tempSurface);

	return tex;
}
SDL_Texture* TextureManager::LoadTextureText(TTF_Font* font, string text, SDL_Renderer* ren) {
	SDL_Color color = { 0,0,0 };
	SDL_Surface* surface = TTF_RenderText_Solid(font,text.c_str(), color);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);

	return tex;
}