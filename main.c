#include <stdio.h>
#include "chip8_io.h"
#include "cpu.h"

int main(int argc, char const *argv[]) {

  // struct CPU cpu = initialize();
  // initializeDisplay();

  // for (;;) {
  //   uint8 keyPress = getKeyPress();

  //   if (keyPress == 0x71) { break; }
  // }

  // uint16 opcode = fetchOpcode(&cpu);
  // printf("op %x\n", opcode);
  // readOpcode(opcode, &cpu);

  struct HashTable *keyTable = createKeyTable();

  // closeDisplay();

  return 0;
}
