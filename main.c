#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "chip8_io.h"
#include "cpu.h"

#define CPU_RATE 500
#define DELAY_RATE 60
#define MS_CONVERSION 1000000L
#

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
  0x6005,   // store delay length in V0
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

void decrementTimers(struct CPU *);

uint8 loadProgram(struct CPU *);

void decrementCounters(struct CPU *);

// set counter
// run cycle
// get elapsed time
// if less than expected clock duration, sleep difference
// reset counter

int main(int argc, char const *argv[])
{
  WINDOW *window = initializeDisplay();
  struct HashTable *keyTable = initializeInput();
  struct CPU *cpu = initialize(keyTable);

  uint16 cpu_cycle_length = MS_CONVERSION / CPU_RATE;
  uint16 delay_cycle_length = MS_CONVERSION / DELAY_RATE;

  uint8 status = loadProgram(cpu);
  if (status == 0) {
    printw("Could not load program. Press any key to exit.\n");
    sleep(1);
  }

  clock_t cpu_counter = clock();
  clock_t delay_counter = clock();
  double elapsed_time;

  while (status) {
    setKeyState(window, cpu->keyState, keyTable);

    elapsed_time = ((double)(clock() - delay_counter) / CLOCKS_PER_SEC) * MS_CONVERSION;
    if (elapsed_time >= delay_cycle_length) {
      decrementCounters(cpu);
      delay_counter = clock();
    }

    elapsed_time = ((double)(clock() - cpu_counter) / CLOCKS_PER_SEC) * MS_CONVERSION;
    if (elapsed_time >= cpu_cycle_length) {
      printw("tick: %f\n", elapsed_time);
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

      cpu_counter = clock();
    } else {
      usleep(cpu_cycle_length - elapsed_time);
    }
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

  return 1;
}

void decrementTimers(struct CPU *cpu) {
  if (cpu->delayTimer > 0) {
    --cpu->delayTimer;
  }

  if (cpu->soundTimer > 0) {
    // emit sound
    --cpu->soundTimer;
  }
}

uint8 loadProgram(struct CPU *cpu)
{
  // Test instructions, start loading at RAM[0x200]
  uint16 *program = byteSpread;
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
