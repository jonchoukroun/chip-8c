#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "chip8_io.h"
#include "cpu.h"

// #define CLOCK_RATE 500
// #define NS_CONVERSION 1000000000L

#define MS_CLOCK_RATE 16667L

// test programs
#define PROGRAM_SIZE 50
uint16 cornerLetters[PROGRAM_SIZE] = {
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

uint16 timerLetters[PROGRAM_SIZE] = {
  0x60b4,   // store delay length in V0
  0x610a,   // store fontset A in V1
  0xf129,   // set register I to fontest value in V1
  0x620e,   // store value 0xe in V2
  0xf015,   // set delay timer to value in V0
  0xd225,   // draw 5 bits height of fontset in I at x, y values of V2
  0xf007,   // store current delay timer value in V0
  0x3000,   // skip next instuction if value stored in V3 is 0
  0x120c,   // jump to address 0x212 to retrieve new delay timer value
  0x00e0,   // clear screen
  0x1000,   // terminate
};

uint16 keyDisplay[PROGRAM_SIZE] = {
  0x00e0,   // clear screen
  0x601d,   // set V0 to center x
  0x610d,   // set V1 to center y
  0xf20a,   // store keypress in V2
  0x42ff,   // skip if V2 isn't 71 (q?)
  0x1000,   // terminate
  0xf229,   // set I register to value of stored byte in V2
  0x00e0,   // clear screen
  0xd015,   // draw stored fontset
  0x1206    // jump to keypress store
};

uint16 byteSpread[PROGRAM_SIZE] = {
  0x600a,
  0x6105,
  0x6200,
  0xf029,
  0xd215,

  0x6205,
  0xd215,

  0x620a,
  0xd215,

  0x620f,
  0xd215,

  0x6214,
  0xd215,

  0x6219,
  0xd215,

  0x621e,
  0xd215,

  0x6223,
  0xd215,

  0x6228,
  0xd215,

  0x622d,
  0xd215,

  0x6232,
  0xd215,

  0x6237,
  0xd215,

  0x623c,
  0xd215,

  0x1000
};

uint8 runCycle(struct CPU *);

uint8 loadProgram(struct CPU *);

void decrementCounters(struct CPU *);

int main(int argc, char const *argv[])
{
  WINDOW *window = initializeDisplay();
  struct HashTable *keyTable = initializeInput();
  struct CPU *cpu = initialize(keyTable);

  uint8 status = loadProgram(cpu);
  if (status == 0) {
    printw("Could not load program. Press any key to exit.\n");
    sleep(1);
  }

  while (status) {
    setKeyState(window, cpu->keyState, keyTable);

    status = runCycle(cpu);

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

    // const long clockRate = 1 / (NS_CONVERSION * CLOCK_RATE);
    // struct timespec clockSpeed = {0};
    // clockSpeed.tv_nsec = clockRate;
    // nanosleep(&clockSpeed, NULL);
    usleep(MS_CLOCK_RATE);
  }

  destroyIO(window, keyTable);

  destroyCPU(cpu);
  return 0;
}

uint8 runCycle(struct CPU *cpu)
{
  if (cpu->programCounter == 0) {
    return 0;
  }
  uint16 opcode = fetchOpcode(cpu);
  uint8 status = executeOpcode(cpu, opcode);
  if (status == 0) { return 0; }

  // debugger
  // printw("execute %x\n", opcode);
  // refresh();

  if (cpu->delayTimer > 0) {
    --cpu->delayTimer;
  }

  if (cpu->soundTimer > 0) {
    --cpu->soundTimer;
  }

  return 1;
}

uint8 loadProgram(struct CPU *cpu)
{
  // Test instructions, start loading at RAM[0x200]
  uint16 *program = keyDisplay;
  for (uint8 i = 0; i < PROGRAM_SIZE; i++) {
    uint16 idx = 0x200 + (i * 2);
    uint16 opcode = program[i];
    cpu->RAM[idx] = (opcode & 0xff00) >> 8;
    cpu->RAM[idx + 1] = opcode & 0x00ff;
  }

  return 1;
}

void decrementCounters(struct CPU *cpu)
{
  if (cpu->delayTimer > 0) {
    --cpu->delayTimer;
  }

  if (cpu->soundTimer > 0) {
    --cpu->soundTimer;
  }
}
