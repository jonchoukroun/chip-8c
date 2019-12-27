#include <stdio.h>
#include <time.h>
#include "chip8_io.h"
#include "cpu.h"

// testing
#define PROGRAM_SIZE 50

uint8 runCycle(struct CPU *);

uint8 loadProgram(struct CPU *);

int main(int argc, char const *argv[]) {
  struct CPU *cpu = initialize();
  WINDOW *window = initializeDisplay();
  struct HashTable *keyTable = initializeInput();

  uint8 status = loadProgram(cpu);
  if (status == 0) {
    printw("Could not load program. Press any key to exit.\n");
    nanosleep(0, 16667);
  }

  while (status) {
    setKeyState(window, cpu->keyState, keyTable);

    status = runCycle(cpu);
    // testFrameBuffer(window, cpu->frameBuffer);

    if (status == 0) {
      printw("Press any key to shut down...\n");
      wrefresh(window);
      getch();
      break;
    }

    if (cpu->drawFlag == 1) {
      drawFrameBuffer(window, cpu->frameBuffer);
      cpu->drawFlag = 0;
    }
  }

  destroyIO(window, keyTable);

  destroyCPU(cpu);
  return 0;
}

uint8 runCycle(struct CPU *cpu) {
  if (cpu->programCounter == 0) {
    return 0;
  }
  uint16 opcode = fetchOpcode(cpu);
  uint8 status = executeOpcode(cpu, opcode);
  if (status == 0) { return 0; }
  // printw("executed %x\n", opcode);
  // refresh();

  if (cpu->delayTimer > 0) {
    --cpu->delayTimer;
  }

  if (cpu->soundTimer > 0) {
    --cpu->soundTimer;
  }

  return 1;
}

uint8 loadProgram(struct CPU *cpu) {
  // Test instructions, start loading at RAM[0x200]
  uint16 program[PROGRAM_SIZE] = {
    // draw A
    0x600a,
    0xf029,
    0x6100,
    0x6200,
    0xd125,
    // draw B
    0x600b,
    0xf029,
    0x613c,
    0xd125,
    // draw C
    0x600c,
    0xf029,
    0x6100,
    0x623b,
    0xd125,
    // draw D
    0x600d,
    0xf029,
    0x613c,
    0xd125,
    // shutdown
    0x1000
  };

  for (uint8 i = 0; i < PROGRAM_SIZE; i++) {
    uint16 idx = 0x200 + (i * 2);
    uint16 opcode = program[i];
    cpu->RAM[idx] = (opcode & 0xff00) >> 8;
    cpu->RAM[idx + 1] = opcode & 0x00ff;
    // printw("(%x) ram -> %x %x\n", idx, cpu->RAM[idx], cpu->RAM[idx + 1]);
  }

  return 1;
}
