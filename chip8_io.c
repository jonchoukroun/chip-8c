#include <stdlib.h>
#include "chip8_io.h"

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

uint8 hashKey(uint8);

WINDOW * initializeDisplay()
{
  WINDOW *mainWindow;

  initscr();
  clear();
  cbreak();
  noecho();

  uint8 startY = (LINES - WINDOW_HEIGHT) / 2;
  uint8 startX = (COLS - WINDOW_WIDTH) / 2;

  // printw("Press F1 to exit\n");
  refresh();

  mainWindow = newwin(WINDOW_HEIGHT + 2, WINDOW_WIDTH + 2, startY - 2, startX - 2);
  nodelay(mainWindow, 1);
  keypad(mainWindow, 1);
  box(mainWindow, 0, 0);
  wrefresh(mainWindow);

  return mainWindow;
}

struct HashTable * initializeInput()
{
  struct HashTable *keyTable = malloc(sizeof(struct HashTable));

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

void setKeyState(WINDOW *window, uint8 *keyState, struct HashTable *keyTable)
{
  uint8 key;
  if ((key = wgetch(window))) {
    keyState[key] = getKeyValue(keyTable, key);
  } else {
    return;
  }
}

uint8 getKeyValue(struct HashTable *keyTable, uint8 key)
{
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

void clearFrameBuffer(uint8 *frameBuffer)
{
  for (uint16 i = 0; i < (DISPLAY_WIDTH * DISPLAY_HEIGHT); i++) {
    frameBuffer[i] = 0x0;
  }
}

void drawFrameBuffer(WINDOW * window, uint8 *frameBuffer)
{
  for (uint8 y = 0; y < DISPLAY_HEIGHT; y++) {
    for (uint8 x = 0; x < DISPLAY_WIDTH; x++) {
      if (frameBuffer[x + (y * DISPLAY_WIDTH)] == 1) {
        mvwaddch(window, y + 1, x + 1, ACS_CKBOARD);
      } else {
        mvwaddch(window, y + 1, x + 1, ' ');
      }
    }
  }
  box(window, 0, 0);
  wrefresh(window);
}

void destroyIO(WINDOW *window, struct HashTable *keyTable)
{
  free(keyTable);

  delwin(window);
  endwin();
}

uint8 hashKey(uint8 key)
{
  return key % KEYBOARD_SIZE;
}
