
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

#define SDL_LIBS_PATH "SDL_lib.h"

#include SDL_LIBS_PATH

void init();
void deinit();
void gameloop();

const int WIDTH = 800;
const int HEIGHT = 600;
const int STARTX = 400;
const int STARTY = 100;
const int AUDIO_QUALITY = 2048;
const int FPS = 60;
const int FPS_DELAY = 1000/FPS;

const int IMG_INIT_FLAGS = IMG_INIT_PNG;

Window window;
SDL_Event sdl_event;

int main(){

	init();
	gameloop();
	deinit();

	return 0;
}

void init(){

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_FLAGS);
	TTF_Init();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, AUDIO_QUALITY);


	window = Window(STARTX, STARTY, WIDTH, HEIGHT);
	SDL_SetRenderDrawColor(window.renderer, 0x0, 0x0, 0x0, 0x0);
}

void gameloop(){

	bool quit = false;

	while (!quit){

		SDL_Delay(FPS_DELAY);

		while(SDL_PollEvent(&sdl_event)){

			switch (sdl_event.type){

			case SDL_QUIT:
							quit = true;
							break;

			}

		}

		SDL_RenderClear(window.renderer);
		SDL_SetRenderDrawColor(window.renderer, 0x0, 0x0, 0x0, 0x0);

		SDL_RenderPresent(window.renderer);

	}

}

void deinit(){


	window.free();

	Mix_Quit();
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

}