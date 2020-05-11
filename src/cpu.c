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
uint8 generate_random_number();

CPU * initialize_cpu()
{
    CPU *cpu = calloc(1, sizeof(CPU));

    for (uint8 i = 0; i < REGISTER_COUNT; i++) {
        cpu->V[i] = 0;
    }

    cpu->I = 0;

    for (uint8 i = 0; i < STACK_SIZE; i++) {
        cpu->stack[i] = 0;
    }
    cpu->stack_pointer = 0;

    cpu->delay_timer = 0;
    cpu->sound_timer = 0;

    clear_frame_buffer(cpu);
    cpu->draw_flag = 0;

    cpu->program_counter = 0x200;

    for (uint16 i = 0; i < RAM_SIZE; ++i) {
        if (i < FONTSET_SIZE) {
            cpu->RAM[i] = font_set[i];
        } else {
            cpu->RAM[i] = 0;
        }
    }

    for (uint8 i = 0; i < KEYBOARD_SIZE; i++) {
        cpu->key_state[i] = 0;
    }

    // Use EXTENDED or STANDARD keyboard layouts
    cpu->key_table = create_hashtable(EXTENDED);

    return cpu;
}

uint16 fetch_opcode(CPU *cpu)
{
    return cpu->RAM[cpu->program_counter] << 8 | cpu->RAM[cpu->program_counter + 1];
}

uint8 execute_opcode(CPU *cpu, uint16 opcode)
{
    // printf("will execute %x\n", opcode);

    switch ((opcode & 0xf000) >> 12) {
        case 0x0:
            switch(opcode & 0xff) {
                case 0xe0:
                    clear_frame_buffer(cpu);
                    cpu->draw_flag = 1;
                    cpu->program_counter += 2;
                    break;

                case 0xee:
                    cpu->program_counter = cpu->stack[cpu->stack_pointer];
                    cpu-> stack_pointer--;
                    break;

                default:
                    printf("Matched unused opcode: %x\n", opcode);
                    cpu->program_counter += 2;
                    return 0;
            }
            break;

        case 0x1:
            cpu->program_counter = opcode & 0xfff;
            break;

        case 0x2:
            cpu->stack_pointer++;
            cpu->stack[cpu->stack_pointer] = cpu->program_counter;
            cpu->program_counter = opcode & 0x0fff;
            break;

        case 0x3:
            if ((cpu->V[(opcode & 0x0f00) >> 8]) == (opcode & 0x00ff))
                cpu->program_counter += 2;

            cpu->program_counter += 2;
            break;

        case 0x4:
            if ((cpu->V[(opcode & 0x0f00) >> 8]) != (opcode & 0x00ff))
                cpu->program_counter += 2;

            cpu->program_counter += 2;
            break;

        case 0x5:
            if (cpu->V[(opcode & 0x0f00) >> 8] == cpu->V[(opcode & 0x00f0) >> 4])
                cpu->program_counter += 2;

            cpu->program_counter += 2;
            break;

        case 0x6:
            cpu->V[(opcode & 0x0f00) >> 8] = (opcode & 0x00ff);
            cpu->program_counter += 2;
            break;

        case 0x7:
            cpu->V[(opcode & 0x0f00) >> 8] += (opcode & 0x00ff);
            cpu->program_counter += 2;
            break;

        case 0x8:
            switch (opcode & 0x000f) {
                case 0x0:
                    cpu->V[(opcode & 0x0f00) >> 8] = cpu->V[(opcode & 0x00f0) >> 4];
                    break;

                case 0x1:
                    cpu->V[(opcode & 0x0f00) >> 8] |= cpu->V[(opcode & 0x00f0) >> 4];
                    break;

                case 0x2:
                    cpu->V[(opcode & 0x0f00) >> 8] &= cpu->V[(opcode & 0x00f0) >> 4];
                    break;

                case 0x3:
                    cpu->V[(opcode & 0x0f00) >> 8] ^= cpu->V[(opcode & 0x00f0) >> 4];
                    break;

                case 0x4:
                    cpu->V[(opcode & 0x0f00) >> 8] += cpu->V[(opcode & 0x00f0) >>4];
                    if (cpu->V[(opcode & 0x0f00) >> 8] > 0xff) {
                        cpu->V[CARRY_FLAG_ADDRESS] = 1;
                    } else {
                        cpu->V[CARRY_FLAG_ADDRESS] = 0;
                    }

                    break;

                case 0x5:
                    if (cpu->V[(opcode & 0x0f00) >> 8] > cpu->V[(opcode & 0x00f0) >> 4]) {
                        cpu->V[CARRY_FLAG_ADDRESS] = 1;
                    } else {
                        cpu->V[CARRY_FLAG_ADDRESS] = 0;
                    }
                    cpu->V[(opcode & 0x0f00) >> 8] -= cpu->V[(opcode & 0x00f0) >> 4];
                    break;

                case 0x6:
                    cpu->V[CARRY_FLAG_ADDRESS] = cpu->V[(opcode & 0x0f00) >> 8] & 1;
                    cpu->V[(opcode & 0x0f00) >> 8] >>= 1;
                    break;

                case 0x7:
                    if (cpu->V[(opcode & 0x00f0) >> 4] > cpu->V[(opcode & 0x0f00) >> 8]) {
                        cpu->V[CARRY_FLAG_ADDRESS] = 1;
                    } else {
                        cpu->V[CARRY_FLAG_ADDRESS] = 0;
                    }
                    cpu->V[(opcode & 0x0f00) >> 8] = cpu->V[(opcode & 0x00f0) >> 4] - cpu->V[(opcode & 0x0f00) >> 8];
                    break;

                case 0xe:
                    cpu->V[CARRY_FLAG_ADDRESS] = cpu->V[(opcode & 0x0f00) >> 8] & 1 << (sizeof(uint8) - 1);
                    cpu->V[(opcode & 0x0f00) >> 8] <<= 1;
                    break;

                default:
                    printf("Could not match opcode %x\n", opcode);
                    return 0;
            }
            cpu->program_counter += 2;
            break;

        case 0x9:
            if ((opcode & 0x000f) != 0) {
                printf("Coult not match opcode: %x\n", opcode);
                return 0;
            }

            if (cpu->V[(opcode & 0x0f00) >> 8] != cpu->V[(opcode & 0x00f0) >> 4])
                cpu->program_counter += 2;

            cpu->program_counter += 2;
            break;

        case 0xa:
            cpu->I = opcode & 0x0fff;
            cpu->program_counter += 2;
            break;

        case 0xb:
            cpu->program_counter = cpu->V[0x0] + (opcode & 0x0fff);
            break;

        case 0xc: {
            uint8 num = generate_random_number();
            cpu->V[(opcode & 0x0f00) >> 8] = num & (opcode & 0x00ff);
            cpu->program_counter += 2;
            break;
        }

        case 0xd: {
            uint8 height = opcode & 0x000f;
            uint16 pixel;
            cpu->V[CARRY_FLAG_ADDRESS] = 0;

            for (uint8 row = 0; row < height; row++) {
                pixel = cpu->RAM[cpu->I + row];
                uint8 y_pos = (cpu->V[(opcode & 0x00f0) >> 4] + row) % DISPLAY_HEIGHT;
                for (uint8 col = 0; col < PIXEL_WIDTH; col++) {
                    uint8 x_pos = (cpu->V[(opcode & 0x0f00) >> 8] + col) % DISPLAY_WIDTH;

                    if (pixel & (0x80 >> col)) {
                        uint16 frame = x_pos + (y_pos * DISPLAY_WIDTH);
                        if (cpu->frame_buffer[frame])
                            cpu->V[CARRY_FLAG_ADDRESS] = 1;

                        cpu->frame_buffer[frame] ^= 1;
                    }
                }
            }
            cpu->draw_flag = 1;
            cpu->program_counter += 2;
            break;
        }

        case 0xe:
            switch (opcode & 0x00ff) {
                case 0x9e:
                    if (cpu->key_state[cpu->V[(opcode & 0x0f00) >> 8]] != 0)
                        cpu->program_counter += 2;
                    break;

                case 0xa1:
                    if (cpu->key_state[cpu->V[(opcode & 0x0f00) >> 8]] == 0)
                        cpu->program_counter += 2;
                    break;

                default:
                    printf("Could not match opcode %x\n", opcode);
                    return 0;
            }
            cpu->program_counter += 2;
            break;

        case 0xf:
            switch (opcode & 0x00ff) {
                case 0x07:
                    cpu->V[(opcode & 0x0f00) >> 8] = cpu->delay_timer;
                    break;

                case 0x0a: {
                    SDL_Event event;
                    uint8 waiting = 1;
                    while(waiting) {
                        SDL_WaitEvent(&event);
                        if (event.type == SDL_KEYDOWN) {
                            uint8 key = event.key.keysym.scancode;
                            // Handle quit
                            if (key == SDL_SCANCODE_Q) return 0;

                            uint8 key_value = get_key_value(cpu->key_table, key);
                            if (key_value != 0xff) {
                                cpu->V[(opcode & 0x0f00) >> 8] = key_value;
                                waiting = 0;
                            } else {
                                printf("Invalid keypress %x\n", key);
                            }
                        }
                    }
                    break;
                }

                case 0x15:
                    cpu->delay_timer = cpu->V[(opcode & 0x0f00) >> 8];
                    break;

                case 0x18:
                    cpu->sound_timer = cpu->V[(opcode & 0x0f00) >> 8];
                    break;

                case 0x1e:
                    cpu->I += cpu->V[(opcode & 0x0f00) >> 8];
                    break;

                case 0x29:
                    cpu->I = cpu->V[(opcode & 0x0f00) >> 8] * 5;
                    break;

                case 0x33: {
                    uint8 decimal = cpu->V[(opcode & 0x0f00) >> 8];
                    uint8 sig = 100;

                    while (decimal > 0) {
                        cpu->RAM[cpu->I] = decimal / sig;
                        decimal %= sig;
                        sig /= 10;
                        cpu->I++;
                    }
                    break;
                }

                case 0x55:
                    for (uint8 i = 0x0; i <= (opcode & 0x0f00) >> 8; ++i) {
                        cpu->RAM[cpu->I] = cpu->V[i];
                        cpu->I++;
                    }
                    break;

                case 0x65:
                    for (uint8 i = 0x0; i <= (opcode & 0x0f00) >> 8; ++i) {
                        cpu->V[i] = cpu->RAM[cpu->I];
                        cpu->I++;
                    }
                    break;

                default:
                    printf("Could not match opcode %x\n", opcode);
                    return 0;
            }
            cpu->program_counter += 2;
            break;

        default:
            printf("Cannont match opcode: %x\n", opcode);
            return 0;
    }

    return 1;
}

void clear_frame_buffer(CPU *cpu)
{
    for (uint16 i = 0; i < (DISPLAY_SIZE); ++i) {
        cpu->frame_buffer[i] = 0;
    }
}

uint8 generate_random_number()
{
    FILE *random_file = fopen("/dev/urandom", "r");
    if (random_file == NULL) {
        printf("Failed to open /dev/urandom\n");
        return -1;
    } else {
        uint8 random_data[2];
        ssize_t result = fread(random_data, sizeof(uint8), (sizeof(random_data) / sizeof(uint8)), random_file);
        if (result < 0) {
            printf("Cannot read random data\n");
        } else {
            return random_data[0];
        }
    }

    return -1;
}

// Cycle management
Cycle *create_cycle(uint8 type)
{
    Cycle *cycle = malloc(sizeof(Cycle));
    if (type == CLOCK_CYCLE) {
        cycle->chunk = MCS_CONVERSION / CPU_RATE;
    } else {
        cycle->chunk = MCS_CONVERSION / DELAY_RATE;
    }
    cycle->type = type;
    cycle->start = clock();
    cycle->elapsed = 0;

    return cycle;
}

void update_cycle(Cycle *cycle)
{
    clock_t now = clock();
    double elapsed_time = ((double)(now - cycle->start) / CLOCKS_PER_SEC) * MCS_CONVERSION;
    cycle->elapsed = elapsed_time;
}

void delay(Cycle *cycle) {
    usleep(MCS_CLOCK_RATE - cycle->elapsed);
    reset_cycle(cycle);
}

void reset_cycle(Cycle *cycle)
{
    cycle->start = clock();
    cycle->elapsed = 0;
}

void destroy_cpu(CPU *cpu) {
    free(cpu);
}
