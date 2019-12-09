#include <stdio.h>
#include "chip8_cpu.h"

int main(int argc, char const *argv[]) {

  struct CPU cpu = initialize();
  uint16 opcode = fetchOpcode(cpu);
  cpu = readOpcode(opcode, cpu);

  return 0;
}
