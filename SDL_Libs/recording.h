
#ifndef __RECORDING__
#define __RECORDING__

#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#include <iostream>
#include <cinttypes>

enum AUDIOSTATE{

	IDLE,
	RECORDING,
	PLAYING

};

static AUDIOSTATE globalState = AUDIOSTATE::IDLE;
static SDL_AudioSpec used_spec;
static SDL_AudioDeviceID used_ID;
static uint8_t* used_data;
static int buffersize = 0;
static int bufferpointer = 0;


uint8_t* record(int milliseconds, int id=-1, SDL_AudioSpec* spec = nullptr);
void playback(uint8_t* data, int milliseconds, int id=-1, SDL_AudioSpec* spec = nullptr);

static void recording_callback(void*, uint8_t*, int);
static void playback_callback(void*, uint8_t*, int);


uint8_t* record(int milliseconds, int id, SDL_AudioSpec* spec){

	if (globalState != AUDIOSTATE::IDLE){
		return nullptr;
	}

	SDL_AudioSpec Rspec;
	if (spec != nullptr) Rspec = (*spec);
	else{
		SDL_zero(Rspec);
        Rspec.freq = 44100;
        Rspec.format = AUDIO_F32;
        Rspec.channels = 2;
        Rspec.samples = 4096;
        Rspec.callback = recording_callback;
	}
	SDL_AudioSpec rcvd;

	id = (id == -1 ? 0 : id);

	used_ID = SDL_OpenAudioDevice(SDL_GetAudioDeviceName( id, SDL_TRUE ), SDL_TRUE, &Rspec, &rcvd, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

	used_spec = rcvd;

	int bytesPerSample = rcvd.channels * (SDL_AUDIO_BITSIZE( rcvd.format ) / 8 );
    int bytesPerMillisecond = rcvd.freq * bytesPerSample / 1000;

    buffersize = (milliseconds+1) * bytesPerMillisecond;

    uint8_t* buffer = new uint8_t[ buffersize ];
    memset( buffer, 0, buffersize);

    used_data = buffer;
    bufferpointer = 0;

    SDL_PauseAudioDevice(used_ID, SDL_FALSE);

   	globalState = AUDIOSTATE::RECORDING;


	return buffer;
}
void playback(uint8_t* data, int milliseconds, int id, SDL_AudioSpec* spec){

	if (globalState != AUDIOSTATE::IDLE) return;

	SDL_AudioSpec Pspec;
	if (spec != nullptr) Pspec = (*spec);
	else{
		SDL_zero(Pspec);
        Pspec.freq = 44100;
        Pspec.format = AUDIO_F32;
        Pspec.channels = 2;
        Pspec.samples = 4096;
        Pspec.callback = playback_callback;
	}

	SDL_AudioSpec rcvd;

	const char* out = (id == -1 ? nullptr : SDL_GetAudioDeviceName(id, SDL_FALSE));



	used_ID = SDL_OpenAudioDevice(out, SDL_FALSE, &Pspec, &rcvd, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

	int bytesPerSample = rcvd.channels * ( SDL_AUDIO_BITSIZE( rcvd.format ) / 8 );
    int bytesPerMillisecond = rcvd.freq * bytesPerSample / 1000;

    buffersize = (milliseconds+1) * bytesPerMillisecond;

    bufferpointer = 0;

	used_spec = rcvd;
	used_data = data;

	SDL_PauseAudioDevice(used_ID, SDL_FALSE);

	globalState = AUDIOSTATE::PLAYING;

}

static void recording_callback(void* udata, uint8_t* stream, int len){

	memcpy(&used_data[bufferpointer], stream, len);
	bufferpointer += len;

	if(bufferpointer + len >= buffersize){//end recording
		SDL_CloseAudioDevice(used_ID);

		globalState = AUDIOSTATE::IDLE;
	}

}
static void playback_callback(void* udata, uint8_t* stream, int len){


	memcpy(stream, &used_data[bufferpointer], len);
	bufferpointer += len;


	if(bufferpointer + len >= buffersize){//end recording

		SDL_CloseAudioDevice(used_ID);

		globalState = AUDIOSTATE::IDLE;
	}

}



#endif