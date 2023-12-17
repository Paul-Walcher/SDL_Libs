
#ifndef __TEXTURE__
#define __TEXTURE__

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
#include "window.h"

const SDL_BlendMode STANDARD_BLENDMODE = SDL_BLENDMODE_BLEND;
const SDL_RendererFlip STANDARD_FLIPTYPE = SDL_FLIP_NONE;

class Texture{

protected:

	SDL_Texture* texture = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Surface* pixelSurface = nullptr;
	Window* window;

	//cliprect is the rectangle that specifies what part of the image is shown
	//renderrect specifies where the image will be rendered
	SDL_Rect *cliprect = nullptr, *renderrect = nullptr;
	//blendmode, fliptype and center
	SDL_BlendMode blendmode = STANDARD_BLENDMODE;
	SDL_RendererFlip flipType = STANDARD_FLIPTYPE;
	SDL_Point* center = nullptr;

	int width = -1, height = -1;
	uint8_t alpha = 0xff;
	double angle=0.0;
	std::string filepath;

public:

	std::string path;
	//for making a texture uninitialized
	Texture();
	//loads the texture preinitialized
	Texture(const std::string&, Window* window_ptr);
	Texture(const std::string&, Window* window_ptr, const SDL_Rect&, const SDL_Rect&);

	virtual ~Texture();

	//loads the texture new from the path to SDL_renderer
	void load(const std::string&, Window* window_ptr);
	void reload();

	int get_width() const;
	int get_height() const;

	//for independent loading
	void load_image(const std::string&, bool reload = false);
	//setting the two rects
	void set_cliprect(const SDL_Rect&);
	void set_renderrect(const SDL_Rect&);
	//for getting rid of any rectviews
	void unset_cliprect();
	void unset_renderrect();
	//if you change the renderer, this can be updated here
	void set_window(Window* window_ptr);
	Window const* get_window() const;
	//modulating the image color
	void modulate_color(const uint8_t, const uint8_t, const uint8_t);
	//for blending
	void set_blendmode(const SDL_BlendMode);
	const SDL_BlendMode get_blendmode() const;
	void set_alpha(const uint8_t);
	const uint8_t get_alpha() const;
	void set_angle(const double);
	const double get_angle();
	void rotation_config(const SDL_Point&, const SDL_RendererFlip f=STANDARD_FLIPTYPE);
	
	uint32_t* get_pixels();
	uint32_t get_pitch(); //width of the pixel line

	void create_blank(int, int, SDL_TextureAccess acc = SDL_TEXTUREACCESS_TARGET);
	void set_as_render_target(SDL_Renderer*);
	void unset_as_render_target(SDL_Renderer* r);

	//drawing the image onto the screen
	void draw() const;
	void draw(int x, int y, int w=-1, int h=-1) const;

	Texture& operator=(Texture&&);
	Texture& operator=(Texture&);
	Texture(const Texture&);
	Texture(const Texture&&);

};

Texture::Texture(){
}

Texture::Texture(const std::string& path, Window* window_ptr){

	this->filepath = path;
	this->window = window_ptr;
	load_image(path);

}

Texture::Texture(const std::string& path, Window* window_ptr, const SDL_Rect& cr, const SDL_Rect& rr){

	this->filepath = path;
	this->window = window_ptr;
	load_image(path);
	set_cliprect(cr);
	set_renderrect(rr);
}

Texture::~Texture(){



	if (texture != nullptr) SDL_DestroyTexture(texture);
	if(pixelSurface != nullptr) SDL_FreeSurface(pixelSurface);

	if (cliprect != nullptr) delete cliprect;
	if (renderrect != nullptr) delete renderrect;
	if(center != nullptr) delete center;

}

void Texture::load(const std::string& path, Window* window_ptr){
	

	this->filepath = path;
	this->window = window_ptr;
	load_image(path);
}

int Texture::get_width() const {
	return width;
}

int Texture::get_height() const{
	return height;
}

void Texture::load_image(const std::string& path, bool reload){


	if (window == nullptr) return;
	if (texture != nullptr) SDL_DestroyTexture(texture);

	SDL_Surface* s;

	if (!reload){
		filepath = path;
		if (pixelSurface != nullptr) SDL_FreeSurface(pixelSurface);
		s = IMG_Load(path.c_str());
		this->path = path;
		if(s == nullptr) {
			std::cout << "Image with path: " << path << " could not be loaded" << std::endl; 
			exit(1);
		}

		if (window != nullptr){
			pixelSurface = SDL_ConvertSurfaceFormat(s, SDL_GetWindowPixelFormat(window->window), 0);
		}


		width = s->w;
		height = s->h;	

		set_blendmode(blendmode);
		set_alpha(alpha);

	}

	if (pixelSurface != nullptr){
		texture = SDL_CreateTextureFromSurface(window->renderer, pixelSurface);
	}
	else if(!reload){
		texture = SDL_CreateTextureFromSurface(window->renderer, s);
	}

	if (!reload){
		SDL_FreeSurface(s);
	}

	

}

void Texture::reload(){
	load_image(filepath, true);
}

void Texture::set_cliprect(const SDL_Rect& cr){

	if (cliprect != nullptr){
		delete cliprect;
	}
	cliprect = new SDL_Rect{cr};
}

void Texture::set_renderrect(const SDL_Rect& rr){

	if (renderrect != nullptr){
		delete renderrect;
	}
	renderrect = new SDL_Rect{rr};
}

void Texture::unset_cliprect(){
	if (cliprect != nullptr) delete cliprect;
}

void Texture::unset_renderrect(){
	if (renderrect != nullptr) delete renderrect;
}

void Texture::set_window(Window* window_ptr){
	this->window = window_ptr;
	load_image(this->filepath);
}

Window const* Texture::get_window() const{
	return window;
}

void Texture::draw() const {
	if (window == nullptr || texture == nullptr) return;
	SDL_RenderCopyEx(window->renderer, texture, cliprect, renderrect, angle, center, flipType);
}

void Texture::draw(int x, int y, int w, int h) const {


	if (window == nullptr || texture == nullptr) return;
	if( w == -1) w = width;
	if (h == -1) h = height;
	SDL_Rect r = {x, y, w, h};
	SDL_RenderCopyEx(window->renderer, texture, cliprect, &r, angle, center, flipType);

}

void Texture::modulate_color(const uint8_t r, const uint8_t g, const uint8_t b){
	if(texture != nullptr){
		SDL_SetTextureColorMod(texture, r, g, b);
	}
}

void Texture::set_blendmode(const SDL_BlendMode b){
	this->blendmode = b;
	if(this->texture != nullptr){
		SDL_SetTextureBlendMode(texture, blendmode);
		SDL_SetTextureAlphaMod(texture, alpha);
	}
}

const SDL_BlendMode Texture::get_blendmode() const{
	return blendmode;
}

void Texture::set_alpha(const uint8_t a){
	this->alpha = a;
	if (this->texture != nullptr){
		SDL_SetTextureBlendMode(texture, blendmode);
		SDL_SetTextureAlphaMod(texture, alpha);
	}
}

const uint8_t Texture::get_alpha() const{
	return alpha;
}

void Texture::set_angle(const double a){
	angle = a;
}
const double Texture::get_angle(){
	return angle;
}

void Texture::rotation_config(const SDL_Point& p, const SDL_RendererFlip f){
	if(center != nullptr) delete center;
	center = new SDL_Point{p};

	flipType = f;

}

uint32_t* Texture::get_pixels(){
	if (pixelSurface == nullptr) return nullptr;
	return static_cast<uint32_t*>(pixelSurface->pixels);
}

uint32_t Texture::get_pitch(){

	uint32_t back = 0;
	if (pixelSurface != nullptr) back = pixelSurface->pitch >> 2;
	return back;
}

void Texture::create_blank(int width, int height, SDL_TextureAccess access){

	if(window == nullptr) return;
	if (texture != nullptr) SDL_DestroyTexture(texture);
	if (pixelSurface != nullptr) SDL_FreeSurface(pixelSurface);

	texture = SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_RGBA8888, access, width, height);

}

void Texture::set_as_render_target(SDL_Renderer* r){
	if (texture == nullptr) return;
	SDL_SetRenderTarget(r, texture);
}

void Texture::unset_as_render_target(SDL_Renderer* r){
	SDL_SetRenderTarget(r, nullptr);
}

Texture& Texture::operator=(Texture&& t){

	load(t.filepath, t.window);
	return (*this);
}

Texture& Texture::operator=(Texture& t){

	load(t.filepath, t.window);
	return (*this);
}

Texture::Texture(const Texture& other){
	load(other.filepath, other.window);
}
Texture::Texture(const Texture&& other){
	load(other.filepath, other.window);
}

#endif