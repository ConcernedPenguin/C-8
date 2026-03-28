#ifndef CPU_H
#define CPU_H

#include "display.h"
#include <stdbool.h>
#include <stdint.h>

#define MEMORY_SIZE 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

// specifications

typedef struct {

  uint8_t memory[MEMORY_SIZE];
  // 16 8 bit registers V0-VF
  uint8_t v[REGISTER_COUNT];

  uint16_t stack[STACK_SIZE];

  // special registers
  // memory address register, program counter,
  // stack pointer
  uint16_t i;
  uint16_t pc;
  uint8_t sp;
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint32_t last_timer_tick; // this is the last tick counter added to remove
                            // flickering problem in the emulator after many
                            // debugging
  bool keypad[16];
  // uint8_t draw_flag; // a flag to know drawing screen state
  //  uint32_t gfx_color[64 * 32]; // display buffer
  //  bool gfx[64 * 32];           // chip8 og resolution pixels
  // Display *disp;
} CPU;

// initializing cpu state
void cpu_init(CPU *cpu);

// executing one instruction
void cpu_emulate_instruction(CPU *cpu, Display *display);

#endif
