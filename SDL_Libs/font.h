
#ifndef __FONT__
#define __FONT__

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

#include <iostream>
#include <string>
#include <cinttypes>
#include <exception>

#include "texture.h"

const SDL_Color DEFAULT_COLOR {0x0, 0x0, 0x0};//black
const int DEFAULT_FONT_SIZE = 28;


class Font: public Texture{

protected:

	SDL_Color color = DEFAULT_COLOR;
	std::string text;
	TTF_Font* font = nullptr;
	int fontsize = DEFAULT_FONT_SIZE;

public:

	Font();
	Font(const std::string& path, Window* w, const std::string& text, const SDL_Color& c=DEFAULT_COLOR, const int fs=DEFAULT_FONT_SIZE);
	Font(const std::string& path, Window* w, const std::string& text, const SDL_Rect& clip, const SDL_Rect& rend, const SDL_Color& c=DEFAULT_COLOR, const int fs=DEFAULT_FONT_SIZE);

	virtual ~Font();

	void load_image(const std::string& text, bool reload=false);
	void load(const std::string& path, Window* w, const std::string& text, const SDL_Color& c=DEFAULT_COLOR, const int fs=DEFAULT_FONT_SIZE);
	void reload();
	void set_text(const std::string& newtext);
	void set_color(const SDL_Color& c);
	void set_font(const std::string& f, int fs=DEFAULT_FONT_SIZE);

	Font& operator=(Font&&);
	Font& operator=(Font&);

	Font(const Font&);
	Font(const Font&&);

};


//IMPLEMENTATION
Font::Font(): Texture(){

}

Font::Font(const std::string& path, Window* w, const std::string& text, const SDL_Color& c, const int fs):Texture(){

	this->filepath = path;
	this->text = text;
	this->color = c;
	this->window = w;

	font = TTF_OpenFont(path.c_str(), fontsize);

	load_image(text);

}

Font::Font(const std::string& path, Window* w, const std::string& text, const SDL_Rect& clip, const SDL_Rect& rend, const SDL_Color& c, int fs): Texture(){
	set_cliprect(clip);
	set_renderrect(rend);
	this->path = path;
	this->text = text;
	this->color = c;
	this->fontsize = fs;
	this->window = w;

	font = TTF_OpenFont(path.c_str(), fontsize);

	load_image(text);
}

Font::~Font(){

	if (font != nullptr) TTF_CloseFont(font);

}

void Font::load_image(const std::string& text, bool reload){

	if (window->window == nullptr){
		std::cout << "your window is wrongly initialized" << "\n";
		exit(1);
	}

	if(window == nullptr) return;
	if (texture != nullptr) SDL_DestroyTexture(texture);


	SDL_Surface* s;

	if (!reload){

		if(pixelSurface != nullptr) SDL_FreeSurface(pixelSurface);

		s = TTF_RenderText_Solid(font, text.c_str(), color);
		if(window != nullptr){
			pixelSurface = SDL_ConvertSurfaceFormat(s, SDL_GetWindowPixelFormat(window->window), 0);
		}

		if(s == nullptr) {

			std::cout << "Error while loading font" << "\n";
			exit(1);
		}

		width = s->w;
		height = s->h;

	}


	texture = SDL_CreateTextureFromSurface(window->renderer, pixelSurface);
	if(!reload){
		SDL_FreeSurface(s);
	}

	set_blendmode(blendmode);
	set_alpha(alpha);


}

void Font::load(const std::string& path, Window* w, const std::string& text, const SDL_Color& c, const int fs){

	this->filepath = path;
	this->text = text;
	this->color = c;
	this->fontsize = fs;
	this->window = w;


	font = TTF_OpenFont(path.c_str(), fontsize);

	load_image(text);
}

void Font::reload(){
	load_image(this->text, true);
}

void Font::set_text(const std::string& newtext){

	text = newtext;

	load_image(text);

}
void Font::set_color(const SDL_Color& c){

	color = c;
	load_image(text);

}

void Font::set_font(const std::string& path, int fs){

	this->filepath = path;
	this->fontsize = fs;

	if(font != nullptr) TTF_CloseFont(font);

	font = TTF_OpenFont(path.c_str(), fontsize);

	load_image(text);

}

Font& Font::operator=(Font&& f){

	load(f.filepath, f.window, f.text, f.color, f.fontsize);

	return (*this);
}

Font& Font::operator=(Font& f){

	load(f.filepath, f.window, f.text, f.color, f.fontsize);

	return (*this);
}

Font::Font(const Font& f){
	load(f.filepath, f.window, f.text, f.color, f.fontsize);
}
Font::Font(const Font&& f){
	load(f.filepath, f.window, f.text, f.color, f.fontsize);
}

#endif