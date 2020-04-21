#include "types.h"
#include "constants.h"

typedef enum {
    ADD_AND_SHOW,
    DRAW_SPRITES,
    COUNTDOWN
} TestProgram;

const uint16 add_and_show[] = {
    15,
    0x00E0,         // Clear screen
    // Needs prompt
    0xF00A,         // Await key press, store in V0
    // Needs prompt
    0xF10A,         // Await key press, store in V1
    0x8014,         // Adds V1 and V0, with carry
    0xF029,         // Set I to sprite representing value of V0
    0x601E,         // Set sprite's x = half display width - half sprite width
    0x610D,         // Set sprite's y = half display height - half sprite height (uneven)
    0xD015,         // Draw sprite referenced by I at x, y location
    0x6003,         // Set value for timer to 3
    0xF018,         // Set sound timer to value in VX (3)
    0x00E0,         // Clear screen
    0xF00A,         // Await key press store in V0
    0x3000,         // Skip next instruction if V0 == 0x0
    0x1216,         // jump back 2 instructions
    0x00E0          // Clear screen
};

const uint16 draw_sprites[] = {
    10,
    0x00E0,     // clear screen
    0x600a,     // set V0 = 10
    0xF018,     // set sound timer to V0 (10 => 500 ms)
    0xF00A,     // await key press, store in V0
    0x00E0,     // clear screen before drawing new sprite
    0xF029,     // set I to sprite in V0
    0x601E,     // set V0 to 0x1e
    0x610D,     // set V1 to 0x0d
    0xD015,     // draw sprite at I at x, y
    0x1202      // jump key press await
};

const uint16 countdown[] = {
    21,
    // Clear screen
    0x00E0,                             // 0x200
    // Set sound timer to 180 (3 s)
    0x64B4,                             // 0x202
    // Set sprite to 0xf
    0x600f,                             // 0x204
    // Skip next if V0 != 0
    0x4000,                             // 0x206
    // Jump to last instruction
    0x1228,                             // 0x208
    // Skip setting timer unless V0 == 3
    0x4003,                             // 0x20a
    // Set sound timer to 180 (3s)
    0xF418,                             // 0x20c
    // Set X, Y
    0x611E,                             // 0x20e
    0x620D,                             // 0x210
    // Draw sprite at X, Y
    0xF029,                             // 0x212
    0xD125,                             // 0x214
    // Set timer to 60 (1s)
    0x633c,                             // 0x216
    0xF315,                             // 0x218
    // Get delay value
    0xF307,                             // 0x21a
    // Skip next if delay == 0
    0x3300,                             // 0x21c
    // Jump to get delay value
    0x121a,                             // 0x21e
    // Clear screen
    0x00E0,                             // 0x220
    // Set subtractor
    0x6501,                             // 0x222
    // Decrement V0 by 1
    0x8055,                             // 0x224
    // Jump to control flow
    0x1206,                             // 0x226
    0x00E0                              // 0x228
};
