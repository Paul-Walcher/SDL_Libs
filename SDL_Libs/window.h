
#ifndef __WINDOW__
#define __WINDOW__

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

#include <cinttypes>
#include <string>

class Window{

protected:

	int x=0, y=0, w=0, h=0;

	bool mouseFocus=false, keyFocus=false, minimized=false, shown=false, updateCaption=false;

public:

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	int ID = -1;
	int flags = 0;

	bool hasMouseFocus();
    bool hasKeyboardFocus();
    bool isMinimized();
    bool isShown();

    Window();
	Window(std::string, int, int, int, int, uint32_t);
	virtual ~Window();
	void free();

	void hide();
	void show();
	void focus();
	void setCaption(const std::string);

	int X();
	int Y();
	int W();
	int H();

	void handle(SDL_Event&); //for handling an event

};

Window::Window(){} //undefined, just for specification
Window::Window(std::string caption, int x, int y, int w, int h, uint32_t flags){

	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	

	window = SDL_CreateWindow(caption.c_str(), x, y, w, h, flags);
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	ID = SDL_GetWindowID(window);

	this->flags = flags;

	if(flags & SDL_WINDOW_SHOWN) shown = true;
	else shown = false;


}

Window::~Window(){

}

void Window::free(){
	if(renderer != nullptr){
		SDL_DestroyRenderer(renderer);
	}
	if (window != nullptr){
		SDL_DestroyWindow(window);
	}
}

void Window::hide(){
	if (shown){
		shown = false;
		SDL_HideWindow(window);
	}
}

void Window::show(){
	if(!shown){
		shown = true;
		SDL_ShowWindow(window);
	}
}

int Window::X(){return x;}
int Window::Y(){return y;}
int Window::W(){return w;}
int Window::H(){return h;}

void Window::handle(SDL_Event& e){

	if (e.type == SDL_WINDOWEVENT && e.window.windowID == ID){

		switch( e.window.event )
        {
            //Window appeared
            case SDL_WINDOWEVENT_SHOWN:
            shown = true;
            break;

            //Window disappeared
            case SDL_WINDOWEVENT_HIDDEN:
            shown = false;
            break;

            //Get new dimensions and repaint
            case SDL_WINDOWEVENT_SIZE_CHANGED:
            w = e.window.data1;
            h = e.window.data2;
            SDL_RenderPresent( renderer);
            break;

            //Repaint on expose
            case SDL_WINDOWEVENT_EXPOSED:
            SDL_RenderPresent( renderer );
            break;

            //Mouse enter
            case SDL_WINDOWEVENT_ENTER:
            mouseFocus = true;
            updateCaption = true;
            break;
            
            //Mouse exit
            case SDL_WINDOWEVENT_LEAVE:
            mouseFocus = false;
            updateCaption = true;
            break;

            //Keyboard focus gained
            case SDL_WINDOWEVENT_FOCUS_GAINED:
            keyFocus = true;
            updateCaption = true;
            break;
            
            //Keyboard focus lost
            case SDL_WINDOWEVENT_FOCUS_LOST:
            keyFocus = false;
            updateCaption = true;
            break;

            //Window minimized
            case SDL_WINDOWEVENT_MINIMIZED:
            minimized = true;
            break;

            //Window maxized
            case SDL_WINDOWEVENT_MAXIMIZED:
            minimized = false;
            break;
            
            //Window restored
            case SDL_WINDOWEVENT_RESTORED:
            minimized = false;
            break;

	}

	}
}

void Window::focus(){


	show();
	SDL_RaiseWindow(window);

}

void Window::setCaption(const std::string cap){
	SDL_SetWindowTitle(window, cap.c_str());
}


#endif