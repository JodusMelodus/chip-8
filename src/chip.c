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
        chip.rom[i] = font[i - 0x050];
    }

    chip_clear_frame_buffer(&chip);

    return chip;
}

void chip_load_rom(struct Chip *chip, const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "CHIP-8: Error opening file\n");
        abort();
    }

    size_t max_size = 4096 - 0x200;
    size_t n = fread(&chip->rom[0x200], 1, max_size, fp);

    if (n == 0)
    {
        fprintf(stderr, "CHIP-8: ROM is empty\n");
        fclose(fp);
        abort();
    }

    printf("CHIP-8: Loaded ROM with %zu bytes\n", n);

    if (ferror(fp))
    {
        fprintf(stderr, "CHIP-8: Error reading file\n");
        fclose(fp);
        abort();
    }

    if (fgetc(fp) != EOF)
    {
        fprintf(stderr, "CHIP-8: ROM too large (max %zu bytes)\n", max_size);
        fclose(fp);
        abort();
    }

    fclose(fp);
    chip->pc = 0x200;
}

void chip_tick(struct Chip *chip)
{
    // fetch
    uint8_t instruction_high = chip->rom[chip->pc];
    uint8_t instruction_low = chip->rom[chip->pc + 1];

    chip->delay_timer--;
    chip->sound_timer--;
    chip->pc += 2;

    uint16_t instruction = (uint16_t)(instruction_high << 4) | instruction_low;
    uint8_t opcode = instruction_high & 0xF0; // instruction kind
    uint8_t x = instruction_high & 0x0F;      // Variable register lookup
    uint8_t y = instruction_low & 0xF0;       // Variable register lookup
    uint8_t n = instruction_low & 0x0F;       // 4 bit immediate number
    uint8_t nn = instruction_low;             // 8 bit immediate number
    uint16_t nnn = instruction & 0x0FFF;      // 12 bit immediate address

    if (instruction == 0x00E0) // clear screen
        chip_clear_frame_buffer(chip);
    else if (opcode == 0x1) // jump
        chip->pc = nnn;
    else if (opcode == 0x6) // set register VX
        chip->v[x] = nn;
    else if (opcode == 0x7) // add value to VX
        chip->v[x] += nn;
    else if (opcode == 0xA) // set index register
        chip->i = nnn;
    else if (opcode == 0xD) // display/draw
        chip_draw_frame_buffer(chip);
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

void chip_clear_frame_buffer(struct Chip *chip)
{
    for (uint8_t r = 0; r < 64; r++)
    {
        for (uint8_t c = 0; c < 32; c++)
        {
            chip->frame_buffer[c][r] = 0;
        }
    }
}
