#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#include "chip.h"

#define PERIOD (1.0 / 60.0)
#define INSTRUCTIONS_PER_FRAME 100
// #define DEBUG

uint8_t handleInput(uint16_t *keypress)
{
    uint16_t keys = 0x00;

    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        return 0;
    if (GetAsyncKeyState('1') & 0x8000)
        keys |= 1 << 0x01;
    if (GetAsyncKeyState('2') & 0x8000)
        keys |= 1 << 0x02;
    if (GetAsyncKeyState('3') & 0x8000)
        keys |= 1 << 0x03;
    if (GetAsyncKeyState('4') & 0x8000)
        keys |= 1 << 0x0C;
    if (GetAsyncKeyState('Q') & 0x8000)
        keys |= 1 << 0x04;
    if (GetAsyncKeyState('W') & 0x8000)
        keys |= 1 << 0x05;
    if (GetAsyncKeyState('E') & 0x8000)
        keys |= 1 << 0x06;
    if (GetAsyncKeyState('R') & 0x8000)
        keys |= 1 << 0x0D;
    if (GetAsyncKeyState('A') & 0x8000)
        keys |= 1 << 0x07;
    if (GetAsyncKeyState('S') & 0x8000)
        keys |= 1 << 0x08;
    if (GetAsyncKeyState('D') & 0x8000)
        keys |= 1 << 0x09;
    if (GetAsyncKeyState('F') & 0x8000)
        keys |= 1 << 0x0E;
    if (GetAsyncKeyState('Z') & 0x8000)
        keys |= 1 << 0x0A;
    if (GetAsyncKeyState('X') & 0x8000)
        keys |= 1 << 0x00;
    if (GetAsyncKeyState('C') & 0x8000)
        keys |= 1 << 0x0B;
    if (GetAsyncKeyState('V') & 0x8000)
        keys |= 1 << 0x0F;

    *keypress = keys;
    return 1;
}

int main(void)
{
    struct Chip chip = chip_new();
    chip_load_rom(&chip, "../../roms/6-keypad.ch8");

#ifndef DEBUG
    printf("\033[?25l");
    fflush(stdout);
#endif

    clock_t start = clock();
    int running = 1;

    while (running)
    {
        start = clock();
        chip.keypress = 0x00;
        running = handleInput(&chip.keypress);

        chip.vertical_blank = 1;
        for (size_t i = 0; i < INSTRUCTIONS_PER_FRAME; i++)
            chip_tick(&chip);

        chip_tick_timer(&chip);

#ifndef DEBUG
        chip_draw_frame_buffer(&chip);
        printf("sp=%d\n", chip.sp);
        printf("kp=%X\n", chip.keypress);
        printf("Regiters:\n");
        for (uint8_t i = 0; i < 16; i++)
            printf("V%X: 0x%X | ", i, chip.v[i]);
        printf("\n");
#endif
        // wait til frame passed
        while ((double)(clock() - start) / CLOCKS_PER_SEC <= PERIOD)
            ;
    }

#ifndef DEBUG
    printf("\033[?25h");
    fflush(stdout);
#endif
    return 0;
}