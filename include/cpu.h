#ifndef CHIP_8
#define CHIP_8

#include <stdlib.h>
#include "input.h"
#include "output.h"

typedef struct CPU {
    uint16 *RAM;

    uint8 *V;
    uint16 I;

    uint8 delay_timer;
    uint8 sound_timer;

    uint16 program_counter;

    uint16 *stack;
    uint8 stack_pointer;

    uint8 *frame_buffer;
    uint8 draw_flag;

    uint8 *key_state;

    struct HashTable *key_table;
} CPU;

CPU * initialize_cpu(KEYBOARD_TYPE);

uint16 fetch_opcode(CPU *);

uint8 execute_opcode(CPU *, uint16);

void destroy_cpu(CPU *);

#endif

