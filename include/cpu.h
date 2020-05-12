#ifndef CHIP_8
#define CHIP_8

#include <stdlib.h>
#include "input.h"
#include "output.h"

typedef struct CPU {
    uint8 V[REGISTER_COUNT];
    uint16 I;

    uint16 stack[STACK_SIZE];
    uint8 stack_pointer;

    uint8 delay_timer;
    uint8 sound_timer;

    uint8 frame_buffer[DISPLAY_SIZE];
    uint8 draw_flag;

    uint16 program_counter;

    uint8 RAM[RAM_SIZE];

    uint8 key_state[KEYBOARD_SIZE];

    struct HashTable *key_table;
} CPU;

CPU * initialize_cpu();

uint16 fetch_opcode(CPU *);

uint8 execute_opcode(CPU *, uint16);

void destroy_cpu(CPU *);

#endif

