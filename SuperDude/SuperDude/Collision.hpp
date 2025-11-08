#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include <SDL_ttf.h>
class Collision {
public:
	static bool Collide(SDL_Rect rect1, SDL_Rect rect2) {
		if (rect1.x < rect2.x + rect2.w && rect2.x < rect1.x + rect1.w) {
			if (rect1.y < rect2.y + rect2.h && rect2.y < rect1.y + rect1.h) {
				return true;
			}

		}
		return false;
	}

};