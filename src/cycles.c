#include "cycles.h"

Cycle *create_cycle(uint8 type)
{
    Cycle *cycle = calloc(1, sizeof *cycle);
    if (type == CLOCK_CYCLE) {
        cycle->chunk = MS_CONVERSION / CPU_RATE;
    } else {
        cycle->chunk = MS_CONVERSION / DELAY_RATE;
    }
    cycle->type = type;
    cycle->start = SDL_GetTicks();

    return cycle;
}

void reset_cycle(Cycle *cycle)
{
    cycle->start = SDL_GetTicks();
    cycle->current = 0;
}

void update_cycle(Cycle *cycle)
{
    cycle->current = SDL_GetTicks() - cycle->start;
}

uint8 is_elapsed(Cycle *cycle)
{
    return cycle->current >= cycle->chunk;
}

void delay(Cycle *cycle) {
    SDL_Delay(CLOCK_SPEED - cycle->current);
}

void destroy_cycle(Cycle *cycle)
{
    free(cycle);
}

