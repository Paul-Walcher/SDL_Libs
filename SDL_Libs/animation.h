
#ifndef __ANIMATION__
#define __ANIMATION__

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
#include <vector>
#include <chrono>

const int STANDARD_ANIMATION_SPEED  = 100; //in milliseconds
static std::vector<SDL_Rect*> STANDARD_CLIPRECTS = {nullptr};
static std::vector<unsigned int> STANDARD_CHANGE_TIMES = {STANDARD_ANIMATION_SPEED};

/*
*
*Example usage:
*	Animation sprite;
*	//initializing renderer usw.
*	sprite.load(renderer, "PATH.png");
*	//doing stuff
*	while (running){
	
		//game loop stuff

	sprite.make_update();
	sprite.draw();
	//stuff
	}
*
*	
*
*/

class Animation{

private:

	Texture texture;

	SDL_Renderer* renderer = nullptr;
	SDL_Rect* renderrect = nullptr;

	uint8_t alpha = 0xff;
	SDL_BlendMode blendmode = STANDARD_BLENDMODE;

	std::vector<std::string> paths; //all the different paths, a path gets changed whe the corresponding change time is reached
	std::vector<SDL_Rect*> cliprects; //different cliprects
	std::vector<unsigned int> change_times; //the change times, after the change time is reached change_time_ptr is updated

	unsigned int last_path_ptr = 0; //for checking if image has changed
	unsigned int path_ptr = 0;
	unsigned int cliprect_ptr = 0;
	unsigned int change_time_ptr = 0;

	uint8_t r=0xff, g=0xff, b=0xff; //for color modulation

	bool running = true; //if the animation is running

	std::chrono::milliseconds last_update = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	SDL_RendererFlip flipType = STANDARD_FLIPTYPE;
	double angle=0.0;
	SDL_Point* center = nullptr;

	void load_texture();

public:

	Animation();
	Animation(SDL_Renderer* renderer, std::vector<std::string>& paths, std::vector<SDL_Rect*>& cliprects=STANDARD_CLIPRECTS, std::vector<unsigned int>& change_times=STANDARD_CHANGE_TIMES, SDL_Rect* renderrect=nullptr, bool running=true);
	Animation(SDL_Renderer* renderer, std::vector<std::string> paths, std::vector<SDL_Rect*> cliprects=STANDARD_CLIPRECTS, std::vector<unsigned int> change_times=STANDARD_CHANGE_TIMES, SDL_Rect* renderrect=nullptr, bool running=true);

	virtual ~Animation();

	//functions, mostly from Texture

	void load(SDL_Renderer* renderer, std::vector<std::string> paths, std::vector<SDL_Rect*> cliprects=STANDARD_CLIPRECTS, std::vector<unsigned int> change_times=STANDARD_CHANGE_TIMES);

	int get_width() const;
	int get_height() const;

	//setting the two rects
	void set_cliprects(std::vector<SDL_Rect*>);
	void set_renderrect(const SDL_Rect&);
	//for getting rid of any rectviews
	void unset_cliprects();
	void unset_renderrect();
	//if you change the renderer, this can be updated here
	void set_renderer(SDL_Renderer*);

	//drawing the animation onto the screen
	void draw();
	void draw(int x, int y, int w=-1, int h=-1);

	//modulating the image color
	void modulate_color(const uint8_t, const uint8_t, const uint8_t);
	//for blending
	void set_blendmode(const SDL_BlendMode);
	const SDL_BlendMode get_blendmode() const;
	void set_alpha(const uint8_t);
	const uint8_t get_alpha() const;

	void set_angle(const double);
	void rotation_config(const SDL_Point&, const SDL_RendererFlip f=STANDARD_FLIPTYPE);

	//animation functions
	void stop_animation();
	void start_animation();
	void set_path_ptr(const unsigned int);
	void set_cliprect_ptr(const unsigned int);
	void set_change_time_ptr(const unsigned int);

	//needs to be called for the sprite to be update
	void make_update();
	//resetting the animation
	void reset_animation();
	void reset_timer();

};

Animation::Animation(){

}

Animation::Animation(SDL_Renderer* renderer, std::vector<std::string>& paths, std::vector<SDL_Rect*>& cliprects, std::vector<unsigned int>& change_times, SDL_Rect* renderrect, bool running){

	this->renderer = renderer;
	this->paths = paths;
	this->cliprects = cliprects;
	this->change_times = change_times;
	this->renderrect = renderrect;
	this->running = running;

	last_path_ptr = path_ptr+1;

	load_texture();
}

Animation::Animation(SDL_Renderer* renderer, std::vector<std::string> paths, std::vector<SDL_Rect*> cliprects, std::vector<unsigned int> change_times, SDL_Rect* renderrect, bool running){
	this->renderer = renderer;
	this->paths = paths;
	this->cliprects = cliprects;
	this->change_times = change_times;
	this->renderrect = renderrect;
	this->running = running;

	last_path_ptr = path_ptr+1;

	load_texture();
}

Animation::~Animation(){

	if (renderrect != nullptr) delete renderrect;
	for(int i = 0; i < cliprects.size(); i++) {
		if(cliprects[i] != nullptr){
			delete cliprects[i];
		}
	}

	if (center != nullptr) delete center;

}

void Animation::load(SDL_Renderer* renderer, std::vector<std::string> paths, std::vector<SDL_Rect*> cliprects, std::vector<unsigned int> change_times){
	this->renderer = renderer;
	this->paths = paths;
	this->cliprects = cliprects;
	this->change_times = change_times;

	last_path_ptr = path_ptr+1;

	load_texture();
}

void Animation::load_texture(){
	if (last_path_ptr != path_ptr){
		texture.load(paths[path_ptr], renderer);
		last_path_ptr = path_ptr;
	}
	texture.set_cliprect(*(cliprects[cliprect_ptr]));
	if(renderrect != nullptr){
		texture.set_renderrect(*(renderrect));
	}

	texture.modulate_color(r, g, b);
	texture.set_blendmode(blendmode);
	texture.set_alpha(alpha);
	texture.set_angle(angle);
	if(center != nullptr){
		texture.rotation_config(*(center), flipType);
	}
}

int Animation::get_width() const{

	return texture.get_width();

}

int Animation::get_height() const{
	return texture.get_height();
}

void Animation::set_cliprects(std::vector<SDL_Rect*> rects){
	cliprects = rects;
	if (cliprect_ptr >= cliprects.size()) cliprect_ptr = 0;
	if (cliprects[cliprect_ptr] != nullptr){
		texture.set_cliprect(*(cliprects[cliprect_ptr]));
	}
	else texture.unset_cliprect();
}

void Animation::set_renderrect(const SDL_Rect& r){
	if(renderrect != nullptr) delete renderrect;
	renderrect = new SDL_Rect{r};
	texture.set_renderrect(*(renderrect));
}

void Animation::unset_cliprects(){
	cliprects=STANDARD_CLIPRECTS;
	cliprect_ptr = 0;
	texture.unset_cliprect();
}

void Animation::unset_renderrect(){
	if (renderrect != nullptr) delete renderrect;
	renderrect = nullptr;
	texture.unset_renderrect();
}

void Animation::set_renderer(SDL_Renderer* r){
	renderer = r;
	texture.set_renderer(r);
}

void Animation::modulate_color(uint8_t r, uint8_t g, uint8_t b){
	this->r = r;
	this->g = g;
	this->b = b;
	texture.modulate_color(r, g, b);
}

void Animation::set_blendmode(const SDL_BlendMode b){

	blendmode = b;
	texture.set_blendmode(b);

}

const SDL_BlendMode Animation::get_blendmode() const{

	return blendmode;

}

void Animation::set_alpha(const uint8_t a){

	alpha = a;
	texture.set_alpha(a);

}

const uint8_t Animation::get_alpha() const{
	return alpha;
}

void Animation::make_update(){
	if (running){


		std::chrono::milliseconds update = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		std::chrono::milliseconds to_elapse = std::chrono::milliseconds(change_times[change_time_ptr]);

		if ((update - last_update) > to_elapse){
			//update is needed
			last_update = update;

			path_ptr += 1;
			if(path_ptr >= paths.size()) path_ptr = 0;

			cliprect_ptr += 1;
			if (cliprect_ptr >= cliprects.size()) cliprect_ptr = 0;

			change_time_ptr += 1;
			if(change_time_ptr >= change_times.size()) change_time_ptr = 0;

			//loading new image
			load_texture();

		}

	}
}

void Animation::draw() {

	//change happened, able to draw now
	texture.draw();

}

void Animation::draw(int x, int y, int w, int h){

	//change happend, able to draw now
	texture.draw(x, y, w, h);
}


//animation functions
void Animation::stop_animation(){
	running = false;
}

void Animation::start_animation(){
	running = true;
}

void Animation::set_path_ptr(const unsigned int pp){
	if(pp < paths.size()) path_ptr = pp;
}

void Animation::set_cliprect_ptr(const unsigned int cp){
	if (cp < cliprects.size()) cliprect_ptr = cp;
}	

void Animation::set_change_time_ptr(const unsigned int ctp){
	if(ctp < change_times.size()) change_time_ptr = ctp;
}

void Animation::reset_animation(){

	path_ptr = 0;
	cliprect_ptr = 0;
	change_time_ptr = 0;

	last_update = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

}

void Animation::reset_timer(){
	last_update = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

void Animation::set_angle(const double a){
	angle = a;
	texture.set_angle(a);
}

void Animation::rotation_config(const SDL_Point& p, const SDL_RendererFlip f){

	if(center != nullptr) delete center;
	center = new SDL_Point{p};

	flipType = f;

	texture.rotation_config(p, f);
}


#endif