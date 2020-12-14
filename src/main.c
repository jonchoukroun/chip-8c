#include <SDL2/SDL.h>
#include <string.h>

#include "input.h"
#include "output.h"
#include "cpu.h"
#include "cycles.h"
#include "test_programs.h"

uint8 handle_input(CPU *, SDL_Event);
uint8 load_program(CPU *, char *);
uint8 run_cycle(CPU *);
void decrement_timers(CPU *);

/**
 * Debugging
 **/
void load_test_program(CPU *);
void draw_fb(CPU *);
void check_state(CPU *);
void draw_keys(CPU *);
void cpu_snapshot(uint16, CPU *);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("To play a game run:\n\t$> ./chip8 roms/gamefile.c8\n");
        printf("Or, run a test program:\n\t$>./chip8 -t\n");
        return -1;
    }

    // STANDARD or EXTENDED
    KEYBOARD_TYPE t = STANDARD;
    CPU *cpu = initialize_cpu(t);
    // check_state(cpu);

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
    Cycle *frames_cycle = create_cycle(TIMER_CYCLE);

    while (running == 1) {
        reset_cycle(clock_cycle);
        reset_cycle(frames_cycle);

        while (SDL_PollEvent(&event)) {
            running = handle_input(cpu, event);
        }

        // CPU cycle
        if (run_cycle(cpu) != 1) {
            printf("Exiting early...\n");
            break;
        }

        update_cycle(clock_cycle);
        if (clock_cycle->elapsed < clock_cycle->chunk) {
            delay(clock_cycle);
        }

        update_cycle(frames_cycle);
        if (frames_cycle->chunk >= frames_cycle->elapsed) {
            if (cpu->draw_flag == 1) {
                // draw_fb(cpu);
                update_display(&renderer, cpu->frame_buffer);
                cpu->draw_flag = 0;
            }

            if (cpu->sound_timer > 0) {
                emit_audio(audio_device);
            } else {
                silence_audio(audio_device);
            }

            decrement_timers(cpu);
            reset_cycle(frames_cycle);
        }
        // draw_keys(cpu);
    }

    destroy_audio_device(audio_device);
    destroy_display(&window, &renderer);
    destroy_cycle(clock_cycle);
    destroy_cycle(frames_cycle);
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
    cpu_snapshot(opcode, cpu);
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

/**
 * Debugging
 **/
void load_test_program(CPU *cpu)
{
    const uint16 *program = draw_sprites;
    uint8 size = program[0];
    for (uint8 i = 1; i <= size; i++) {
        uint16 opcode = program[i];
        uint16 idx = PROGRAM_START + ((i - 1) * 2);
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
            } else {
                pixel = 46;
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
        printf("V[%x] = %x\n", i, cpu->V[i]);
    }

    printf("I = %x\n", cpu->I);
    printf("DT = %x\n", cpu->delay_timer);
    printf("ST = %x\n", cpu->sound_timer);

    printf("SP = %x\n", cpu->stack_pointer);
    for (int i = 0; i < STACK_SIZE; i++) {
        printf("Stack[%x] = %x\n", i, cpu->stack[i]);
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

void draw_keys(CPU *cpu)
{
    for (uint8 i = 0; i < KEYBOARD_SIZE; i++) {
        if (cpu->key_state[i] == 1) {
            printf("%x\t", i);
        } else {
            printf("_\t");
        }
    }
    printf("\n");
}

void cpu_snapshot(uint16 opcode, CPU *cpu)
{
    printf("opcode: %x, pc: %x", opcode, cpu->program_counter);
    if (cpu->stack_pointer > 0) printf(" sp: %x,", cpu->stack_pointer);
    for (int i = 0; i < STACK_SIZE; i++) {
        if (cpu->stack[i] > 0) printf(" sp[%x]: %x,", i, cpu->stack[i]);
    }
    if (cpu->delay_timer > 0) printf(" dt: %x,", cpu->delay_timer);
    for (int i = 0; i < REGISTER_COUNT; i++) {
        if (cpu->V[i] > 0) printf(" V[%x]: %x,", i, cpu->V[i]);
    }
    if (cpu->I > 0) printf(" I: %x", cpu->I);

    printf("\n");
}
