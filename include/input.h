#ifndef HASHTABLE
#define HASHTABLE

#include <stdlib.h>
#include "types.h"
#include "constants.h"

typedef struct HashEntry {
    uint8 key;
    uint8 value;
    struct HashEntry *next;
} HashEntry;

typedef struct HashTable {
    uint8 max_index;
    HashEntry *entries[KEYBOARD_SIZE];
} HashTable;

HashTable * create_hashtable();

uint8 get_key_value(HashTable *, uint8);

#endif

