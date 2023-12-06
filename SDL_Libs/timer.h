
#ifndef __TIMER__
#define __TIMER__

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

#include <vector>
#include <cinttypes>
#include <algorithm>
#include <thread>
#include <mutex>

class Timer;
//time gets all checked in milliseconds

const int INFINITE_REPETITIONS = -1;//execute the function inifinite times
const uint32_t STANDARD_CHECK_TIME = 1;//checks every time
static void do_nothing(void*){}//does nothing
static void run_process(Timer* time);

/*
*
*	After timer gets started, you can give the timer a function, a time and a repetition time.
*	The timer will execute the function repetition times oft ater the specified time
*
*/

typedef void(*callback)(void*);
struct callback_t{
	callback f = do_nothing;
	uint32_t frequency = -1;//plays after the longest time possible
	int repetitions = INFINITE_REPETITIONS;
	int repeated = 0;
	void* input = nullptr;
	uint32_t last_time_checked = SDL_GetTicks();

};


class Timer{

private:

	uint32_t timer_birthday;//when the timer was created
	uint32_t last_stop;//last stop to check passed time
	uint32_t check_time;//if the time should even be checked right now

	std::vector<callback_t*> callbacks;//all the callback functions

	bool paused = false;
	bool finished = false;

	std::thread proc;
	std::mutex lock;

public:

	Timer();
	Timer(std::vector<callback_t*>& callbacks, uint32_t check_time=STANDARD_CHECK_TIME);

	virtual ~Timer();

	const uint32_t get_time_elapsed() const;//gives back the time elapsed since birth

	callback_t* make_callback(callback f, uint32_t frequency, int repetitions=INFINITE_REPETITIONS);//creates a callback and gives back a reference to the callback, 
																								//for possible later changes. Be careful tho, the pointer could point to nowhere
	const std::vector<callback_t*>& get_all_callbacks() const;//to get all callbacks
	void remove_callback(const callback_t*);//removes the callback from the list
	const uint32_t get_checktime() const;
	void set_checktime(const uint32_t);

	void tick();// executes the function
	void pause();
	void start();
	void reset();//resets all callbacks
	void reset_callbacks();//resets all callbacks and syncs them
	void stop();

	bool run_inner_process();

	const bool is_paused() const;


};

static void run_process(Timer* time){
	bool running = true;
	while(running){
		running = time->run_inner_process();
	}
}

Timer::Timer(){
	timer_birthday = SDL_GetTicks();
	last_stop = timer_birthday;
	check_time = STANDARD_CHECK_TIME;
	proc = std::thread(run_process, this);
}

Timer::Timer(std::vector<callback_t*>& callbacks, uint32_t check_time){

	this->timer_birthday = SDL_GetTicks();
	this->last_stop = timer_birthday;
	this->check_time = check_time;
	this->callbacks = callbacks;

	proc = std::thread(run_process, this);

}

bool Timer::run_inner_process(){

	lock.lock();
	if (finished){
		lock.unlock();
		return false;
	} 
	if (!paused) tick();
	lock.unlock();
	return true;

}

Timer::~Timer(){

	for(callback_t* ct : callbacks) delete ct;
	lock.lock();
	finished = true;
	lock.unlock();
	proc.join();
}

const uint32_t Timer::get_time_elapsed() const{
	return SDL_GetTicks()-timer_birthday;
}

callback_t* Timer::make_callback(callback f, uint32_t frequency, int repetitions){

	struct callback_t* new_callback = new struct callback_t;
	new_callback->f = f;
	new_callback->frequency = frequency;
	new_callback->repetitions = repetitions;
	callbacks.push_back(new_callback);
	return new_callback;

}

const std::vector<callback_t*>& Timer::get_all_callbacks() const{
	return callbacks;
}
void Timer::remove_callback(const callback_t* to_remove){
	callbacks.erase(std::find(callbacks.begin(), callbacks.end(), to_remove));
	delete to_remove;
}
const uint32_t Timer::get_checktime() const{
	return check_time;
}
void Timer::set_checktime(const uint32_t new_checktime){
	check_time = new_checktime;
}


void Timer::tick(){// executes the function

	if (!paused){

		uint32_t nt = SDL_GetTicks();
		if (nt-last_stop >= check_time){

			last_stop = nt;

			std::vector<callback_t*> to_remove; //for all callbacks to be removed

			last_stop = nt;
			for(callback_t* ct : callbacks){

				if (nt-ct->last_time_checked >= ct->frequency){//it is time to execute the function
					ct->last_time_checked = nt;
					ct->f(ct->input);
					ct->repeated += 1;
					if (ct->repetitions >= 1 && ct->repeated >= ct->repetitions){
						to_remove.push_back(ct);
					}

				}

			}

			for(int i = 0; i < to_remove.size();i++){
				remove_callback(to_remove[i]);
			}


		}

	}

}

void Timer::pause(){
	lock.lock();
	paused = true;
	lock.unlock();
	uint32_t reset_time = SDL_GetTicks();
	for(callback_t* ct : callbacks){
		ct->last_time_checked = reset_time;
	}
}
void Timer::start(){
	lock.lock();
	paused = false;
	lock.unlock();
}
void Timer::reset(){

	timer_birthday = SDL_GetTicks();
	last_stop = timer_birthday;
	for(callback_t* ct : callbacks){
		ct->repeated = 0;
		ct->last_time_checked = timer_birthday;
	}

}

void Timer::reset_callbacks(){
	uint32_t reset_time = SDL_GetTicks();
	for(callback_t* ct : callbacks){
		ct->repeated = 0;
		ct->last_time_checked = reset_time;
	}
}

void Timer::stop(){
	timer_birthday = SDL_GetTicks();
	last_stop = timer_birthday;
	for(callback_t* ct : callbacks){
		delete ct;
	}
	std::vector<callback_t*> nc;
	callbacks = nc;
}


const bool Timer::is_paused() const{
	return paused;
}



#endif

