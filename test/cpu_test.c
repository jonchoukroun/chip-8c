#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "cpu.h"

CPU *cpu = NULL;

int count_pixels(uint8 *);
uint8 get_X(uint16 opcode);
uint8 get_Y(uint16 opcode);
uint8 get_byte(uint16 opcode);
uint16 get_addr(uint16 opcode);

void test_initialize_cpu(void)
{
    uint16 count = 0;
    for (uint8 i = 0; i < REGISTER_COUNT; i++) {
        count += cpu->V[i];
    }
    CU_ASSERT_EQUAL(count, 0);

    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START);

    CU_ASSERT_EQUAL(cpu->I, 0);

    count = 0;
    for (uint8 i = 0; i < STACK_SIZE; i++) {
        count += cpu->stack[i];
    }
    CU_ASSERT_EQUAL(count, 0);

    CU_ASSERT_EQUAL(cpu->stack_pointer, 0);

    CU_ASSERT_EQUAL(cpu->delay_timer, 0);

    CU_ASSERT_EQUAL(cpu->sound_timer, 0);

    CU_ASSERT_EQUAL(count_pixels(cpu->frame_buffer), 0);

    CU_ASSERT_EQUAL(cpu->draw_flag, 0);

    count = 0;
    for (uint8 i = 0; i < FONTSET_SIZE; i++) {
        if (cpu->RAM[i] != 0) count += 1;
    }
    CU_ASSERT_EQUAL(count, FONTSET_SIZE);

    count = 0;
    for (uint16 i = FONTSET_SIZE; i < RAM_SIZE; i++) {
        count += cpu->RAM[i];
    }
    CU_ASSERT_EQUAL(count, 0);

    count = 0;
    for (uint8 i = 0; i < KEYBOARD_SIZE; i++) {
        count += cpu->key_state[i];
    }

    CU_ASSERT_EQUAL(count, 0);
}

void test_fetch_opcode(void)
{
    cpu->RAM[cpu->program_counter] = 0xAB;
    cpu->RAM[cpu->program_counter + 1] = 0x12;
    CU_ASSERT_EQUAL(fetch_opcode(cpu), 0xAB12);
}

void test_CLS(void)
{
    uint16 opcode = 0x00e0;
    cpu->program_counter = PROGRAM_START;

    for (uint16 i = 0; i < DISPLAY_SIZE; i++) {
        if (i % 10 == 0) cpu->frame_buffer[i] = 1;
    }
    CU_ASSERT_NOT_EQUAL(count_pixels(cpu->frame_buffer), 0);

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(count_pixels(cpu->frame_buffer), 0);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_RET(void)
{
    uint16 opcode = 0x00ee;

    uint8 SP = 0x2;
    uint16 val = 0xbaba;
    cpu->stack_pointer = SP;
    cpu->stack[cpu->stack_pointer] = val;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->stack_pointer, SP - 1);
    CU_ASSERT_EQUAL(cpu->program_counter, val);
}

void test_JP(void)
{
    uint16 opcode = 0x1abc;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, 0xabc);
}

void test_CALL(void)
{
    uint16 opcode = 0x2def;
    uint8 SP = 0x0;
    uint16 PC = 0xabc;
    uint16 start_address = 0x123;
    uint16 next_address = get_addr(opcode);

    cpu->program_counter = PC;
    cpu->stack_pointer = SP;
    cpu->stack[cpu->stack_pointer] = start_address;

    CU_ASSERT_EQUAL(cpu->stack_pointer, 0);
    CU_ASSERT_EQUAL(cpu->stack[cpu->stack_pointer], start_address);
    CU_ASSERT_EQUAL(cpu->program_counter, PC);

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->stack_pointer, 1);
    CU_ASSERT_EQUAL(cpu->stack[cpu->stack_pointer], PC);
    CU_ASSERT_EQUAL(cpu->program_counter, next_address);
}

void test_SE(void)
{
    uint16 opcode = 0x30ab;
    uint8 x = get_X(opcode);
    uint8 kk = get_byte(opcode);
    uint16 PC = cpu->program_counter;

    cpu->V[x] = kk;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, PC + 4);

    cpu->program_counter = PC;
    cpu->V[x] = kk + 1;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, PC + 2);
}

void test_SNE(void)
{
    uint16 opcode = 0x41cd;
    uint8 x = get_X(opcode);
    uint8 kk = get_byte(opcode);
    uint16 PC = cpu->program_counter;

    cpu->V[x] = kk;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, PC + 2);

    cpu->program_counter = PC;
    cpu->V[x] = kk + 1;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, PC + 4);
}

void test_SExy(void)
{
    uint16 opcode = 0x5230;
    cpu->program_counter = PROGRAM_START;
    uint8 x = get_X(opcode);
    uint8 y = get_Y(opcode);

    uint8 test_value = 0xa;

    cpu->V[x] = test_value;
    cpu->V[y] = test_value;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 4);

    cpu->program_counter = PROGRAM_START;
    cpu->V[y] = test_value + 1;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_LD(void)
{
    uint16 opcode = 0x6511;
    uint8 x = get_X(opcode);
    uint8 kk = get_byte(opcode);

    cpu->program_counter = PROGRAM_START;
    cpu->V[x] = 0;
    execute_opcode(cpu, opcode);

    CU_ASSERT_EQUAL(cpu->V[x], kk);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_ADD(void)
{
    uint16 opcode = 0x7005;
    uint8 x = get_X(opcode);
    uint8 kk = get_byte(opcode);

    cpu->program_counter = PROGRAM_START;
    cpu->V[x] = 5;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], 0xa);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);

    cpu->program_counter = PROGRAM_START;
    cpu->V[x] = 0xff;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], 0x4);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_LDxy(void)
{
    uint16 opcode = 0x8530;
    uint8 x = get_X(opcode);
    uint8 y = get_Y(opcode);

    uint8 start_value = 0xa;
    uint8 set_value = 0xb;
    cpu->program_counter = PROGRAM_START;
    cpu->V[x] = start_value;
    cpu->V[y] = set_value;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], set_value);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_OR(void)
{
    uint16 opcode = 0x8ab1;
    uint8 x = get_X(opcode);
    uint8 y = get_Y(opcode);

    uint8 x_value = 0x12;
    uint8 y_value = 0x32;
    cpu->V[x] = x_value;
    cpu->V[y] = y_value;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], (x_value | y_value));
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_AND(void)
{
    uint16 opcode = 0x80e2;
    uint8 x = get_X(opcode);
    uint8 y = get_Y(opcode);

    uint8 x_value = 0x4a;
    uint8 y_value = 0xdd;
    cpu->V[x] = x_value;
    cpu->V[y] = y_value;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], (x_value & y_value));
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_XOR(void)
{
    uint16 opcode = 0x8173;
    uint8 x = get_X(opcode);
    uint8 y = get_Y(opcode);

    uint8 x_value = 0x2;
    uint8 y_value = 0xcd;
    cpu->V[x] = x_value;
    cpu->V[y] = y_value;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], (x_value ^ y_value));
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_ANDxy(void)
{
    uint16 opcode = 0x84b4;
    uint8 x = get_X(opcode);
    uint8 y = get_Y(opcode);

    uint8 x_value = 0x33;
    uint8 y_value = 0xee;
    cpu->V[x] = x_value;
    cpu->V[y] = y_value;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], (x_value + y_value) & 0x00ff);
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], 1);

    x_value = 0x1;
    y_value = 0x2;
    cpu->V[x] = x_value;
    cpu->V[y] = y_value;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], (x_value + y_value) & 0x00ff);
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], 0);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_SUB(void)
{
    uint16 opcode = 0x8415;
    uint8 x = get_X(opcode);
    uint8 y = get_Y(opcode);

    uint8 x_value = 0x5;
    uint8 y_value = 0x4;
    cpu->V[x] = x_value;
    cpu->V[y] = y_value;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], (x_value - y_value) & 0xff);
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], 1);

    x_value = 0x1;
    y_value = 0xa;
    cpu->V[x] = x_value;
    cpu->V[y] = y_value;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], (x_value - y_value) & 0xff);
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], 0);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_SHR(void)
{
    uint16 opcode = 0x8846;
    uint8 x = get_X(opcode);

    uint8 x_value = 0x11;
    cpu->V[x] = x_value;
    cpu->program_counter = PROGRAM_START;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], 0x8);
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], 0x1);

    x_value = 0x8;
    cpu->V[x] = x_value;
    cpu->program_counter = PROGRAM_START;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], 0x4);
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], 0x0);

    x_value = 0x0;
    cpu->V[x] = x_value;
    cpu->program_counter = PROGRAM_START;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], 0x0);
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], (x_value & 0x0));
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_SUBN(void)
{
    uint16 opcode = 0x8127;
    uint8 x = get_X(opcode);
    uint8 y = get_Y(opcode);

    uint8 x_value = 0x5;
    uint8 y_value = 0x4;
    cpu->V[x] = x_value;
    cpu->V[y] = y_value;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], (y_value - x_value) & 0xff);
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], 0);

    x_value = 0x1;
    y_value = 0xa;
    cpu->V[x] = x_value;
    cpu->V[y] = y_value;
    cpu->program_counter = PROGRAM_START;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], (y_value - x_value) & 0xff);
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], 1);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_SHL(void)
{
    uint16 opcode = 0x800e;
    uint8 x = get_X(opcode);

    uint8 x_value = 0x80;
    cpu->V[x] = x_value;
    cpu->program_counter = PROGRAM_START;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], 0x0)
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], 0x1);

    x_value = 0x81;
    cpu->V[x] = x_value;
    cpu->program_counter = PROGRAM_START;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], 0x2)
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], 0x1);

    x_value = 0x7e;
    cpu->V[x] = x_value;
    cpu->program_counter = PROGRAM_START;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], 0xfc)
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], 0x0);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_SNExy(void)
{
    uint16 opcode = 0x9230;
    uint8 x = get_X(opcode);
    uint8 y = get_Y(opcode);

    uint8 x_value = 0x5;
    cpu->V[x] = x_value;
    cpu->V[y] = x_value;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);

    cpu->program_counter = PROGRAM_START;
    uint8 y_value = 0xb;
    cpu->V[y] = y_value;
    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 4);
}

void test_LDI(void)
{
    uint16 opcode = 0xaabc;
    uint16 addr = 0xabc;
    cpu->I = 0x123;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->I, addr);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_JPV0(void)
{
    uint16 opcode = 0xb123;
    uint16 addr = get_addr(opcode);
    cpu->program_counter = PROGRAM_START;
    cpu->V[0x0] = 0xa;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, (addr + cpu->V[0x0]));
}

// TODO: RND instruction - mock random number

void test_DRW(void)
{
    uint16 opcode = 0xd5a5;
    uint8 x = get_X(opcode);
    uint8 y = get_Y(opcode);
    uint8 height = opcode & 0x000f;
    uint8 x_value = 0xc;
    uint8 y_value = 0x3;
    uint8 sprite = 0xa;

    for (uint16 i = 0; i < DISPLAY_SIZE; i++) {
        cpu->frame_buffer[i] = 0x0;
    }
    cpu->V[x] = x_value;
    cpu->V[y] = y_value;
    cpu->I = sprite * height;
    cpu->program_counter = PROGRAM_START;

    // Flip bits to test mask & collision flag
    cpu->frame_buffer[x_value + 2 + (y_value * DISPLAY_WIDTH)] = 0b1;
    cpu->frame_buffer[x_value + 3 + (y_value * DISPLAY_WIDTH)] = 0b1;
    cpu->V[CARRY_FLAG_ADDRESS] = 0x0;

    execute_opcode(cpu, opcode);
    for (uint8 i = 0; i < DISPLAY_HEIGHT; i++) {
        for (uint8 j = 0; j < DISPLAY_WIDTH; j++) {
            if ((i < cpu->V[y] && j < cpu->V[x]) ||
                (i > cpu->V[y] + height && j > cpu->V[x] + 4)) {
                    CU_ASSERT_EQUAL(cpu->frame_buffer[j + (i * DISPLAY_WIDTH)], 0x0);
            }
        }
    }

    // Collision
    CU_ASSERT_EQUAL(cpu->V[CARRY_FLAG_ADDRESS], 0x1);
    // Expect: |**  |
    CU_ASSERT_EQUAL(cpu->frame_buffer[x_value + (y_value * DISPLAY_WIDTH)], 0x1);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 1) + (y_value * DISPLAY_WIDTH)], 0x1);
    // Flipped bits
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 2) + (y_value * DISPLAY_WIDTH)], 0x0);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 3) + (y_value * DISPLAY_WIDTH)], 0x0);

    // Expect: |*  *|
    CU_ASSERT_EQUAL(cpu->frame_buffer[x_value + ((y_value + 1) * DISPLAY_WIDTH)], 0x1);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 1) + ((y_value + 1) * DISPLAY_WIDTH)], 0x0);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 2) + ((y_value + 1) * DISPLAY_WIDTH)], 0x0);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 3) + ((y_value + 1) * DISPLAY_WIDTH)], 0x1);

    // Expect: |****|
    CU_ASSERT_EQUAL(cpu->frame_buffer[x_value + ((y_value + 2) * DISPLAY_WIDTH)], 0x1);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 1) + ((y_value + 2) * DISPLAY_WIDTH)], 0x1);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 2) + ((y_value + 2) * DISPLAY_WIDTH)], 0x1);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 3) + ((y_value + 2) * DISPLAY_WIDTH)], 0x1);

    // Expect: |*  *|
    CU_ASSERT_EQUAL(cpu->frame_buffer[x_value + ((y_value + 3) * DISPLAY_WIDTH)], 0x1);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 1) + ((y_value + 3) * DISPLAY_WIDTH)], 0x0);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 2) + ((y_value + 3) * DISPLAY_WIDTH)], 0x0);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 3) + ((y_value + 3) * DISPLAY_WIDTH)], 0x1);

    // Expect: |*  *|
    CU_ASSERT_EQUAL(cpu->frame_buffer[x_value + ((y_value + 4) * DISPLAY_WIDTH)], 0x1);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 1) + ((y_value + 4) * DISPLAY_WIDTH)], 0x0);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 2) + ((y_value + 4) * DISPLAY_WIDTH)], 0x0);
    CU_ASSERT_EQUAL(cpu->frame_buffer[(x_value + 3) + ((y_value + 4) * DISPLAY_WIDTH)], 0x1);

    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_SKP(void)
{
    uint16 opcode = 0xe19e;
    uint8 x = get_X(opcode);
    uint8 key = 0x8;            // 2nd row middle key
    cpu->program_counter = PROGRAM_START;
    cpu->V[x] = key;
    cpu->key_state[key] = 1;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 4);

    cpu->program_counter = PROGRAM_START;
    cpu->key_state[key] = 0;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_SKNP(void)
{
    uint16 opcode = 0xe2A1;
    uint8 x = get_X(opcode);
    uint8 key = 0xf;
    uint16 PC = 0x200;

    cpu->program_counter = PROGRAM_START;
    cpu->V[x] = key;
    cpu->key_state[key] = 1;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);

    cpu->program_counter = PROGRAM_START;
    cpu->key_state[key] = 0;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 4);
}

void test_LDxDT(void)
{
    uint16 opcode = 0xf507;
    uint8 x = get_X(opcode);
    uint8 DT = 0x3f;

    cpu->V[x] = 0x0;
    cpu->delay_timer = DT;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->V[x], DT);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

// TODO: LD Vx, K - must mock await keypress

void test_LDDTx(void)
{
    uint16 opcode = 0xfb15;
    uint8 x = get_X(opcode);
    uint8 DT = 0x1b;

    cpu->V[x] = DT;
    cpu->delay_timer = 0x0;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->delay_timer, DT);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_LDSTx(void)
{
    uint16 opcode = 0xfc18;
    uint8 x = get_X(opcode);
    uint8 ST = 0xef;

    cpu->V[x] = ST;
    cpu->sound_timer = 0x0;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->sound_timer, ST);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_ADDIx(void)
{
    uint16 opcode = 0xf01e;
    uint8 x = get_X(opcode);
    uint16 I = 0xabc;
    uint8 x_value = 0x43;

    cpu->I = I;
    cpu->V[x] = x_value;
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->I, (I + x_value));
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_LDFx(void)
{
    uint16 opcode = 0xf529;
    uint8 x = get_X(opcode);

    for (uint8 sprite = 0x0; sprite <= 0xf; sprite++) {
        cpu->V[x] = sprite;
        cpu->program_counter = PROGRAM_START;
        execute_opcode(cpu, opcode);
        CU_ASSERT_EQUAL(cpu->I, (sprite * 5));
        CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
    }
}

void test_LDBx(void)
{
    uint16 opcode = 0xf533;
    uint8 x = get_X(opcode);
    uint8 x_value = 0xb7;       // 183
    uint8 h_value = 0x1;
    uint8 t_value = 0x8;
    uint8 o_value = 0x3;

    cpu->V[x] = x_value;
    cpu->I = PROGRAM_START;
    for (uint16 i = cpu->I; i < 3; i++) {
        cpu->RAM[i] = 0x0;
    }
    cpu->program_counter = PROGRAM_START;

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->RAM[cpu->I], h_value);
    CU_ASSERT_EQUAL(cpu->RAM[cpu->I + 1], t_value);
    CU_ASSERT_EQUAL(cpu->RAM[cpu->I + 2], o_value);
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_LDIx(void)
{
    uint16 opcode = 0xff55;
    uint8 x = get_X(opcode);
    uint16 I = 0x300;
    uint8 test_values[] = {
        0xf, 0xe, 0xd, 0xc,
        0xb, 0xa, 0x9, 0x8,
        0x7, 0x6, 0x5, 0x4,
        0x3, 0x2, 0x1, 0x0
    };

    cpu->program_counter = PROGRAM_START;
    cpu->I = I;
    for (uint8 i = 0; i <= 0xf; i++) {
        cpu->V[i] = test_values[i];
    }

    execute_opcode(cpu, opcode);
    CU_ASSERT_EQUAL(cpu->I, I);
    for (uint16 i = 0; i <= 0xf; i++) {
        CU_ASSERT_EQUAL(cpu->RAM[cpu->I+ i + 1], test_values[i]);
    }
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

void test_LDxI(void)
{
    uint16 opcode = 0xf865;
    uint8 x = get_X(opcode);
    uint8 test_values[] = {
        0xf, 0xe, 0xd, 0xc,
        0xb, 0xa, 0x9, 0x8,
        0x7, 0x6, 0x5, 0x4,
        0x3, 0x2, 0x1, 0x0
    };

    cpu->program_counter = PROGRAM_START;
    cpu->I = 0x250;
    for (uint8 i = 0; i <= 0xf; i++) {
        cpu->V[i] = 0x0;
        cpu->RAM[cpu->I + i + 1] = test_values[i];
    }

    execute_opcode(cpu, opcode);
    for (uint8 i = 0 ; i <= x; i++) {
        CU_ASSERT_EQUAL(cpu->V[i], test_values[i]);
    }
    for (uint8 i = (x + 1); i <= 0xf; i++) {
        CU_ASSERT_EQUAL(cpu->V[i], 0x0);
    }
    CU_ASSERT_EQUAL(cpu->program_counter, PROGRAM_START + 2);
}

int init_cpu_suite(void)
{
    cpu = initialize_cpu(STANDARD);
    return 0;
}

int clean_cpu_suite(void)
{
    destroy_cpu(cpu);
    return 0;
}

int main(void)
{
    if (CU_initialize_registry() != CUE_SUCCESS) {
        printf("Failed to initialize registry.\n");
        return CU_get_error();
    }

    CU_Suite *test_suite = CU_add_suite(
        "CPU unit tests",
        init_cpu_suite,
        clean_cpu_suite);
    if (test_suite == NULL) {
        printf("Failed to add suite to registry.\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        CU_add_test(
            test_suite,
            "CPU | initialize_cpu sets all values to 0, PC to 0x200, and adds fontset to RAM",
            test_initialize_cpu) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | fetch_opcode gets value at PC and combines with next value",
            test_fetch_opcode) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | CLS flips all pixels to 0",
            test_CLS) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | RET sets PC to top of stack then decrements SP",
            test_RET) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | JP sets PC to address in opcode",
            test_JP) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | CALL appends PC to stack and assigns opcode address to PC",
            test_CALL) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | SE skips next instruction if register value is equal to opcode byte",
            test_SE) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | SNE skips next instruction if register value is not equal to opcode byte",
            test_SNE) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | SNE x,y skips next instruction if register values are equal",
            test_SExy) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | LD assigns byte to register",
            test_LD) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | ADD adds byte to register",
            test_ADD) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | LD x,y sets x register to value of y register",
            test_LDxy) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | OR sets x register to x OR y",
            test_OR) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | AND sets x register to x AND y",
            test_AND) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | XOR sets x register to x XOR y",
            test_XOR) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | AND x,y sets x to x + y and carry flag if sum is greater than 8 bits",
            test_ANDxy) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | SUB subtracts y from x and set carry flag if x is greater",
            test_SUB) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | SHR divides register x by 2 and sets carry flag if x is odd",
            test_SHR) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | SUBN sets register x to y - x and sets carry flag if y is greater",
            test_SUBN) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | SHL multiples register by 2 and sets carry flag when greater than 0x7f",
            test_SHL) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | SNExy skips next instruction if register values are not equal",
            test_SNExy) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | LD I sets I to address in opcode",
            test_LDI) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | JP V0 sets PC to sum of opcode address and value in register V0",
            test_JPV0) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | DRW flips frame buffer pixels at register values to match sprite in I for height in opcode",
            test_DRW) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | SKP skips next instruction if key value in register is pressed",
            test_SKP) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | SKNP skips next instruction if key value in register is not pressed",
            test_SKNP) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | LDxDT set register to value of delay timer",
            test_LDxDT) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | LDDTx sets delay timer to value in register",
            test_LDDTx) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | LDSTx sets sounds timer to value in register",
            test_LDSTx) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | ADDIx adds register value to I",
            test_ADDIx) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | LDFx sets I to location of hex value in register",
            test_LDFx) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | LDBx sets I[0..3] to units of register's decimal value",
            test_LDBx) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | LDIx sets register values in memory starting at I",
            test_LDIx) == NULL ||
        CU_add_test(
            test_suite,
            "CPU | LDxI set memory values starting at I into registers",
            test_LDxI) == NULL) {
        printf("Failed to add tests to suite.\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}

// Helper functions
int count_pixels(uint8 *buffer)
{
    int count = 0;
    for (uint16 i = 0; i < DISPLAY_SIZE; i++) {
        count += buffer[i];
    }

    return count;
}

uint8 get_X(uint16 opcode)
{
    return (opcode & 0x0f00) >> 8;
}

uint8 get_Y(uint16 opcode)
{
    return (opcode & 0x00f0) >> 4;
}

uint8 get_byte(uint16 opcode)
{
    return opcode & 0x00ff;
}

uint16 get_addr(uint16 opcode)
{
    return opcode & 0x0fff;
}
