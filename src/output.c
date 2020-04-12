#include "output.h"

enum BG_COLOR {
    BG_RED = 40,
    BG_GREEN = 40,
    BG_BlUE = 40,
    BG_ALPHA = 255
};

enum TEXT_COLOR {
    TEXT_RED = 255,
    TEXT_GREEN = 176,
    TEXT_BlUE = 0,
    TEXT_ALPHA = 75
};

int initialize_display(SDL_Window **window, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL. Error: %s\n", SDL_GetError());
        return 0;
    }

    *window = SDL_CreateWindow(
        DISPLAY_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        (DISPLAY_WIDTH * SCALE),
        (DISPLAY_HEIGHT * SCALE),
        SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Failed to create window. Error: %s\n", SDL_GetError());
        return 0;
    }

    *renderer = SDL_CreateRenderer(
        *window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) {
        printf("Failed to create renderer. Error: %s\n", SDL_GetError());
        return 0;
    }

    SDL_SetRenderDrawColor(*renderer, BG_RED, BG_GREEN, BG_BlUE, BG_ALPHA);
    SDL_RenderPresent(*renderer);

    return 1;
}

uint8 update_display(SDL_Renderer **renderer, uint8 *frame_buffer)
{
    // Clear screen
    SDL_SetRenderDrawColor(*renderer, BG_RED, BG_GREEN, BG_BlUE, BG_ALPHA);
    SDL_RenderClear(*renderer);

    for (uint16 i = 0; i < (DISPLAY_WIDTH * DISPLAY_HEIGHT); ++i) {
        if (frame_buffer[i]) {
            SDL_Rect pixel;
            pixel.x = (i % DISPLAY_WIDTH) * SCALE;
            pixel.y = (i / DISPLAY_WIDTH) * SCALE;
            pixel.w = SCALE;
            pixel.h = SCALE;

            SDL_SetRenderDrawColor(*renderer, TEXT_RED, TEXT_GREEN, TEXT_BlUE, TEXT_ALPHA);
            SDL_RenderFillRect(*renderer, &pixel);
        }
    }
    SDL_RenderPresent(*renderer);

    return 1;
}

void destroy_display(SDL_Window **window, SDL_Renderer **renderer)
{
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyWindow(*window);
    SDL_Quit();
}
