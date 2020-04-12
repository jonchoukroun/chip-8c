#include "types.h"
#include "constants.h"

// TODO: add prompt
// #define PROGRAM_SIZE 15
// const uint16 add_and_show[PROGRAM_SIZE] = {
//     0x00E0,         // Clear screen
//     // Needs prompt
//     0xF00A,         // Await key press, store in V0
//     // Needs prompt
//     0xF10A,         // Await key press, store in V1
//     0x8010,         // Adds V1 and V0, with carry
//     0xF029,         // Set I to sprite representing value of V0
//     0x601E,         // Set sprite's x = half display width - half sprite width
//     0x610D,         // Set sprite's y = half display height - half sprite height (uneven)
//     0xD015,         // Draw sprite referenced by I at x, y location
//     0x6003,         // Set value for timer to 3
//     0xF018,         // Set sound timer to value in VX (3)
//     0x00E0,         // Clear screen
//     0xF00A,         // Await key press store in V0
//     0x3000,         // Skip next instruction if V0 == 0x0
//     0x1216,         // jump back 2 instructions
//     0x00E0          // Clear screen
// };

// #define PROGRAM_SIZE 7
// const uint16 draw_sprites[PROGRAM_SIZE] = {
//     0x00E0,
//     0xF00A,
//     0xF029,
//     0x601E,
//     0x610D,
//     0xD015,
//     0x1200
// };

#define PROGRAM_SIZE 10
const uint16 draw_keypress[PROGRAM_SIZE] = {
    0x00E0,
    0x6001,     // Set V0 = 1
    0x611E,     // set display x
    0x620D,     // set display y
    0xF029,     // set I to V0
    0xD125,     // draw I at x, y 5px
    0xE0A1,     // skip if 1 not is pressed
    0x1212,     // jump to end
    0x1202,     // jump to top + 2
    0x00E0
};
