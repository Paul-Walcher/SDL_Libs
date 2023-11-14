
#ifndef __DRAWCIRCLE__
#define __DRAWCIRCLE__

#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#undef main
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#endif

#include <cmath>

void SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int r, bool filled = false, int thickness=1){

	for(int i = -r; i <= r; i++){
		for(int j = -r; j <= r; j++){

			if (static_cast<int>(sqrt(i*i + j*j)) <= r) {
				if (filled) SDL_RenderDrawPoint(renderer, x+i, y+j);
				else {
					double line = sqrt(i*i + j*j);
					if (std::abs(line-static_cast<double>(r)) <= static_cast<double>(thickness)) SDL_RenderDrawPoint(renderer, x+i, y+j);
				}
			}


		}
	}

}

#endif
