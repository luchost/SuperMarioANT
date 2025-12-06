#pragma once
#include "Collision.hpp"
#include "Game.h"
using namespace std;

class Coin {
public:
	SDL_Rect crop = {1673,531,163,163};
	SDL_Rect pos;
	//int* score;
	int destroyme = pos.y + 90;
	Coin( SDL_Rect pos) {
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
	vector<Coin*>* coin;
public:
	QuestionBox(vector<Coin*> &coin) :Platform() {
		this->coin = &coin;
	
	}
	void OnCollision() override {
		coin->push_back(new Coin({ pos.x,pos.y - 90,163/4,163/4 }));
			std::cout << "Question block";
	}

};