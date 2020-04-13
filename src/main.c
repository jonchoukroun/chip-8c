#include <SDL2/SDL.h>

#include "input.h"
#include "output.h"
#include "cpu.h"
#include "test_programs.h"

uint8 load_program(CPU *);
uint8 handle_input(CPU *, uint8);
uint8 run_cycle(CPU *);
void decrement_timers(CPU *);

int main(/* int argc, char const *argv[] */)
{
    CPU *cpu = initialize_cpu();

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    initialize_display(&window, &renderer);

    SDL_AudioDeviceID audio_device = initialize_audio();
    if (audio_device == 0) {
        return -1;
    }

    load_program(cpu);

    // // while game is running
    uint8 running = 1;
    SDL_Event e;

    // TODO: set timer start
    Cycle *clock_cycle = create_cycle(CLOCK_CYCLE);
    Cycle *timer_cycle = create_cycle(TIMER_CYCLE);

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

        if (cpu->sound_timer > 0) {
            emit_audio(audio_device);
        } else {
            silence_audio(audio_device);
        }

        update_cycle(clock_cycle);
        if (clock_cycle->chunk > clock_cycle->elapsed) {
            delay(clock_cycle);
        }

        update_cycle(timer_cycle);
        if (timer_cycle->chunk < timer_cycle->elapsed) {
            decrement_timers(cpu);
            reset_cycle(timer_cycle);
        }
    }

    destroy_audio_device(audio_device);
    destroy_display(&window, &renderer);
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
    const uint16 *program = buzzer;
    for (uint8 i = 0; i < PROGRAM_SIZE; i++) {
        uint16 idx = 0x200 + (i * 2);
        uint16 opcode = program[i];
        cpu->RAM[idx] = (opcode & 0xff00) >> 8;
        cpu->RAM[idx + 1] = opcode & 0x00ff;
    }

    return 1;
}

void decrement_timers(CPU *cpu)
{
    if (cpu->delay_timer > 0) cpu->delay_timer--;

    if (cpu->sound_timer > 0) cpu->sound_timer--;
}
