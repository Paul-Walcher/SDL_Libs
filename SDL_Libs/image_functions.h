
#ifndef __IMAGE_FUNCTIONS__
#define __IMAGE_FUNCTIONS__

#include <string>

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

SDL_Surface* load_image(const std::string&, const SDL_Surface*);
SDL_Texture* load_texture(const std::string&, SDL_Renderer* r);



SDL_Surface* load_image(const std::string& path, const SDL_Surface* w){
	SDL_Surface* loaded = IMG_Load(path.c_str());

	if (loaded == nullptr) {return nullptr;}

	SDL_Surface* opt = SDL_ConvertSurface(loaded, w->format, 0);
	if (opt != nullptr){
		SDL_FreeSurface(loaded);
		loaded = opt;
	}

	return loaded;
}

SDL_Texture* load_texture(const std::string& path, SDL_Renderer* r){

	SDL_Surface* temp;
	temp = IMG_Load(path.c_str());

	if (temp == nullptr) return nullptr;

	SDL_Texture* back = SDL_CreateTextureFromSurface(r, temp);
	SDL_FreeSurface(temp);
	return back;
}

#endif