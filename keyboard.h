#ifndef Keyboard
#define Keyboard

#include "cpu.h"

void displayKeyPress(uint8 *);

struct CPU readKeyOpcode(uint16, struct CPU);

#endif
