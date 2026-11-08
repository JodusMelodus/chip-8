#include "chip.h"

#include <stdio.h>
#include <stdlib.h>

extern const uint8_t font[5 * 16] = {
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
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

struct Chip chip_new()
{
    struct Chip chip = {0};

    // Popular font address
    for (uint16_t i = 0x050; i < 0x09F; i++)
    {
        chip.memory[i] = font[i - 0x050];
    }

    for (uint8_t r = 0; r < 64; r++)
    {
        for (uint8_t c = 0; c < 32; c++)
        {
            chip.frame_buffer[c][r] = 0;
        }
    }

    return chip;
}

void chip_tick(struct Chip *chip)
{
}

void chip_draw_frame_buffer(struct Chip *chip)
{
    printf("\033[H");
    for (uint8_t r = 0; r < 32; r++)
    {
        for (uint8_t c = 0; c < 64; c++)
        {
            printf(chip->frame_buffer[r][c] ? "# " : ". ");
        }
        printf("\n");
    }
}

void chip_stack_push(struct Chip *chip, uint16_t value)
{
    if (chip->sp >= 16)
    {
        fprintf(stderr, "CHIP-8: Stack Overflow (sp=%u)\n", chip->sp);
        abort();
    }

    chip->stack[chip->sp++] = value;
}

uint16_t chip_stack_pop(struct Chip *chip)
{
    if (chip->sp == 0)
    {
        fprintf(stderr, "CHIP-8: Stack Underflow\n");
        abort();
    }

    return chip->stack[--chip->sp];
}
