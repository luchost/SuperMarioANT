#pragma once
#include "Collision.hpp"

class Platform {
protected:
	SDL_Rect pos;
	SDL_Texture* tex;
	SDL_Rect crop;

public:
	Platform() {
		pos = { 0,0,0,0 };
		tex = nullptr;
		crop = { 0,0,0,0 };
	}
	virtual void OnCollision() {
	
	}
	SDL_Rect getPos() {
		return pos;
	}
	SDL_Texture* getTex() {
		return tex;
	}
	SDL_Rect getCrop() {
		return crop;
	}

	void setTex(SDL_Texture* tex) {
		this->tex = tex;
	}
	void setPos(SDL_Rect rect) {
		pos = rect;
	}
	void setCrop(SDL_Rect rect) {
		crop = rect;
	}


};

class QuestionBox :public Platform {
	void OnCollision() override {
		SDL_Rect crop2 = getCrop();
		crop2.y += 180;
		setCrop(crop2);

			std::cout << "Question block";
	}

};