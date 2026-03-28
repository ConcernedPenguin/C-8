#include "display.h"
#include <string.h>

// clear the framebuffer
void display_clear(Display *disp) {
  // signature and args of memset coz i didint really abt memset lol
  // void *memset(void *ptr, int value, size_t num);
  memset(disp->gfx, 0, sizeof(disp->gfx));
  // now setting the draw flag to 1 because the framebuffer is cleared
  // so renderer must know the screen should be updated
  disp->draw_flag = 1;
}

// settings a single pixel
void display_set_pixel(Display *disp, int x, int y, uint8_t value) {
  // it prevents out of bounds
  // ie if x or y more that the actual width and height
  if (x < 0 || x >= CHIP8_WIDTH || y < 0 || y >= CHIP8_HEIGHT)
    return;

  // mapping a 2d array (x, y) to 1d array(gfx) using formula
  //(A 2D coordinate (x, y) is mapped to a 1D index using the formula:)
  //(index = y * width + x)
  int index = y * CHIP8_WIDTH + x;
  disp->gfx[index] = value;
  disp->draw_flag = 1;
}
