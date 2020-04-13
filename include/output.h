#ifndef DISPLAY
#define DISPLAY

#include <SDL2/SDL.h>
#include "types.h"
#include "constants.h"

int initialize_display(SDL_Window **, SDL_Renderer **);

uint8 update_display(SDL_Renderer **, uint8 *);

void destroy_display(SDL_Window **, SDL_Renderer **);

struct Wave {
    uint16 sample_rate;
    uint16 frequency;
    uint16 amplitude;
    uint16 period;
};

int initialize_audio();

void emit_audio(SDL_AudioDeviceID);

void silence_audio(SDL_AudioDeviceID);

void destroy_audio_device(SDL_AudioDeviceID);

#endif
