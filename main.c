#include <stdio.h>
#include "cpu.h"

int main(int argc, char const *argv[]) {

  struct CPU cpu = initialize();
  uint16 opcode = fetchOpcode(&cpu);
  printf("op %x\n", opcode);
  readOpcode(opcode, &cpu);

  return 0;
}
