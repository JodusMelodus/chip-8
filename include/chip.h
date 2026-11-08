#ifndef CHIP_H
#define CHIP_H

#include <stdint.h>

extern const uint8_t font[5 * 16];

struct Chip
{
    uint8_t memory[4096];         // 4 kilobytes RAM
    uint8_t frame_buffer[32][64]; // 64x32 pixels, monochrome
    uint16_t pc;                  // program counter
    uint16_t i;                   // index register, used to point to locations in memory
    uint16_t stack[16];           // stack
    uint8_t delay_timer;          // decremented at a rate of 60Hz
    uint8_t sound_timer;          // decremented at a rate of 60Hz and beeping when not 0
    uint8_t v[16];                // general purpose variable registers
};

struct Chip chip_new();
void chip_tick(struct Chip *chip);
void chip_draw_frame_buffer(struct Chip *chip);

#endif