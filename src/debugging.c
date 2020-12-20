#include "debugging.h"
#include "test_programs.h"

void load_test_program(CPU *cpu)
{
    TEST_PROGRAM *program = malloc(sizeof *program);
    program->name = SET_NAME;
    program->size = SET_SIZE;
    program->opcodes = SET_OPCODES;

    for (uint16 i = 0; i < program->size; i++) {
        cpu->RAM[PROGRAM_START + i] = program->opcodes[i];
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
