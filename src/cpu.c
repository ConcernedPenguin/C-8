#include "cpu.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

Display *display;
// before anything an opcode in binary looks like 0000 xxxx yyyy nnnn
// eg 6 A 0 F -- > in binary --> 0110 1010 0000 1111
CPU cpu;
void cpu_init(CPU *cpu) {
  memset(cpu, 0, sizeof(CPU));
  // chip8 roms start at 0x200
  cpu->pc = 0x200;
}

void cpu_emulate_instruction(CPU *cpu, Display *display) {
  // fetching a single chip8 opcode from memory
  // chip8 instructions are 16bits and memory stores 8 bits per address
  // so a memory will be like
  // pc -> 0x60
  // pc + 1 -> 0x0A
  // so instruction will be 0x600A

  uint16_t opcode = (cpu->memory[cpu->pc] << 8) | cpu->memory[cpu->pc + 1];
  // here <<8 is used to shift the memory address to 8 bits left
  // i.e 11111111 - > 11111111100000000 since memory uses to address to store
  // and opcode
  //  here '|' is bitwise operator (inclusive OR)

  // printf("Opcode: %04X\n", opcode);
  cpu->pc += 2; /// pre-increment program counter for next opcode since memory
                /// address are in 2 byte

  // instruction format eg 6XNN
  // 6 is opcode family(6), X is register index(X), NN are 8 bit immediate
  // value(NN)

  // 6 X  N  N
  //| |  |  |
  //| |  ---- 8-bit immediate value (NN)
  //| -------- register index (X)
  //|--------- opcode family (6)

  // similary D X Y N and others are checked and instructions are filled
  // and as an eg DXYN means draw sprite at (V2, V3), height 5 when D235
  // decoding and filling out current instruction format
  uint16_t nnn =
      opcode &
      0x0FFF; // here '&' is a bitwise AND operator i.e extracts NNN from opcode
  uint8_t nn = opcode & 0x00FF;     // exctracting NN
  uint8_t n = opcode & 0x000F;      // exttracting N
  uint8_t x = (opcode >> 8) & 0x0F; // extracting X -- range: V0-VF ie 0-F
  uint8_t y = (opcode >> 4) & 0x0F; // extracing Y -- same as X

  // emulate opcode
  // first grouping instruction by opcode family(the first digit in opcode)

  switch (
      (opcode >> 12) &
      0x0F) // opcode >>eg: 0000 0000 0000 0110 >>12  = 0x6 and 0x6 & 0x0F = 0x6
  {
  case 0x00: // cases starting with 00 i.e 00E0 and 00EE
    if (nn == 0xE0) {
      // 0x00E0 :  clear the screen opcode
      display_clear(display);
      display->draw_flag = 1;
    } else if (nn == 0xEE) {
      // 0x00EE: Set the PC to last address on subroutine stack (pop it of from
      // stack) so that the next opcode will be being gotten from that address
      // that was saved when the subroutine was called
      cpu->sp--; // sp points to next empty value so first move back sp to the
                 // last valid entry
      cpu->pc = cpu->stack[cpu->sp]; // s
    }
    break;

  case 0x01:
    // 0x1NNN: jumps to address of NNN;
    cpu->pc = nnn;
    break;

  case 0x02:
    // 0x2NNN: calls subroutine at NNN
    // save current address to return to on subroutine stack and set PC to
    // subroutine address so that the next opcode is going to be gotten from
    // their
    cpu->stack[cpu->sp] = cpu->pc;
    cpu->sp++;
    cpu->pc = nnn; // override PC
    break;

  case 0x03:
    // 0x3XNN: skip the next instruction if VX equals NN (usually the next
    // instruction is a jump to skip a code block)
    if (cpu->v[x] == nn)
      cpu->pc +=
          2; // here the opcode jumps or skips the current code block i.e pc+=2;
    break;

  case 0x04:
    // 0x4NNN: skips the next instruction if vx does not equal NN
    if (cpu->v[x] != nn)
      cpu->pc += 2;
    break;

  case 0x05:
    // 0x5XY0: skips the next instruction if VX equals VY;
    if (cpu->v[x] == cpu->v[y])
      cpu->pc += 2;
    break;

  case 0x06:
    // 0x6XNN: sets VX register to NN
    cpu->v[x] = nn;
    break;

  case 0x07:
    // 0x0=7XNN: adds NN to VX
    cpu->v[x] += nn;
    break;

  case 0x08:
    // cases starting with '0x8'
    switch (n) {
    case (0):
      // 0x8XY0: set register VX = VY
      cpu->v[x] = cpu->v[y];
      break;

    case (1):
      // 0x8XY1: sets vx to (VX bitwise OR VY)
      cpu->v[x] = cpu->v[x] | cpu->v[y];

      break;

    case (2):
      // 0x8XY2: sets vx to (VX bitwise AND VY)
      cpu->v[x] = cpu->v[x] & cpu->v[y];
      break;

    case (3):
      // 0x8XY3: sets vx to (VX bitwise XOR VY)
      cpu->v[x] = cpu->v[x] ^ cpu->v[y];
      break;

    case (4):
      // 0x8XY4: add VY TO VX and set VF to 1 if carry
      // carry happens when sum exceeds 255(0xFF) coz registers are 8bit
      if (cpu->v[x] + cpu->v[y] > 0xFF)
        cpu->v[0xF] = 1;
      cpu->v[x] += cpu->v[y];
      break;

    case (5):
      // 0x8XY5: VY is subtracted from VX then VF is set to 0 when there is no
      // underflow and 1 when therer is not ie VF set to 1 if VX>=VY (VF is set
      // before shifting)
      if (cpu->v[x] >= cpu->v[y])
        cpu->v[0xF] = 1;
      cpu->v[x] -= cpu->v[y];
      break;

    case (6):
      // 0x8XY6: shifts VX to the right by 1. Store the least significant bit
      // of VX prior to the shift into VF
      cpu->v[0xF] = cpu->v[x] & 0x1;
      cpu->v[x] = cpu->v[x] >> 1;
      break;

    case (7):
      // 0x8XY7: sets VX to VY minus VX then VF is set to ) when there is an
      // underflow and 1 when not i.e if VY >= VX then VF = 1
      if (cpu->v[y] >= cpu->v[x])
        cpu->v[0xF] = 1;
      cpu->v[x] = cpu->v[y] - cpu->v[x];
      break;

    case (0xE):
      // 0x8XYE: shifts VX to the left by 1, then sets VF the most significant
      // bit of VX prior to the shift
      cpu->v[0xF] = (cpu->v[x] >> 7) & 0x1;
      cpu->v[x] = cpu->v[x] << 1;
      break;
    }
    break;

  case 0x09:
    // 0x9XY0: skips next instruction if vx does not equal vy
    if (cpu->v[x] != cpu->v[y])
      cpu->pc += 2;
    break;

  case 0x0A:
    // 0xANNN: sets index register I to the address NNN
    cpu->i = nnn;
    break;

  case 0x0B:
    // 0XBNNN: jumps to the address NNN plus V0
    cpu->pc = cpu->v[0] + nnn;
    break;

  case 0x0C: {
    // 0xCXNN: sets VX to the result of a bitwise AND operation
    // on a random number(0-255) and NN
    uint8_t random = rand() & 0xFF; // generating random number and masking it
                                    // so only upto 255 is generated
    cpu->v[x] = random & nn;
    break;
  }

  case 0x0D: {
    // 0xDXYN: draw a sprite at coordinate(VX, VY) that has a width of 8px and a
    // height of N pixels. Read from memory location I; screen pixels are XOR'd
    // with sprite bits, VF (carry flag) is set if any screen pixels are set off
    // --> useful for collision detection

    // this is the hardest opcode to implement so writing down what the manual
    // instruction says
    /*
     Set the X coordinate to the value in VX modulo 64 (or, equivalently, VX &
    63, where & is the binary AND operation) Set the Y coordinate to the value
    in VY modulo 32 (or VY & 31) Set VF to 0 For N rows: Get the Nth byte of
    sprite data, counting from the memory address in the I register (I is not
    incremented) For each of the 8 pixels/bits in this sprite row (from left to
    right, ie. from most to least significant bit): If the current pixel in the
    sprite row is on and the pixel at coordinates X,Y on the screen is also on,
    turn off the pixel and set VF to 1 Or if the current pixel in the sprite row
    is on and the screen pixel is not, draw the pixel at the X and Y coordinates
    If you reach the right edge of the screen, stop drawing this row Increment X
    (VX is not incremented) Increment Y (VY is not incremented) Stop if you
    reach the bottom edge of the screen
    */
    uint8_t x_coord = (cpu->v[x] % VIDEO_WIDTH);
    uint8_t y_coord = (cpu->v[y] % VIDEO_HEIGHT);
    const uint8_t x_og = x_coord; // original position in x coordinate
    cpu->v[0xF] = 0;              // initalize carry flag as 0

    // loop over all N rows of the sprite
    for (uint8_t row = 0; row < n; row++) {
      // get next byte/row of sprite data
      uint8_t sprite_data = cpu->memory[cpu->i + row];

      x_coord = x_og; // resetting x for the next row to draw

      for (uint8_t column = 0; column < 8; column++) {
        // checking out of bounds
        if (x_coord >= VIDEO_WIDTH || y_coord >= VIDEO_HEIGHT)
          break;
        // mapping 2d space to 1d array using formula (i = x + width * y)
        bool *pixel = &display->gfx_bool[x_coord + y_coord * VIDEO_WIDTH];
        //
        bool sprite_bit = (sprite_data & (0x80 >> column)) !=
                          0; // here precedency also comes into play
        if (sprite_bit && *pixel) {
          cpu->v[0xF] = 1; // set carry flag 1
        }

        // XOR display pixel with sprite pixel/bit to set it on or off
        *pixel ^= sprite_bit;

        // stop drawing when we reach the right edge of the screen
        if (x_coord >= VIDEO_WIDTH)
          break;

        // increment X
        x_coord++;
      }
      // stop drawing entire sprite after reaching bottom of screen and
      // increment Y
      if (y_coord >= VIDEO_HEIGHT)
        break;
      y_coord++;
    }
    display->draw_flag = 1;
    break;
  }

  case 0x0E:
    // cases starting with 0xE i.e 0xEX9E and 0xEAX1
    switch (nn) {
    // 0xEX9E: skips the next instruction if the key stored in VX(only
    // considering the lowest nibble) is pressed
    case (0x9E): {
      // only the lowest nibble of VX is used as key index so masking it
      // i.e cpu->v[x] can be 0-255 but keypad is only 0-15 which might cause
      // out of bounds memory access
      uint8_t key = cpu->v[x] & 0x0F;

      if (cpu->keypad[cpu->v[key]])
        cpu->pc += 2;
      break;
    }
    // 0xEAX1: skips next instruction if the key is stored in VX is not pressed
    // (opposite of EX9E)
    case (0xA1): {
      // only the lowest nibble of VX is used as key index so masking it
      // i.e cpu->v[x] can be 0-255 but keypad is only 0-15 which might cause
      // out of bounds memory access
      uint8_t key = cpu->v[x] & 0x0F;

      if (!cpu->keypad[cpu->v[key]])
        cpu->pc += 2;
      break;
    }
    }
    break;

  case 0x0F:
    // cases starting with 0xF
    switch (nn) {
    case (0x07):
      // 0xFX07: sets VX reg to the value of the delay timer
      cpu->v[x] = cpu->delay_timer;
      break;

    case (0x0A): {
      // 0xFX0A: A key press is awaited then it is stored in VX(blocking
      // operation, all sintruction halted until next key event but delay and
      // sound timers should countinue processing)
      bool any_key_pressed = false;
      // static uint8_t key = 0xFF;

      for (uint8_t i = 0; i < 16; i++) {
        if (cpu->keypad[i]) {
          cpu->v[x] = i;
          any_key_pressed = true;
          break;
        }
      }

      if (!any_key_pressed)
        cpu->pc -= 2; // repeating the instruction

      break;
    }

    case (0x15):
      // 0xFX15: sets the delay timer to VX
      cpu->delay_timer = cpu->v[x];
      break;

    case (0x1E):
      // 0xFX1E: adds vx to I while not affecting the VF(no carry)
      cpu->i += cpu->v[x];
      break;

      break;
    }
    break;
  default:
    printf("Unimplemented opcode: 0x%04X\n", opcode);
    break;
  }
}
