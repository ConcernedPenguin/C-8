#include "chip8.h"

#include "SDL_log.h"
#include "cpu.h"
#include "display.h"
#include "renderer.h"

#include <SDL2/SDL.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FONT_START 0x000

bool init_chip8(Chip8 *chip8, const char rom_name[]) {

  const uint32_t entry_point =
      0x200; // chip8 roms always get loaded at 0x200 mem adress;
  const uint8_t font[] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80, // F
  };

  // initialize the entire chip8_machine
  memset(chip8, 0, sizeof(Chip8));

  // allocating cpu
  chip8->cpu = malloc(sizeof(CPU));
  cpu_init(chip8->cpu);

  // allocating display
  chip8->display = malloc(sizeof(Display));
  memset(chip8->display, 0, sizeof(Display));

  // creating and allocating renderer which happens inside the function as a
  // whole
  chip8->renderer = renderer_create(10);

  if (!chip8->cpu || !chip8->display || !chip8->renderer) {
    destroy_chip8(chip8);
    return false;
  }

  // load font
  memcpy(&chip8->cpu->memory[FONT_START], font, sizeof(font));

  // open rom file
  FILE *rom = fopen(rom_name, "rb");
  if (!rom) {
    SDL_Log("Rom file %s is invalid or does not exist\n", rom_name);
    return false;
  }

  // check rom size
  fseek(rom, 0, SEEK_END); // Moving pointer to end
  // size_t is used to represent sizes and counts and is the result of sizeof()
  // func
  const size_t rom_size = ftell(rom);
  const size_t max_size = sizeof chip8->cpu->memory - entry_point;
  rewind(rom); // sets file position to the begining of the file

  if (rom_size > max_size) {
    SDL_Log("Rom file %s of size %zu is too big, the maxm size allowed is %zu",
            rom_name, rom_size, max_size);
  }

  // load rom
  if (fread(&chip8->cpu->memory[entry_point], rom_size, 1, rom) != 1) {
    SDL_Log("Could not read rom file %s into CHIP8 memory", rom_name);
    return false;
  }

  fclose(rom);

  // set chip8 machine defaults
  chip8->cpu->pc = entry_point;
  chip8->rom_name = rom_name;
  chip8->cpu->sp = 0; // chip8->cpu->stack[0];
  chip8->state = RUNNING;
  return true;
}

void destroy_chip8(Chip8 *chip8) {
  if (chip8->renderer)
    renderer_destroy(chip8->renderer);
  if (chip8->display)
    free(chip8->display);
  if (chip8->cpu)
    free(chip8->cpu);
}
