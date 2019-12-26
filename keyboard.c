#include "keyboard.h"

void displayKeyPress(uint8 * keyboard) {
  for (uint8 i = 0; i < KEYBOARD_SIZE; i++) {
    if (keyboard[i] != 0) {
      printf("keypress: %x\n", keyboard[i]);
    }
  }
}

void readKeyOpcode(uint16 opcode, struct CPU *cpu) {
  uint8 key;

  switch (opcode & 0x00ff) {
  case 0x9e:
    if (cpu->keyboard[cpu->V[(opcode & 0x0f00) >> 8]] != 0) {
      cpu->programCounter += 2;
    }
    break;

  case 0xa1:
    if (cpu->keyboard[cpu->V[(opcode & 0x0f00) >> 8]] == 0) {
      cpu->programCounter += 2;
    }
    break;

  case 0x0a:
    // Wait for keypress...
    key = 0;
    cpu->V[(opcode & 0x0f00) >> 8] = key;
    break;

  default:
    printf("Could not match opcode %x\n", opcode);
    break;
  }
}
