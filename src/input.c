#include <SDL2/SDL.h>
#include <stdio.h>
#include "input.h"

void initialize_hashtable(HashTable *);
uint8 hash_key(uint8);

const uint8 SDL_KEYBOARD_EXTENDED[KEYBOARD_SIZE] = {
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

const uint8 SDL_KEYBOARD_STANDARD[KEYBOARD_SIZE] = {
    SDL_SCANCODE_COMMA,
    SDL_SCANCODE_7,
    SDL_SCANCODE_8,
    SDL_SCANCODE_9,
    SDL_SCANCODE_U,
    SDL_SCANCODE_I,
    SDL_SCANCODE_O,
    SDL_SCANCODE_J,
    SDL_SCANCODE_K,
    SDL_SCANCODE_L,
    SDL_SCANCODE_M,
    SDL_SCANCODE_PERIOD,
    SDL_SCANCODE_0,
    SDL_SCANCODE_P,
    SDL_SCANCODE_SEMICOLON,
    SDL_SCANCODE_SLASH,
};

HashTable * create_hashtable(KEYBOARD_TYPE type)
{
    HashTable *table = malloc(sizeof(HashTable));
    initialize_hashtable(table);

    uint8 max_index = 0;
    for (uint8 i = 0; i < KEYBOARD_SIZE; i++) {
        uint8 key;
        if (type == STANDARD) {
            key = SDL_KEYBOARD_STANDARD[i];
        } else {
            key = SDL_KEYBOARD_EXTENDED[i];
        }

        HashEntry *entry;
        entry = malloc(sizeof(HashEntry));
        entry->key = key;
        entry->value = i;

        uint8 idx = hash_key(key);
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
