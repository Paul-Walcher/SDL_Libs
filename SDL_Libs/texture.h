
#ifndef __TEXTURE__
#define __TEXTURE__

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

const SDL_BlendMode STANDARD_BLENDMODE = SDL_BLENDMODE_BLEND;
const SDL_RendererFlip STANDARD_FLIPTYPE = SDL_FLIP_NONE;

class Texture{

protected:

	SDL_Texture* texture = nullptr;
	SDL_Renderer* renderer = nullptr;

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

public:

	std::string path;
	//for making a texture uninitialized
	Texture();
	//loads the texture preinitialized
	Texture(const std::string&, SDL_Renderer*);
	Texture(const std::string&, SDL_Renderer*, const SDL_Rect&, const SDL_Rect&);

	virtual ~Texture();

	void load(const std::string&, SDL_Renderer*);

	int get_width() const;
	int get_height() const;

	//for independent loading
	void load_image(const std::string&);
	//setting the two rects
	void set_cliprect(const SDL_Rect&);
	void set_renderrect(const SDL_Rect&);
	//for getting rid of any rectviews
	void unset_cliprect();
	void unset_renderrect();
	//if you change the renderer, this can be updated here
	void set_renderer(SDL_Renderer*);
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
	//drawing the image onto the screen
	void draw() const;
	void draw(int x, int y, int w=-1, int h=-1) const;

};

Texture::Texture(){

}

Texture::Texture(const std::string& path, SDL_Renderer* r){

	this->renderer = r;
	load_image(path);

}

Texture::Texture(const std::string& path, SDL_Renderer* r, const SDL_Rect& cr, const SDL_Rect& rr){

	this->renderer = r;
	load_image(path);
	set_cliprect(cr);
	set_renderrect(rr);
}

Texture::~Texture(){

	SDL_DestroyTexture(texture);

	if (cliprect != nullptr) delete cliprect;
	if (renderrect != nullptr) delete renderrect;
	if(center != nullptr) delete center;

}

void Texture::load(const std::string& path, SDL_Renderer* r){
	this->renderer = r;
	load_image(path);
}

int Texture::get_width() const {
	return width;
}

int Texture::get_height() const{
	return height;
}

void Texture::load_image(const std::string& path){

	if (texture != nullptr) SDL_DestroyTexture(texture);

	this->path = path;

	SDL_Surface* s = IMG_Load(path.c_str());
	if(s == nullptr) {
		std::cout << "Image with path: " << path << " could not be loaded" << std::endl; 
		exit(1);
	}

	width = s->w;
	height = s->h;

	texture = SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);

	set_blendmode(blendmode);
	set_alpha(alpha);
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

void Texture::set_renderer(SDL_Renderer* r){
	renderer = r;
}

void Texture::draw() const {
	if (renderer == nullptr || texture == nullptr) return;
	SDL_RenderCopyEx(renderer, texture, cliprect, renderrect, angle, center, flipType);
}

void Texture::draw(int x, int y, int w, int h) const {


	if (renderer == nullptr || texture == nullptr) return;
	if( w == -1) w = width;
	if (h == -1) h = height;
	SDL_Rect r = {x, y, w, h};
	SDL_RenderCopyEx(renderer, texture, cliprect, &r, angle, center, flipType);

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
	}
}

const SDL_BlendMode Texture::get_blendmode() const{
	return blendmode;
}

void Texture::set_alpha(const uint8_t a){
	this->alpha = a;
	if (this->texture != nullptr){
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


#endif