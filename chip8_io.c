#include <stdlib.h>
#include "chip8_io.h"

void initializeDisplay() {
  WINDOW *mainWindow;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, 1);

  uint8 startY = (LINES - WINDOW_HEIGHT) / 2;
  uint8 startX = (COLS - WINDOW_WIDTH) / 2;

  printw("Press F1 to exit\n");

  refresh();

  mainWindow = newwin(WINDOW_HEIGHT, WINDOW_WIDTH, startY, startX);
  box(mainWindow, 0, 0);
  wrefresh(mainWindow);
}

void closeDisplay() {
  endwin();
}

uint8 keyboard[KEYBOARD_SIZE] = {
  0x2c,   // 0 -- (,)
  0x37,   // 1 -- (7)
  0x38,   // 2 -- (8)
  0x39,   // 3 -- (9)
  0x75,   // 4 -- (u)
  0x69,   // 5 -- (i)
  0x6f,   // 6 -- (o)
  0x6a,   // 7 -- (j)
  0x6b,   // 8 -- (k)
  0x6c,   // 9 -- (l)
  0x6d,   // a -- (m)
  0x2e,   // b -- (.)
  0x30,   // c -- (0)
  0x70,   // d -- (p)
  0x3b,   // e -- (;)
  0x2f    // f -- (/)
};

struct HashTable *createKeyTable() {
  struct HashTable *keyTable;

  keyTable = malloc(sizeof(struct HashTable));

  keyTable->entries = (struct HashEntry **)malloc(KEYBOARD_SIZE * sizeof(struct HashEntry));

  for (uint8 i = 0; i < KEYBOARD_SIZE; i++) {
    struct HashEntry *entry;
    entry = malloc(sizeof(struct HashEntry));
    entry->key = keyboard[i];
    entry->value = i;

    uint8 idx = hashKey(keyboard[i]);
    struct HashEntry *temp = keyTable->entries[idx];
    if (keyTable->entries[idx] != NULL) {
      temp = keyTable->entries[idx];
      entry->next = temp;
      keyTable->entries[idx] = entry;
    } else {
      entry->next = NULL;
      keyTable->entries[idx] = entry;
    }
  }

  return keyTable;
}

// void putTable(struct HashTable *keyTable) {
//   for (uint8 i = 0; i < KEYBOARD_SIZE; i++) {
//     printf("%x: %x\n", keyboard[i], getKeyValue(keyTable, keyboard[i]));
//   }
// }

uint8 getKeyValue(struct HashTable *keyTable, uint8 key) {
  uint8 idx = hashKey(key);
  struct HashEntry *temp;

  temp = keyTable->entries[idx];
  while (temp != NULL) {
    if (temp->key == key) {
      return temp->value;
    }
    temp = temp->next;
  }

  return 0xff;
}

uint8 hashKey(uint8 key) {
  return key % KEYBOARD_SIZE;
}
