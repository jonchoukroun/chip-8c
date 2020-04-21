#include <SDL2/SDL.h>
#include <string.h>

#include "input.h"
#include "output.h"
#include "cpu.h"
#include "test_programs.h"

uint8 load_program(CPU *, char *);
uint8 handle_input(CPU *, uint8);
uint8 run_cycle(CPU *);
void decrement_timers(CPU *);

// debugging
void load_test_program(CPU *);
void draw_fb(CPU *);
void check_state(CPU *);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("To play a game run:\n\t$> ./chip8 roms/gamefile.c8\n");
        printf("Or, run a test program:\n\t$>./chip8 -t\n");
        return -1;
    }

    CPU *cpu = initialize_cpu();
    // check_state(cpu);

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    initialize_display(&window, &renderer);

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
            printf("Will delay\n");
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

uint8 load_program(CPU *cpu, char *filename)
{
    FILE *program;
    program = fopen(filename, "rb");
    if (!program) {
        printf("Failed to open ROM %s\n", filename);
        return 0;
    }

    int byte;
    int i = 0x200;
    while ((byte = getc(program)) != EOF) {
        cpu->RAM[i] = byte;
        // printf("stored: %x %x\n", i, cpu->RAM[i]);
        i++;
    }
    fclose(program);

    return 1;
}

void decrement_timers(CPU *cpu)
{
    if (cpu->delay_timer > 0) cpu->delay_timer--;

    if (cpu->sound_timer > 0) cpu->sound_timer--;
}

/**
 * Debugging
 **/
void load_test_program(CPU *cpu)
{
    const uint16 *program = draw_sprites;
    uint8 size = program[0];
    for (uint8 i = 1; i <= size; i++) {
        uint16 opcode = program[i];
        uint16 idx = 0x200 + ((i - 1) * 2);
        cpu->RAM[idx] = (opcode & 0xff00) >> 8;
        cpu->RAM[idx + 1] = opcode & 0x00ff;
    }
}

void draw_fb(CPU *cpu) {
    for (uint16 row = 0; row < DISPLAY_HEIGHT; row++) {
        for (uint16 col = 0; col < DISPLAY_WIDTH; col++) {
            uint8 pixel = 32;
            if (cpu->frame_buffer[col + (row * DISPLAY_WIDTH)] == 1) {
                pixel = 35;
            }
            printf("%c", pixel);
        }
        printf("\n");
    }
    printf("\n");
}

void check_state(CPU *cpu)
{
    for (int i = 0; i < REGISTER_COUNT; i++) {
        printf("V[%d] = %x\n", i, cpu->V[i]);
    }
    printf("V[%x] = %x\n", CARRY_FLAG_ADDRESS, cpu->V[CARRY_FLAG_ADDRESS]);

    printf("I = %x\n", cpu->I);
    printf("DT = %x\n", cpu->delay_timer);
    printf("ST = %x\n", cpu->sound_timer);

    printf("SP = %x\n", cpu->stack_pointer);
    for (int i = 0; i < STACK_SIZE; i++) {
        printf("Stack[%d] = %x\n", i, cpu->stack[i]);
    }

    int count = 0;
    for (int i = 0; i < (DISPLAY_SIZE); i++) {
        if (cpu->frame_buffer[i] == 1) {
            printf("on @ %x\n", i);
            count++;
        }
    }
    printf("pixels on = %d\n", count);

}
