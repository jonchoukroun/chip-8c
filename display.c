#include "display.h"

void clearDisplay(uint8 *frame_buffer) {
  for (uint16 i = 0; i < (DISPLAY_WIDTH * DISPLAY_HEIGHT); i++) {
    frame_buffer[i] = 0x0;
  }
}

void putFrameBuffer(uint8 *frame_buffer) {
  printf("\n\n\t\t");

  // Debugging
  // printf("|");
  // for (uint8 y = 0; y < 8; y++) {
  //   for (uint8 x = 0; x < 8; x++) {
  //     printf("%d", x);
  //   }
  // }
  // printf("|");

  for (uint8 i = 0; i < 66; i++) {
    printf("_");
  }
  printf("\n\t\t|");

  for (uint8 y = 0; y < 32; y++) {
    for (uint8 x = 0; x < 64; x++) {
      if (frame_buffer[x + (y * 64)] == 1) {
        printf("*");
      } else {
        printf(" ");
      }
    }
    printf("|");
    printf("\n\t\t|");
  }

  for (uint8 i = 0; i < 64; i++) {
    printf("_");
  }
  printf("|\n");

  printf("\n");
}
