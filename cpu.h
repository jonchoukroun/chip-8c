#ifndef CHIP_8
#define CHIP_8

#include <stdint.h>
// remove when exporting functions to other c file
#include <stdio.h>

#define RAM_SIZE 4096
#define REGISTER_COUNT 16
#define KEYBOARD_SIZE 16
#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 64
#define STACK_SIZE 8

typedef uint8_t uint8;
typedef uint16_t uint16;

struct CPU {
  uint8 V[REGISTER_COUNT];
  uint16 I;

  uint8 stack[STACK_SIZE];
  uint8 stackPointer;

  uint8 delayTimer;
  uint8 SoundTimer;

  uint8 frameBuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];

  uint16 programCounter;

  uint8 RAM[RAM_SIZE];

  uint8 keyboard[KEYBOARD_SIZE];
};

struct CPU initialize();

uint16 fetchOpcode(struct CPU *);

void readOpcode(uint16, struct CPU *);

void executeMathInstruction(uint16, struct CPU *);

void executeTimerInstruction(uint16, struct CPU *);

uint8 generateRandomNumber();

#endif
