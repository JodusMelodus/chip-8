#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#include "chip.h"

#define PERIOD (1.0 / 60.0)
// #define DEBUG

int main(void)
{
    struct Chip chip = chip_new();
    chip_load_rom(&chip, "../../roms/5-quirks.ch8");

#ifndef DEBUG
    printf("\033[?25l");
    fflush(stdout);
#endif

    clock_t start = clock(), end = clock();
    double delta_time;
    double accumulated = 0.0;
    int running = 1;

    while (running)
    {
        end = clock();
        delta_time = (double)(end - start) / CLOCKS_PER_SEC;
        start = clock();
        accumulated += delta_time;

        chip.keypress = 0x00;
        while (_kbhit())
        {
            int c = tolower(_getch());
            if (c == 27)
            {
                running = 0;
                break;
            }

            if (c == '1')
                chip.keypress |= 0xF1;
            if (c == '2')
                chip.keypress |= 0xF2;
            if (c == '3')
                chip.keypress |= 0xF3;
            if (c == '4')
                chip.keypress |= 0xFC;
            if (c == 'q')
                chip.keypress |= 0xF4;
            if (c == 'w')
                chip.keypress |= 0xF5;
            if (c == 'e')
                chip.keypress |= 0xF6;
            if (c == 'r')
                chip.keypress |= 0xFD;
            if (c == 'a')
                chip.keypress |= 0xF7;
            if (c == 's')
                chip.keypress |= 0xF8;
            if (c == 'd')
                chip.keypress |= 0xF9;
            if (c == 'f')
                chip.keypress |= 0xFE;
            if (c == 'z')
                chip.keypress |= 0xFA;
            if (c == 'x')
                chip.keypress |= 0xF0;
            if (c == 'c')
                chip.keypress |= 0xFB;
            if (c == 'v')
                chip.keypress |= 0xFF;
        }

        while (accumulated >= PERIOD)
        {
            accumulated -= PERIOD;
            chip_tick(&chip);
        }

#ifndef DEBUG
        chip_draw_frame_buffer(&chip);
        printf("sp=%d\n", chip.sp);
        printf("kp=%X\n", chip.keypress);
        printf("Regiters:\n");
        for (uint8_t i = 0; i < 16; i++)
            printf("V%X: 0x%X | ", i, chip.v[i]);
        printf("\n");
#endif
        Sleep(1);
    }

#ifndef DEBUG
    printf("\033[?25h");
    fflush(stdout);
#endif
    return 0;
}