#ifndef CHIP8_H
#define CHIP8_H

#include "cpu.h"
#include "renderer.h"
#include <stdbool.h>

#define MEMORY_SIZE 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

// Emulator states
typedef enum {
  QUIT,
  RUNNING,
  PAUSED,
} emulator_state;

// chip8 machine object
typedef struct Chip8 {
  CPU *cpu;
  Display *display;
  Renderer *renderer;

  emulator_state state;
  const char *rom_name;
} Chip8;

/*
00E0   CLS
00EE   RET
1NNN   JP
2NNN   CALL
3XNN   SE
4XNN   SNE
6XNN   LD Vx, NN
7XNN   ADD Vx, NN
8XY0   LD Vx, Vy
8XY4   ADD Vx, Vy
ANNN   LD I, NNN
DXYN   DRW




Chip8 *chip8_create(int scale);

void chip8_run(Chip8 *chip8);

int chip8_load_rom(Chip8 *chip8, const char *filename);

void chip8_destroy(Chip8 *chip8);
*/
bool init_chip8(Chip8 *chip8, const char rom_name[]);

void destroy_chip8(Chip8 *chip8);

bool load_state(Chip8 *chip8, const char *filename);

bool save_state(const Chip8 *chip8, const char *filename);

#endif
