#include "display.h"

void clearDisplay(uint8 *frameBuffer) {
  for (uint16 i = 0; i < (DISPLAY_WIDTH * DISPLAY_HEIGHT); i++) {
    frameBuffer[i] = 0x0;
  }
}

void putFrameBuffer(uint8 *frameBuffer) {
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
      if (frameBuffer[x + (y * 64)] == 1) {
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
