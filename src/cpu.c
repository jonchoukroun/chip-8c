#include <SDL2/SDL.h>
#include "cpu.h"

// Input
uint8 font_set[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0x10, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // a
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // b
    0xF0, 0x80, 0x80, 0x80, 0xF0, // c
    0xE0, 0x90, 0x90, 0x90, 0xE0, // d
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // e
    0xF0, 0x80, 0xF0, 0x80, 0x80  // f
};

void clear_frame_buffer(CPU *);
uint8 generate_random_number(void);

CPU * initialize_cpu(KEYBOARD_TYPE keyboard)
{
    CPU *cpu = calloc(1, sizeof *cpu);

    uint16 *ram = calloc(RAM_SIZE, sizeof ram);
    cpu->RAM = ram;

    uint8 *registers = calloc(REGISTER_COUNT, sizeof *registers);
    cpu->V = registers;

    cpu->I = 0;

    cpu->sound_timer = 0;
    cpu->delay_timer = 0;

    cpu->program_counter = PROGRAM_START;

    uint16 *stack = calloc(STACK_SIZE, sizeof *stack);
    cpu->stack = stack;
    cpu->stack_pointer = 0;

    uint8 *frame_buffer = calloc(DISPLAY_SIZE, sizeof *frame_buffer);
    cpu->frame_buffer = frame_buffer;
    cpu->draw_flag = 0;

    for (uint16 i = 0; i < FONTSET_SIZE; i++) {
        cpu->RAM[i] = font_set[i];
    }

    uint8 *key_state = calloc(KEYBOARD_SIZE, sizeof *key_state);
    cpu->key_state = key_state;

    cpu->key_table = create_hashtable(keyboard);

    return cpu;
}

uint16 fetch_opcode(CPU *cpu)
{
    return cpu->RAM[cpu->program_counter] << 8 | cpu->RAM[cpu->program_counter + 1];
}

uint8 execute_opcode(CPU *cpu, uint16 opcode)
{
    cpu->program_counter += 2;

    uint8 x = (opcode & 0x0f00) >> 8;
    uint8 y = (opcode & 0x00f0) >> 4;
    uint8 byte = opcode & 0x00ff;
    uint16 addr = opcode & 0x0fff;
    uint8 nibble = opcode & 0x000f;

    switch ((opcode & 0xf000) >> 12) {
        case 0x0:
            switch(byte) {
                case 0xe0:
                    clear_frame_buffer(cpu);
                    cpu->draw_flag = 1;
                    break;

                case 0xee:
                    if (cpu->stack_pointer == 0x0) {
                        printf("Fatal error: Stack underflow.\nOpcode: %x\n", opcode);
                        return 0;
                    }
                    cpu->stack_pointer--;
                    break;

                default:
                    printf("Matched unused opcode: %x\n", opcode);
                    return 0;
            }
            break;

        case 0x1:
            break;

        case 0x2:
            if (cpu->stack_pointer == STACK_SIZE) {
                printf("Fatal error: stack overflow.\nOpcode: %x\n", opcode);
                return 0;
            }
            cpu->stack_pointer++;
            break;

        case 0x3:
            if ((cpu->V[x]) == byte) {
            } else {
            }
            break;

        case 0x4:
            if ((cpu->V[x]) != byte) {
            } else {
            }
            break;

        case 0x5:
            if (cpu->V[x] == cpu->V[y]) {
            } else {
            }
            break;

        case 0x6:
            cpu->V[x] = byte;
            break;

        case 0x7:
            cpu->V[x] += byte;
            break;

        case 0x8:
            switch (nibble) {
                case 0x0:
                    cpu->V[x] = cpu->V[y];
                    break;

                case 0x1:
                    cpu->V[x] |= cpu->V[y];
                    break;

                case 0x2:
                    cpu->V[x] &= cpu->V[y];
                    break;

                case 0x3:
                    cpu->V[x] ^= cpu->V[y];
                    break;

                case 0x4: {
                    uint16 result = cpu->V[x] + cpu->V[y];
                    cpu->V[CARRY_FLAG_ADDRESS] = result > 0xff ? 1 : 0;
                    cpu->V[x] = result & 0xff;
                    break;
                }

                case 0x5:
                    cpu->V[CARRY_FLAG_ADDRESS] = (cpu->V[x] > cpu->V[y]) ? 1 : 0;
                    cpu->V[x] -= cpu->V[y];
                    break;

                case 0x6:
                    cpu->V[CARRY_FLAG_ADDRESS] = cpu->V[x] & 1;
                    cpu->V[x] >>= 1;
                    break;

                case 0x7:
                    cpu->V[CARRY_FLAG_ADDRESS] = (cpu->V[y] > cpu->V[x]) ? 1 : 0;
                    cpu->V[x] = cpu->V[y] - cpu->V[x];
                    break;

                case 0xe:
                    cpu->V[CARRY_FLAG_ADDRESS] = (cpu->V[x] & 0x80) ? 1 : 0;
                    cpu->V[x] <<= 1;
                    break;

                default:
                    printf("Could not match opcode %x\n", opcode);
                    return 0;
            }
            break;

        case 0x9:
            if (nibble != 0) {
                printf("Coult not match opcode: %x\n", opcode);
                return 0;
            }

            if (cpu->V[x] != cpu->V[y]) {
            } else {
            }
            break;

        case 0xa:
            cpu->I = addr;
            break;

        case 0xb:
            break;

        case 0xc: {
            uint8 num = generate_random_number();
            cpu->V[x] = (num & byte);
            break;
        }

        case 0xd: {
            uint8 height = nibble;
            uint16 pixel;
            cpu->V[CARRY_FLAG_ADDRESS] = 0;

            for (uint8 row = 0; row < height; row++) {
                pixel = cpu->RAM[cpu->I + row];
                for (uint8 col = 0; col < PIXEL_WIDTH; col++) {
                    if (pixel & (0x80 >> col)) {
                        uint8 x_pos = (cpu->V[x] + col) % DISPLAY_WIDTH;
                        uint8 y_pos = (cpu->V[y] + row) % DISPLAY_HEIGHT;
                        uint16 frame = x_pos + (y_pos * DISPLAY_WIDTH);
                        if (cpu->frame_buffer[frame])
                            cpu->V[CARRY_FLAG_ADDRESS] = 1;

                        cpu->frame_buffer[frame] ^= 1;
                    }
                }
            }
            cpu->draw_flag = 1;
            break;
        }

        case 0xe:
            switch (byte) {
                case 0x9e:
                    if (cpu->key_state[cpu->V[x]] == 1) {
                    } else {
                    }
                    break;

                case 0xa1:
                    if (cpu->key_state[cpu->V[x]] == 0) {
                    } else {
                    }
                    break;

                default:
                    printf("Could not match opcode %x\n", opcode);
                    return 0;
            }
            break;

        case 0xf:
            switch (byte) {
                case 0x07:
                    cpu->V[x] = cpu->delay_timer;
                    break;

                case 0x0a: {
                    uint8 is_pressed = 0;
                    for (uint8 i = 0; i < KEYBOARD_SIZE; i++) {
                        if (cpu->key_state[i] == 1) {
                            cpu->V[x] = i;
                            is_pressed = 1;
                        }
                    }
                    if (is_pressed == 0) return 1;

                    break;
                }

                case 0x15:
                    cpu->delay_timer = cpu->V[x];
                    break;

                case 0x18:
                    cpu->sound_timer = cpu->V[x];
                    break;

                case 0x1e:
                    cpu->I += cpu->V[x];
                    break;

                case 0x29:
                    cpu->I = cpu->V[x] * 5;
                    break;

                case 0x33: {
                    uint8 decimal = cpu->V[x];

                    cpu->RAM[cpu->I] = decimal / 100;
                    cpu->RAM[cpu->I + 1] = (decimal % 100) / 10;
                    cpu->RAM[cpu->I + 2] = decimal % 10;

                    break;
                }

                case 0x55:
                    for (uint8 i = 0; i <= x; i++) {
                        cpu->RAM[cpu->I + i] = cpu->V[i];
                    }
                    cpu->I += x + 1;
                    break;

                case 0x65:
                    for (uint8 i = 0; i <= x; i++) {
                        cpu->V[i] = cpu->RAM[cpu->I + i];
                    }
                    cpu->I += x + 1;
                    break;

                default:
                    printf("Could not match opcode %x\n", opcode);
                    return 0;
            }
            break;

        default:
            printf("Cannont match opcode: %x\n", opcode);
            return 0;
    }

    return 1;
}

void clear_frame_buffer(CPU *cpu)
{
    for (uint16 i = 0; i < DISPLAY_SIZE; i++) {
        cpu->frame_buffer[i] = 0;
    }
}

/**
 * Uses preferred arc4random to generate a random number
 **/
uint8 generate_random_number()
{
    return (uint8)arc4random_uniform(RAND_UPPER_BOUND);
}

void destroy_cpu(CPU *cpu) {
    destroy_hashtable(cpu->key_table);
    free(cpu);
}
