#include <SDL2/SDL.h>
#include <stdio.h>
#include "input.h"

void initialize_hashtable(HashTable *);
uint8 hash_key(uint8);

const uint8 SDL_KEYBOARD[KEYBOARD_SIZE] = {
    SDL_SCANCODE_KP_2,
    SDL_SCANCODE_NUMLOCKCLEAR,
    SDL_SCANCODE_KP_EQUALS,
    SDL_SCANCODE_KP_DIVIDE,
    SDL_SCANCODE_KP_7,
    SDL_SCANCODE_KP_8,
    SDL_SCANCODE_KP_9,
    SDL_SCANCODE_KP_4,
    SDL_SCANCODE_KP_5,
    SDL_SCANCODE_KP_6,
    SDL_SCANCODE_KP_1,
    SDL_SCANCODE_KP_3,
    SDL_SCANCODE_KP_MULTIPLY,
    SDL_SCANCODE_KP_MINUS,
    SDL_SCANCODE_KP_PLUS,
    SDL_SCANCODE_KP_ENTER
};

HashTable * create_hashtable()
{
    HashTable *table = malloc(sizeof(HashTable));
    initialize_hashtable(table);

    uint8 max_index = 0;
    for (uint8 i = 0; i < KEYBOARD_SIZE; i++) {
        HashEntry *entry;
        entry = malloc(sizeof(HashEntry));
        entry->key = SDL_KEYBOARD[i];
        entry->value = i;

        uint8 idx = hash_key(SDL_KEYBOARD[i]);
        max_index = idx > max_index ? idx : max_index;

        HashEntry *temp = table->entries[idx];
        if (table->entries[idx] != NULL) {
            temp = table->entries[idx];
            entry->next = temp;
            table->entries[idx] = entry;
        } else {
            entry->next = NULL;
            table->entries[idx] = entry;
        }
    }
    table->max_index = max_index;

    return table;
}

uint8 get_key_value(HashTable *key_table, uint8 key) {
    uint8 idx = hash_key(key);

    HashEntry *temp = calloc(1, sizeof(HashEntry));

    temp = key_table->entries[idx];

    while (temp != NULL) {
        if (temp->key == key) {
            return temp->value;
        }

        temp = temp->next;
    }

    return 0xff;
}

void initialize_hashtable(HashTable *table)
{
    table->max_index = 0;

    for (uint8 i = 0; i < KEYBOARD_SIZE; ++i) {
        HashEntry *entry = malloc(sizeof(HashEntry));
        entry->key = 0xff;
        entry->value = 0xff;
        entry->next = NULL;
        table->entries[i] = entry;
    }
}

uint8 hash_key(uint8 key)
{
    return key % KEYBOARD_SIZE;
}
