
#ifndef __FONT__
#define __FONT__

#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#include <iostream>
#include <string>
#include <cinttypes>

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
	Font(const std::string& path, SDL_Renderer* r, const std::string& text, const SDL_Color& c=DEFAULT_COLOR, const int fs=DEFAULT_FONT_SIZE);
	Font(const std::string& path, SDL_Renderer* r, const std::string& text, const SDL_Rect& clip, const SDL_Rect& rend, const SDL_Color& c=DEFAULT_COLOR, const int fs=DEFAULT_FONT_SIZE);

	virtual ~Font();

	void load_image(const std::string& text);
	void load(const std::string& path, SDL_Renderer* r, const std::string& text, const SDL_Color& c, const int fs=DEFAULT_FONT_SIZE);
	void set_text(const std::string& newtext);
	void set_color(const SDL_Color& c);
	void set_font(const std::string& f, int fs=DEFAULT_FONT_SIZE);

};


//IMPLEMENTATION
Font::Font(): Texture(){

}

Font::Font(const std::string& path, SDL_Renderer* r, const std::string& text, const SDL_Color& c, const int fs):Texture(){

	this->path = path;
	this->renderer = r;
	this->text = text;
	this->color = c;
	this->fontsize = fs;

	font = TTF_OpenFont(path.c_str(), fontsize);

	load_image(text);

}

Font::Font(const std::string& path, SDL_Renderer* r, const std::string& text, const SDL_Rect& clip, const SDL_Rect& rend, const SDL_Color& c, int fs): Texture(){
	set_cliprect(clip);
	set_renderrect(rend);
	this->path = path;
	this->renderer = r;
	this->text = text;
	this->color = c;
	this->fontsize = fs;

	font = TTF_OpenFont(path.c_str(), fontsize);

	load_image(text);
}

Font::~Font(){

	TTF_CloseFont(font);
	Texture::~Texture();
}

void Font::load_image(const std::string& text){

	if (texture != nullptr) SDL_DestroyTexture(texture);


	SDL_Surface* s = TTF_RenderText_Solid(font, text.c_str(), color);
	if(s == nullptr) {
		exit(1);
	}

	width = s->w;
	height = s->h;

	texture = SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);

	set_blendmode(blendmode);
	set_alpha(alpha);

}

void Font::load(const std::string& path, SDL_Renderer* r, const std::string& text, const SDL_Color& c, const int fs){

	this->path = path;
	this->renderer = r;
	this->text = text;
	this->color = c;
	this->fontsize = fs;


	font = TTF_OpenFont(path.c_str(), fontsize);

	load_image(text);
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

	this->path = path;
	this->fontsize = fs;

	if(font == nullptr) TTF_CloseFont(font);

	font = TTF_OpenFont(path.c_str(), fontsize);

	load_image(text);

}


#endif