#include "chip.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    srand(time(NULL));
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

    if (chip->delay_timer > 0)
        chip->delay_timer--;
    if (chip->sound_timer > 0)
        chip->sound_timer--;
    chip->pc += 2;

    uint16_t instruction = ((uint16_t)(instruction_high) << 8) | instruction_low;
    uint8_t opcode = (instruction_high & 0xF0) >> 4; // instruction kind
    uint8_t x = instruction_high & 0x0F;             // Variable register lookup
    uint8_t y = (instruction_low & 0xF0) >> 4;       // Variable register lookup
    uint8_t n = instruction_low & 0x0F;              // 4 bit immediate number
    uint8_t nn = instruction_low;                    // 8 bit immediate number
    uint16_t nnn = instruction & 0x0FFF;             // 12 bit immediate address

    if (instruction == 0x00E0) // clear screen
        chip_clear_frame_buffer(chip);
    else if (opcode == 0x1) // jump
        chip->pc = nnn;
    else if (opcode == 0x2) // call subroutine at NNN
    {
        chip_stack_push(chip, chip->pc);
        chip->pc = nnn;
    }
    else if (instruction == 0x00EE) // return from subroutine
        chip->pc = chip_stack_pop(chip);
    else if (opcode == 0x3) // skip one instruction if VX == NN
    {
        if (chip->v[x] == nn)
            chip->pc += 2;
    }
    else if (opcode == 0x4) // skip one instruction if VX != NN
    {
        if (chip->v[x] != nn)
            chip->pc += 2;
    }
    else if (opcode == 0x5) // skip one instruction if VX == VY
    {
        if (chip->v[x] == chip->v[y])
            chip->pc += 2;
    }
    else if (opcode == 0x9) // skip one instruction if VX != VY
    {
        if (chip->v[x] != chip->v[y])
            chip->pc += 2;
    }
    else if (opcode == 0x6) // set register VX
        chip->v[x] = nn;
    else if (opcode == 0x7) // add value to VX
        chip->v[x] += nn;
    else if (opcode == 0x8) // arithmetic and logic
    {
        switch (n)
        {
        case 0x0: // set VX = VY
            chip->v[x] = chip->v[y];
            break;
        case 0x1: // binary OR
            chip->v[x] |= chip->v[y];
            break;
        case 0x2: // binary AND
            chip->v[x] &= chip->v[y];
            break;
        case 0x3: // binary XOR
            chip->v[x] ^= chip->v[y];
            break;
        case 0x4: // VX + VY with carry
            uint16_t sum = chip->v[x] + chip->v[y];
            chip->v[0xF] = sum >> 0xFF;
            chip->v[x] = sum & 0xFF;
            break;
        case 0x05: // VX - VY
            chip->v[x] -= chip->v[y];
            break;
        case 0x07: // VY - VX
            chip->v[x] = chip->v[y] - chip->v[x];
            break;
        case 0x06: // shift VX by one right (COULD CAUSE ERRORS IN OTHER PROGRAMS, IMPLEMENTATION CHANGED)
            chip->v[0xF] = chip->v[x] & 0x01;
            chip->v[x] >>= 1;
            break;
        case 0x0E: // shift VX by one left (COULD CAUSE ERRORS IN OTHER PROGRAMS, IMPLEMENTATION CHANGED)
            chip->v[0xF] = chip->v[x] & 0x80;
            chip->v[x] <<= 1;
            break;
        default:
            break;
        }
    }
    else if (opcode == 0xA) // set index
        chip->i = nnn;
    else if (opcode == 0xB) // jump with offset (COULD CAUSE ERRORS, IMPLEMENTATION CHANGED)
        chip->pc = nnn + chip->v[0x0];
    else if (opcode == 0xC)
        chip->v[x] = rand() & nn;
    else if (opcode == 0xD) // draw sprite
    {
        uint8_t x_start = chip->v[x] & 63;
        uint8_t y_start = chip->v[y] & 31;
        chip->v[0xF] = 0;

        for (uint8_t row = 0; row < n; row++)
        {
            uint8_t y_coord = y_start + row;
            if (y_coord >= 32)
                break; /* bottom edge: stop */

            uint8_t sprite_byte = chip->rom[chip->i + row];

            for (uint8_t col = 0; col < 8; col++)
            {
                uint8_t x_coord = x_start + col;
                if (x_coord >= 64)
                    break; /* right edge: stop this row */

                uint8_t sprite_pixel = (sprite_byte >> (7 - col)) & 1;
                if (!sprite_pixel)
                    continue;

                if (chip->frame_buffer[y_coord][x_coord])
                    chip->v[0xF] = 1; /* collision */

                chip->frame_buffer[y_coord][x_coord] ^= 1;
            }
        }
    }
    else if (opcode == 0xE)
    {
        uint8_t key = 0xF0 | (chip->v[x] & 0xF);

        if (nn == 0x9E) // skip instruction if VX key pressed
        {
            if (chip->keypress == key)
                chip->pc += 2;
        }
        else if (nn == 0xA1) // skip instruction if VX key not pressed
        {
            if (chip->keypress != key)
                chip->pc += 2;
        }
    }
    else if (opcode == 0xF)
    {
        // Timers
        if (nn == 0x07) // read sound timer
            chip->v[x] = chip->delay_timer;
        else if (nn == 0x15) // set delay timer
            chip->delay_timer = chip->v[x];
        else if (nn == 0x18) // set sound timer
            chip->sound_timer = chip->v[x];

        else if (nn == 0x1E) // add to index
            chip->i += chip->v[x];
        else if (nn == 0x0A) // loop until keypress
        {
            if ((chip->keypress & 0xF0) == 0xF0)
                chip->v[x] = chip->keypress & 0x0F;
            else
                chip->pc -= 2; // loop until key press
        }
        else if (nn == 0x29) // font character
            chip->i = 0x050 + (5 * chip->v[x]);
        else if (nn == 0x33) // BCD conversion
        {
            chip->rom[chip->i] = chip->v[x] / 100;
            chip->rom[chip->i + 1] = (chip->v[x] / 10) % 10;
            chip->rom[chip->i + 2] = chip->v[x] % 10;
        }
        else if (nn == 0x55) // store registers to rom (COULD BREAK, IMPLEMENTATION CHANGED)
        {
            for (uint8_t i = 0; i < x; i++)
                chip->rom[chip->i + i] = chip->v[i];
        }
        else if (nn == 0x65) // read registers from rom (COULD BREAK, IMPLEMENTATION CHANGED)
        {
            for (uint8_t i = 0; i < x; i++)
                chip->v[i] = chip->rom[chip->i + i];
        }
    }
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
