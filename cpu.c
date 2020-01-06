#include "cpu.h"
#include "fontset.h"
#include "chip8_io.h"

uint8 executeMathInstruction(struct CPU *, uint16);

uint8 executeFInstructions(struct CPU *, uint16);

uint8 executeInputInstruction(struct CPU *, uint16);

uint8 generateRandomNumber();

struct CPU * initialize(struct HashTable *keyTable)
{
  struct CPU *cpu = malloc(sizeof(struct CPU));
  cpu->programCounter = 0x200;
  cpu->stackPointer = 0x0;

  for (uint16 i = 0; i < RAM_SIZE; i++) {
    cpu->RAM[i] = 0;
  }

  for (uint8 i = 0; i < 80; i++) {
    cpu->RAM[i] = fontSet[i];
  }

  for (uint8 i = 0; i <= 0xf; i++) {
    cpu->V[i] = 0;
  }

  for (uint16 i = 0; i < 2048; i++) {
    cpu->frameBuffer[i] = 0;
  }

  cpu->keyTable = keyTable;

  for (uint8 i = 0; i < KEYBOARD_SIZE; i++) {
    cpu->keyState[i] = 0;
  }

  cpu->drawFlag = 0;

  return cpu;
}

uint16 fetchOpcode(struct CPU *cpu)
{
  return cpu->RAM[cpu->programCounter] << 8 | cpu->RAM[cpu->programCounter + 1];
}

uint8 executeOpcode(struct CPU *cpu, uint16 opcode)
{
  uint8 status = 1;

  switch ((opcode & 0xf000) >> 12) {
  case 0x0:
    if ((opcode & 0x00ff) == 0xe0) {
      clearFrameBuffer(cpu->frameBuffer);
      cpu->drawFlag = 1;

      cpu->programCounter += 2;
    } else if ((opcode & 0x00ff) == 0xee) {
      cpu->programCounter = cpu->stack[cpu->stackPointer];
      cpu->stackPointer--;
    } else {
      // Ignored jump to subroutine
      cpu->programCounter +=2;
    }
    break;

  case 0x1:
    cpu->programCounter = opcode & 0x0fff;
    break;

  case 0x2:
    cpu->stackPointer++;
    cpu->stack[cpu->stackPointer] = cpu->programCounter;
    cpu->programCounter = opcode & 0x0fff;
    break;

  case 0x3:
    if ((cpu->V[(opcode & 0x0f00) >> 8]) == (opcode & 0x00ff)) {
      cpu->programCounter += 2;
    }

    cpu->programCounter += 2;
    break;

  case 0x4:
    if ((cpu->V[(opcode & 0x0f00) >> 8]) != (opcode & 0x00ff)) {
      cpu->programCounter += 2;
    }

    cpu->programCounter += 2;
    break;

  case 0x5:
    if (cpu->V[(opcode & 0x0f00) >> 8] == cpu->V[(opcode & 0x00f0) >> 4]) {
      cpu->programCounter += 2;
    }

    cpu->programCounter += 2;
    break;

  case 0x6:
    cpu->V[(opcode & 0x0f00) >> 8] = (opcode & 0x00ff);

    cpu->programCounter += 2;
    break;

  case 0x7:
    cpu->V[(opcode & 0x0f00) >> 8] += (opcode & 0x00ff);

    cpu->programCounter += 2;
    break;

  case 0x8:
    status = executeMathInstruction(cpu, opcode);

    cpu->programCounter += 2;
    break;

  case 0x9:
    if ((opcode & 0x000f) != 0) {
      status = 0;
    }

    if (cpu->V[(opcode & 0x0f00) >> 8] != cpu->V[(opcode & 0x00f0) >> 4]) {
      cpu->programCounter += 2;
    }

    cpu->programCounter += 2;
    break;

  case 0xa:
    cpu->I = opcode & 0x0fff;

    cpu->programCounter += 2;
    break;

  case 0xb:
    cpu->programCounter = cpu->V[0x0] + (opcode & 0x0fff);
    break;

  case 0xc: {
    uint8 num = generateRandomNumber();
    cpu->V[(opcode & 0x0f00) >> 8] = num & (opcode & 0x00ff);

    cpu->programCounter += 2;
    break;
  }

  case 0xd: {
    uint8 height = opcode & 0x000f;
    uint8 width = 0x8;
    uint16 pixel;
    cpu->V[0xf] = 0;

    for (uint8 row = 0; row < height; row++) {
      pixel = cpu->RAM[cpu->I + row];
      uint8 y = (cpu->V[(opcode & 0x00f0) >> 4] + row) % DISPLAY_HEIGHT;
      for (uint8 col = 0; col < width; col++) {
        uint8 x = (cpu->V[(opcode & 0x0f00) >> 8] + col) % DISPLAY_WIDTH;

        if (pixel & (0x80 >> col)) {
          uint16 frame = x + (y * 64);
          if (cpu->frameBuffer[frame]) { cpu->V[0xf] = 1; }

          cpu->frameBuffer[frame] ^= 1;
        }
      }
    }
    cpu->drawFlag = 1;
    cpu->programCounter += 2;
    break;
  }

  case 0xe:
    status = executeInputInstruction(cpu, opcode);

    cpu->programCounter += 2;
    break;

  case 0xf:
    status = executeFInstructions(cpu, opcode);
    cpu->programCounter += 2;
    break;

  default:
    // printw("Cannont match opcode: %x\n", opcode);
    status = 0;
  }

  return status;
}

uint8 executeMathInstruction(struct CPU *cpu, uint16 opcode)
{
  uint8 x = (opcode & 0x0f00) >> 8;
  uint8 y = (opcode & 0x00f0) >> 4;
  switch (opcode & 0x000f) {
  case 0x0:
    cpu->V[x] = cpu->V[y];
    break;

  case 0x1:
    cpu->V[x] |= cpu->V[y];
    break;

  case 0x2:
    cpu->V[x] &= cpu->V[y];
    break;

  case 0x3:
    cpu->V[x] ^= cpu->V[y];
    break;

  case 0x4:
    if (cpu->V[x] + cpu->V[y] > 255) {
      cpu->V[0xf] = 1;
    }
    cpu->V[x] += cpu->V[y];
    break;

  case 0x5:
    if (cpu->V[x] > cpu->V[y]) {
      cpu->V[0xf] = 1;
    } else {
      cpu->V[0xf] = 0;
    }
    cpu->V[x] -= cpu->V[y];
    break;

  case 0x6:
    cpu->V[0xf] = cpu->V[x] & 1;
    cpu->V[x] >>= 1;
    break;

  case 0x7:
    if (cpu->V[y] > cpu->V[x]) {
      cpu->V[0xf] = 1;
    } else {
      cpu->V[0xf] = 0;
    }
    cpu->V[x] = cpu->V[y] - cpu->V[x];
    break;

  case 0xe:
    cpu->V[0xf] = cpu->V[x] & 1 << (sizeof(uint8) - 1);
    cpu->V[x] <<= 1;
    break;

  default:
    // printw("Could not match opcode %x\n", opcode);
    return 0;
  }

  return 1;
}

uint8 executeFInstructions(struct CPU *cpu, uint16 opcode)
{
  uint8 x = (opcode & 0x0f00) >> 8;

  switch (opcode & 0x0ff) {
  case 0x07:
    cpu->V[x] = cpu->delayTimer;
    break;

  case 0x0a: {
    uint8 key = getch();
    cpu->V[x] = getKeyValue(cpu->keyTable, key);
    break;
  }

  case 0x15:
    cpu->delayTimer = cpu->V[x];
    break;

  case 0x18:
    cpu->soundTimer = cpu->V[x];
    break;

  case 0x1e:
    cpu->I += cpu->V[x];
    break;

  case 0x29:
    cpu->I = cpu->V[x] * 5;
    break;

  case 0x33: {
    uint8 decimal = cpu->V[x];
    uint8 sig = 100;

    while (decimal > 0) {
      cpu->RAM[cpu->I] = decimal / sig;
      decimal %= sig;
      sig /= 10;
      cpu->I += 1;
    }
    break;
  }

  case 0x55:
    for (uint8 i = 0x0; i <= x; i++) {
      cpu->RAM[cpu->I] = cpu->V[i];
      cpu->I++;
    }
    break;

  case 0x65:
    for (uint8 i = 0x0; i <= x; i++) {
      cpu->V[i] = cpu->RAM[cpu->I];
      cpu->I++;
    }
    break;

  default:
    // printw("Could not match opcode %x\n", opcode);
    return 0;
  }

  return 1;
}

uint8 executeInputInstruction(struct CPU *cpu, uint16 opcode)
{
  switch (opcode & 0x00ff) {
  case 0x9e:
    if (cpu->keyState[cpu->V[(opcode & 0x0f00) >> 8]] != 0) {
      cpu->programCounter += 2;
    }
    break;

  case 0xa1:
    if (cpu->keyState[cpu->V[(opcode & 0x0f00) >> 8]] == 0) {
      cpu->programCounter += 2;
    }
    break;

  default:
    // printw("Could not match opcode %x\n", opcode);
    return 0;
  }

  return 1;
}

uint8 generateRandomNumber()
{
  FILE *randomFile = fopen("/dev/urandom", "r");
  if (randomFile < 0) {
    // printw("Cannot open\n");
    return -1;
  } else {
    uint8 randomData[2];
    ssize_t result = fread(randomData, sizeof(uint8), (sizeof(randomData) / sizeof(uint8)), randomFile);
    if (result < 0) {
      // printw("cannon read\n");
    } else {
      return randomData[0];
    }
  }

  return -1;
}

void destroyCPU(struct CPU *cpu) {
  free(cpu);
}
