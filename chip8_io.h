#ifndef IO
#define IO

#include <ncurses.h>
#include "cpu.h"

#define WINDOW_HEIGHT 32
#define WINDOW_WIDTH 64

void initializeDisplay();

void closeDisplay();

struct HashEntry {
  uint8 key;
  uint8 value;
  struct HashEntry *next;
};

struct HashTable {
  uint8 size;
  struct HashEntry **entries;
};

struct HashTable *createKeyTable();

// void putTable(struct HashTable *);

uint8 getKeyValue(struct HashTable *, uint8);

uint8 hashKey(uint8);

#endif
