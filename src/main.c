#include "SDL_events.h"
#include "SDL_timer.h"
#include "chip8.h"
#include "cpu.h"
#include "display.h"
#include "renderer.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <rom_name>\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  Chip8 chip8 = {0};
  if (!init_chip8(&chip8, argv[1])) {
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // srand(time(NULL));

  while (chip8.state != QUIT) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {

      case SDL_QUIT: // quits on clicking X button on the program
        chip8.state = QUIT;
        break;

      case SDL_KEYDOWN: // quits on Esc key
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          chip8.state = QUIT;
          break;
        }
        // key symbols like sdlk1 sdlk_q etc live in event.keysym.sym (learned
        // the harder way)
        switch (event.key.keysym.sym) {
        case SDLK_SPACE:
          // spacebar
          if (chip8.state == RUNNING) {
            chip8.state = PAUSED;
            puts("=============PAUSED============");
          } else
            chip8.state = RUNNING;
          break;

        case SDLK_1:
          chip8.cpu->keypad[0x1] = true;
          break;
        case SDLK_2:
          chip8.cpu->keypad[0x2] = true;
          break;
        case SDLK_3:
          chip8.cpu->keypad[0x3] = true;
          break;
        case SDLK_4:
          chip8.cpu->keypad[0xC] = true;
          break;

        case SDLK_q:
          chip8.cpu->keypad[0x4] = true;
          break;
        case SDLK_w:
          chip8.cpu->keypad[0x5] = true;
          break;
        case SDLK_e:
          chip8.cpu->keypad[0x6] = true;
          break;
        case SDLK_r:
          chip8.cpu->keypad[0xD] = true;
          break;

        case SDLK_a:
          chip8.cpu->keypad[0x7] = true;
          break;
        case SDLK_s:
          chip8.cpu->keypad[0x8] = true;
          break;
        case SDLK_d:
          chip8.cpu->keypad[0x9] = true;
          break;
        case SDLK_f:
          chip8.cpu->keypad[0xE] = true;
          break;

        case SDLK_z:
          chip8.cpu->keypad[0xA] = true;
          break;
        case SDLK_x:
          chip8.cpu->keypad[0x0] = true;
          break;
        case SDLK_c:
          chip8.cpu->keypad[0xB] = true;
          break;
        case SDLK_v:
          chip8.cpu->keypad[0xF] = true;
          break;
        }
        break;

      case SDL_KEYUP:
        switch (event.key.keysym.sym) {
        case SDLK_SPACE:
          // spacebar
          if (chip8.state == RUNNING) {
            chip8.state = PAUSED;
            puts("=============PAUSED============");
          } else {
            chip8.state = RUNNING;
          }
          break;

        case SDLK_1:
          chip8.cpu->keypad[0x1] = false;
          break;
        case SDLK_2:
          chip8.cpu->keypad[0x2] = false;
          break;
        case SDLK_3:
          chip8.cpu->keypad[0x3] = false;
          break;
        case SDLK_4:
          chip8.cpu->keypad[0xC] = false;
          break;

        case SDLK_q:
          chip8.cpu->keypad[0x4] = false;
          break;
        case SDLK_w:
          chip8.cpu->keypad[0x5] = false;
          break;
        case SDLK_e:
          chip8.cpu->keypad[0x6] = false;
          break;
        case SDLK_r:
          chip8.cpu->keypad[0xD] = false;
          break;

        case SDLK_a:
          chip8.cpu->keypad[0x7] = false;
          break;
        case SDLK_s:
          chip8.cpu->keypad[0x8] = false;
          break;
        case SDLK_d:
          chip8.cpu->keypad[0x9] = false;
          break;
        case SDLK_f:
          chip8.cpu->keypad[0xE] = false;
          break;

        case SDLK_z:
          chip8.cpu->keypad[0xA] = false;
          break;
        case SDLK_x:
          chip8.cpu->keypad[0x0] = false;
          break;
        case SDLK_c:
          chip8.cpu->keypad[0xB] = false;
          break;
        case SDLK_v:
          chip8.cpu->keypad[0xF] = false;
          break;
        }
        break;
      }
    }
    if (chip8.state == PAUSED)
      continue;

    for (int i = 0; i < 6; i++) { // frames per second
      cpu_emulate_instruction(chip8.cpu, chip8.display);
    }

    // timers tick at 60hz/updating timers
    if (chip8.cpu->delay_timer > 0)
      chip8.cpu->delay_timer--;

    if (chip8.cpu->sound_timer > 0)
      chip8.cpu->sound_timer--;

    if (chip8.display->draw_flag) {
      // renderer_clear(chip8.renderer);
      renderer_clear(chip8.renderer);
      renderer_draw(chip8.display, chip8.renderer);
      renderer_present(chip8.renderer);
      chip8.display->draw_flag = 0;
    }

    SDL_Delay(16);
  }

  destroy_chip8(&chip8);
  SDL_Quit();
  return 0;
}
