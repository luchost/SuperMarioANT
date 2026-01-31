#pragma once
#include "Game.h"
#include"Platform.hpp"
#include"Collision.hpp"
using namespace std;
class Enemy {
public:
	SDL_Rect pos;
	SDL_Rect head;
	SDL_Rect crop;
	SDL_Texture* tex;
	int moveSpeed;
	int rotation=1;
	int anim = 0;


	bool checkGround(vector<Platform*> platforms) {
		SDL_Rect infront = { pos.x + pos.w * rotation,pos.y,pos.w,pos.h + 10 };
		for (int i = 0; i < platforms.size(); i++) {
			if (Collision::Collide(infront, platforms[i]->getPos())) {
				return true;
			}
		}
		return false;
	
	}
	virtual void Move() {
		pos.x += moveSpeed * rotation;
		head.x += moveSpeed * rotation;
		if (anim > 4) {
			if (crop.x < 194) {
				crop.x += crop.w;
			}
			else {
				crop.x -= crop.w;
			}
			anim = 0;
		}
		anim++;
	}
	virtual void onHurt() {
		cout << "Ouch!!!"<<endl;
		crop.x = 1 + 2 * crop.w;
		for (int i = 0; i < 10; i++) {
			pos.y += 10;
			pos.x - 3;
		}
	}
	Enemy() {
		 moveSpeed = 0;
		 tex = nullptr;
		 pos = { 0,0,0,0 };
		 head = { 0,0,0,0 };
		 crop = { 0,0,0,0 };
	
	}
	Enemy(SDL_Rect pos, SDL_Rect crop, SDL_Texture* tex, int moveSpeed) {
		this->pos = pos;
		this->crop = crop;
		this->tex = tex;
		this->moveSpeed = moveSpeed;
		this->head = { pos.x,pos.y,pos.w,10 };
	}
};

class Ghost :public Enemy {
public:
	SDL_Rect* playerPos = nullptr;
	Ghost():Enemy() {
	
	}
	Ghost(SDL_Rect pos, SDL_Rect crop, SDL_Texture* tex, int moveSpeed,SDL_Rect* PlayerPos) :Enemy(pos, crop, tex, moveSpeed) {
		this->playerPos = PlayerPos;
	}
	bool LookingAtMe() {
	
	
	}
	void Move() {
		if (playerPos->x > pos.x) {
			pos.x += moveSpeed;
		}
		else if (playerPos->x < pos.x) {
			pos.x -= moveSpeed;
		}

		if (playerPos->y > pos.y) {
			pos.y += moveSpeed;
		}
		else if (playerPos->y < pos.y) {
			pos.y -= moveSpeed;
		}
	}
	void OnHurt() {
		cout << "Boo";
	}

};