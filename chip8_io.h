#ifndef IO
#define IO

#include <ncurses.h>
#include "cpu.h"

#define WINDOW_HEIGHT 32
#define WINDOW_WIDTH 64

struct HashEntry {
  uint8 key;
  uint8 value;
  struct HashEntry *next;
};

struct HashTable {
  uint8 size;
  struct HashEntry *entries[KEYBOARD_SIZE];
};

WINDOW * initializeDisplay();

struct HashTable * initializeInput();

void setKeyState(WINDOW *, uint8 *, struct HashTable *);

uint8 getKeyValue(struct HashTable *, uint8);

uint8 hashKey(uint8);

uint8 validateKeyPress(uint8);

void drawFrameBuffer(WINDOW *, uint8 *);

void testFrameBuffer(WINDOW *, uint8 *);

void destroyIO(WINDOW *, struct HashTable *);

// debugging
void putHashTable(struct HashTable *);

#endif
