#ifndef DISPLAY
#define DISPLAY

#include <SDL2/SDL.h>
#include "types.h"
#include "constants.h"

int initialize_display(SDL_Window **, SDL_Renderer **);

uint8 update_display(SDL_Renderer **, uint8 *);

void destroy_display(SDL_Window **, SDL_Renderer **);

#endif
