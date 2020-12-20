#include <SDL2/SDL.h>
#include <string.h>

#include "input.h"
#include "output.h"
#include "cpu.h"
#include "cycles.h"
#include "debugging.h"

uint8 handle_input(CPU *, SDL_Event);
uint8 load_program(CPU *, char *);
uint8 run_cycle(CPU *);
void decrement_timers(CPU *);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("To play a game run:\n\t$> ./chip8 roms/gamefile.c8\n");
        printf("Or, run a test program:\n\t$>./chip8 -t\n");
        return -1;
    }

    // STANDARD or EXTENDED
    KEYBOARD_TYPE t = EXTENDED;
    CPU *cpu = initialize_cpu(t);

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    initialize_display(&window, &renderer);
    update_display(&renderer, cpu->frame_buffer);

    SDL_AudioDeviceID audio_device = initialize_audio();
    if (audio_device == 0) {
        return -1;
    }

    if (strcmp(argv[1], "-t") == 0) {
        load_test_program(cpu);
    } else {
        if (load_program(cpu, argv[1]) == 0) {
            return -1;
        }
    }

    // while game is running
    uint8 running = 1;
    SDL_Event event;

    Cycle *clock_cycle = create_cycle(CLOCK_CYCLE);
    Cycle *frame_cycle = create_cycle(TIMER_CYCLE);

    while (running == 1) {
        reset_cycle(clock_cycle);

        while (SDL_PollEvent(&event)) {
            running = handle_input(cpu, event);
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

        update_cycle(frame_cycle);
        if (is_elapsed(frame_cycle) == 1) {
            if (cpu->sound_timer > 0) {
                emit_audio(audio_device);
            } else {
                silence_audio(audio_device);
            }

            decrement_timers(cpu);
            reset_cycle(frame_cycle);
        }

        if (is_elapsed(clock_cycle) == 0) {
            delay(clock_cycle);
        }
    }

    destroy_audio_device(audio_device);
    destroy_display(&window, &renderer);
    destroy_cycle(clock_cycle);
    destroy_cycle(frame_cycle);
    destroy_cpu(cpu);

    return 0;
}

uint8 handle_input(CPU *cpu, SDL_Event event)
{
    if (event.type == SDL_QUIT) return 0;

    // Ignore non key presses
    if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP) return 1;

    uint8 key = event.key.keysym.scancode;
    if (key == SDL_SCANCODE_Q) return 0;

    uint8 value = get_key_value(cpu->key_table, key);
    // Warn but don't fail
    if (value == 0xff) {
        printf("Invalid keypress %x\n", value);
        return 1;
    }

    cpu->key_state[value] = event.type == SDL_KEYDOWN ? 1 : 0;

    return 1;
}

uint8 run_cycle(CPU *cpu)
{
    uint16 opcode = fetch_opcode(cpu);
    if (execute_opcode(cpu, opcode) != 1) {
        // Allow to fail silently?
        printf("Failed to execute opcode %x. Exiting...\n", opcode);
        return 0;
    }

    return 1;
}

uint8 load_program(CPU *cpu, char *filename)
{
    FILE *program = fopen(filename, "rb");
    if (!program) {
        printf("Failed to open ROM %s\n", filename);
        return 0;
    }
    fseek(program, 0, SEEK_END);
    long program_size = ftell(program);
    rewind(program);

    if (RAM_SIZE - PROGRAM_START < program_size) {
        printf("ROM is too large too fit in memory\n");
        return 0;
    }

    char *program_buffer = (char *)malloc(sizeof(char) * program_size);
    if (!program_buffer) {
        printf("Failed to allocate program buffer\n");
        return 0;
    }
    size_t read_result = fread(program_buffer, sizeof(char), (size_t)program_size, program);
    if (read_result != (size_t)program_size) {
        printf("Failed to read ROM\n");
        return 0;
    }

    for (int i = 0; i < program_size; i++) {
        cpu->RAM[i + PROGRAM_START] = (uint8)program_buffer[i];
    }

    fclose(program);
    free(program_buffer);

    return 1;
}

void decrement_timers(CPU *cpu)
{
    if (cpu->delay_timer > 0) cpu->delay_timer--;

    if (cpu->sound_timer > 0) cpu->sound_timer--;
}
