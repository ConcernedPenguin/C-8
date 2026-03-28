#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32

typedef struct Display {
  // the main 64x32 framebuffer for display
  uint32_t gfx[CHIP8_WIDTH * CHIP8_HEIGHT];
  bool gfx_bool[64 * 32]; // chip8 og resolution pixels
  uint8_t draw_flag;
} Display;

// clear the screen
void display_clear(Display *disp);

// set a single pixel in framebuffer
void display_set_pixel(Display *disp, int x, int y, uint8_t value);
// here uint8_t value is the state of pixel which in our case is either 0 or 1

#endif
