#include "keyboard.h"

void displayKeyPress(uint8 * keyboard) {
  for (uint8 i = 0; i < KEYBOARD_SIZE; i++) {
    if (keyboard[i] != 0) {
      printf("keypress: %x\n", keyboard[i]);
    }
  }
}

struct CPU readKeyOpcode(uint16 opcode, struct CPU cpu) {
  switch (opcode & 0x00ff)
  {
  case 0x9e:
    if (cpu.keyboard[cpu.V[(opcode & 0x0f00) >> 8]] != 0) {
      cpu.program_counter += 2;
    }
    break;

  case 0xa1:
    if (cpu.keyboard[cpu.V[(opcode & 0x0f00) >> 8]] == 0) {
      cpu.program_counter += 2;
    }

  default:
    break;
  }
  return cpu;
}
