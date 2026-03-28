#include "renderer.h"
#include "SDL_error.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "display.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <wchar.h>

#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32

struct Renderer {
  SDL_Window *window;
  SDL_Renderer *renderer;
  int scale;
  int width;
  int height;
};

typedef struct {
  uint8_t r, g, b, a;
} RGBA;

RGBA bg_color = {5, 10, 40, 200};       // dark background
RGBA pixel_color = {80, 180, 255, 255}; // bright pixel

/*
 this is a mistake made by me since i used an opaque struct like prototyping in
main.c i.e "Renderer renderer" but since compiler doesnt know size of renderer
defined in renderer.h it has to be allocated dynamically not in stack i.e
Renderer *renderer = malloc(sizeof(Renderer))


// here r is a pointer to Renderer type which is predefined in renderer.h
int renderer_init(Renderer *r, int scale) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL Init failed: %s\n", SDL_GetError());
    return -1;
  }
*/

Renderer *renderer_create(int scale) {
  Renderer *r = malloc(sizeof(Renderer));
  if (!r)
    return NULL;

  /*
   if (SDL_Init(SDL_INIT_VIDEO) != 0) {
     fprintf(stderr, "SDL init failed :%s\n", SDL_GetError());
     free(r);
     return NULL;
   }
 */
  r->scale = scale;
  r->height = CHIP8_HEIGHT * scale;
  r->width = CHIP8_WIDTH * scale;

  r->window =
      SDL_CreateWindow("Chippy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       r->width, r->height, SDL_WINDOW_SHOWN);

  if (!r->window) {
    fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
    SDL_Quit();
    free(r);
    return NULL;
  }

  r->renderer = SDL_CreateRenderer(r->window, -1, SDL_RENDERER_ACCELERATED);
  // SDL_RENDERER_ACCELERATED is used because this flag essentially tells use to
  // use
  // hardware acceleration on gpu if possible instead of software rendering on
  // cpu
  if (!r->renderer) {
    fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
    return NULL;
  }

  return r;
}

void renderer_clear(Renderer *r) {
  // setting screen black first then renderer clear
  SDL_SetRenderDrawColor(r->renderer, bg_color.r, bg_color.g, bg_color.b,
                         bg_color.a);
  SDL_RenderClear(r->renderer);
}

void renderer_draw(Display *disp, Renderer *r) {
  // setting background dark
  SDL_SetRenderDrawColor(r->renderer, bg_color.r, bg_color.g, bg_color.b,
                         bg_color.a); // sets drawing color
  SDL_RenderClear(r->renderer);       // filling the whole window with dark

  // on bright pixels
  SDL_SetRenderDrawColor(r->renderer, pixel_color.r, pixel_color.g,
                         pixel_color.b, pixel_color.a);
  // looping through the framebuffer
  for (int y = 0; y < CHIP8_HEIGHT; y++) {
    for (int x = 0; x < CHIP8_WIDTH; x++) {
      //(A 2D coordinate (x, y) is mapped to a 1D index using the formula:)
      //(index = y * width + x)
      int index = y * CHIP8_WIDTH + x;
      if (disp->gfx_bool[index]) {
        SDL_Rect pixel = {x * r->scale, y * r->scale, r->scale, r->scale};
        SDL_RenderFillRect(r->renderer, &pixel);
      }
    }
  }
  disp->draw_flag = 0;
}

void renderer_present(Renderer *r) { SDL_RenderPresent(r->renderer); }

void renderer_destroy(Renderer *r) {
  if (!r)
    return;
  SDL_DestroyRenderer(r->renderer);
  SDL_DestroyWindow(r->window);
  free(r);
  // SDL_Quit();
}
