#include <SDL2/SDL.h>

#include "input.h"
#include "output.h"
#include "cpu.h"
#include "test_programs.h"

uint8 load_program(CPU *);
uint8 handle_input(CPU *, uint8);
uint8 run_cycle(CPU *);
// void decrementTimers(CPU *);
// void decrementCounters(CPU *);

int main(/* int argc, char const *argv[] */)
{
    CPU *cpu = initialize_cpu();

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    initialize_display(&window, &renderer);

    load_program(cpu);

    // while game is running
    uint8 running = 1;
    SDL_Event e;
    while (running == 1) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;

            if (e.type == SDL_KEYDOWN) {
                running = handle_input(cpu, e.key.keysym.scancode);
            }
        }

        // CPU cycle
        if (run_cycle(cpu) != 1) {
            printf("Exiting early...\n");
            break;
        }

        if (cpu->draw_flag == 1) {
            update_display(&renderer, cpu->frame_buffer);
            cpu->draw_flag = 0;
        }

        // TODO: update timers

        // TODO: sleep to match elapsed time with game rate
    }

    // clean SDL
    destroy_display(&window, &renderer);
    // clean CPU
    destroy_cpu(cpu);

    return 0;
}

uint8 handle_input(CPU *cpu, uint8 key)
{
    if (key == SDL_SCANCODE_Q) return 0;

    uint8 key_value = get_key_value(cpu->key_table, key);
    if (key_value == 0xff) printf("Invalid key pressed: %x (%x)\n", key, key_value);

    cpu->key_state[key_value] = 1;

    return 1;
}

uint8 run_cycle(CPU *cpu)
{
    uint16 opcode = fetch_opcode(cpu);
    if (execute_opcode(cpu, opcode) != 1) {
        // Allow to fail silently?
        printf("Failed to execute opcode. Exiting...\n");
        return 0;
    }

    return 1;
}

uint8 load_program(struct CPU *cpu)
{
    // Test instructions, start loading at RAM[0x200]
    const uint16 *program = draw_keypress;
    for (uint8 i = 0; i < PROGRAM_SIZE; i++) {
        uint16 idx = 0x200 + (i * 2);
        uint16 opcode = program[i];
        cpu->RAM[idx] = (opcode & 0xff00) >> 8;
        cpu->RAM[idx + 1] = opcode & 0x00ff;
    }

    return 1;
}

// void decrementCounters(struct CPU *cpu)
// {
//     if (cpu->delayTimer > 0) {
//         --cpu->delayTimer;
//     }

//     if (cpu->soundTimer > 0) {
//         --cpu->soundTimer;
//     }
// }
