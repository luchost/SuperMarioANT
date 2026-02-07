#pragma once
#include "Game.h"
class PowerUp {
public:
	int* MarioHp;
	SDL_Rect* Mariopos;
	SDL_Rect pos;
	SDL_Rect crop;
	SDL_Texture* tex;
	virtual void Effect() {
		cout << "WTF";
	};
};

class Mushroom :public PowerUp {
public:
	Mushroom() {
		pos = { 0,0,0,0 };
		crop = { 0,0,0,0 };
	}
	Mushroom(int* MarioHp, SDL_Rect* Mariopos, SDL_Rect pos) {
		this->MarioHp = MarioHp;
		this->Mariopos = Mariopos;
		this->pos = pos;
	}
	void Effect() {
		cout << *MarioHp;
		if (*MarioHp < 2) {			
			*MarioHp = 2;
			Mariopos->h *= 2;
		}
	
	}

};
class Coin: public PowerUp{
public:
	SDL_Rect crop = { 1673,531,163,163 };
	SDL_Rect pos;
	//int* score;
	int destroyme = pos.y + 90;
	Coin(SDL_Rect pos) {
		//this->score = &score;
		this->pos = pos;
		destroyme = pos.y + 90;
	}
	bool Move() {
		if (pos.y < destroyme) {
			pos.y += 5;
			return true;
		}
		else {
			return false;
		}

	}

};