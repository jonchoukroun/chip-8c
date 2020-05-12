#ifndef CyclesManagement
#define CyclesManagement

#include <stdlib.h>
#include <SDL2/SDL.h>
#include "types.h"
#include "constants.h"

enum CycleType {
    CLOCK_CYCLE,
    TIMER_CYCLE
};

typedef struct Cycle {
    uint8 type;
    uint32_t chunk;
    uint32_t start;
    uint32_t elapsed;
} Cycle;

Cycle * create_cycle(uint8);

void update_cycle(Cycle *);

void delay(Cycle *);

void reset_cycle(Cycle *);

void destroy_cycle(Cycle *);

#endif
