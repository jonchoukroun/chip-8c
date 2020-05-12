#include "cycles.h"

Cycle *create_cycle(uint8 type)
{
    Cycle *cycle = malloc(sizeof(Cycle));
    if (type == CLOCK_CYCLE) {
        cycle->chunk = MS_CONVERSION / CPU_RATE;
    } else {
        cycle->chunk = MS_CONVERSION / DELAY_RATE;
    }
    cycle->type = type;
    cycle->start = 0;
    cycle->elapsed = 0;

    return cycle;
}

void reset_cycle(Cycle *cycle)
{
    // cycle->start = clock();
    cycle->start = SDL_GetTicks();
}

void update_cycle(Cycle *cycle)
{
    // clock_t now = clock();
    // double elapsed_time = ((double)(now - cycle->start) / CLOCKS_PER_SEC) * MCS_CONVERSION;
    // cycle->elapsed = elapsed_time;
    // printf("cycle %d | %f\n", cycle->type, cycle->elapsed);
    cycle->elapsed = SDL_GetTicks() - cycle->start;

}

void delay(Cycle *cycle) {
    // usleep(MCS_CLOCK_RATE - cycle->elapsed);
    SDL_Delay(cycle->chunk - cycle->elapsed);
}

void destroy_cycle(Cycle *cycle)
{
    free(cycle);
}

