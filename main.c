#include <stdio.h>
#include "chip8_io.h"
#include "cpu.h"

uint8 runCycle(struct CPU *);

int main(int argc, char const *argv[]) {

  struct CPU *cpu = initialize();
  WINDOW *window = initializeDisplay();
  struct HashTable *keyTable = initializeInput();

  uint8 running = 1;
  while (running) {
    setKeyState(window, cpu->keyState, keyTable);

    running = runCycle(cpu);
    if (running == 0) {
      // debugger: require keypress to end program
      printw("No opcode match\n");
      getch();
      break;
    }

    if (cpu->drawFlag == 1) {
      drawFrameBuffer(window, cpu->frameBuffer);
      cpu->drawFlag = 0;
    }
  }

  destroyIO(window, keyTable);

  // printf("%x\n", cpu->programCounter);
  // uint16 opcode = fetchOpcode(cpu);
  // readOpcode(opcode, cpu);

  destroyCPU(cpu);
  return 0;
}

uint8 runCycle(struct CPU *cpu) {
  uint16 opcode = fetchOpcode(cpu);
  uint8 status = executeOpcode(cpu, opcode);
  if (status == 0) { return 0; }

  if (cpu->delayTimer > 0) {
    --cpu->delayTimer;
  }

  if (cpu->soundTimer > 0) {
    --cpu->soundTimer;
  }

  return 1;
}
