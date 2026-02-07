#pragma once
#include "Collision.hpp"
#include "PowerUps.h"
using namespace std;

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
	vector<PowerUp*>* item;
	int* MarioHp;
	SDL_Rect* Mariopos;
public:
	QuestionBox(vector<PowerUp*> &item, int* MarioHp, SDL_Rect* Mariopos) :Platform() {
		this->item= &item;
		this->Mariopos = Mariopos;
		this->MarioHp = MarioHp;
	}

	void OnCollision() override {
		item->push_back(new Mushroom(MarioHp,Mariopos,{ pos.x,pos.y - 90,163/4,163/4 }));
			std::cout << "Question block";
	}

};




