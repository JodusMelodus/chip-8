#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include <conio.h>

#include "chip.h"

#define PERIOD (1.0 / 60.0)

int main(void)
{
    struct Chip chip = chip_new();

    LARGE_INTEGER freq, prev, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&prev);

    double accumulated = 0.0;
    int running = 1;

    while (running)
    {
        QueryPerformanceCounter(&now);
        accumulated += (double)(now.QuadPart - prev.QuadPart) / freq.QuadPart;
        prev = now;
        if (accumulated > 0.25)
            accumulated = 0.25;

        while (_kbhit())
        {
            int c = tolower(_getch());
            if (c == 27)
            {
                running = 0;
                break;
            }

            if (c == '1')
                chip.keypress = 0x1;
            if (c == '2')
                chip.keypress = 0x2;
            if (c == '3')
                chip.keypress = 0x3;
            if (c == '4')
                chip.keypress = 0xC;
            if (c == 'q')
                chip.keypress = 0x4;
            if (c == 'w')
                chip.keypress = 0x5;
            if (c == 'e')
                chip.keypress = 0x6;
            if (c == 'r')
                chip.keypress = 0xD;
            if (c == 'a')
                chip.keypress = 0x7;
            if (c == 's')
                chip.keypress = 0x8;
            if (c == 'd')
                chip.keypress = 0x9;
            if (c == 'f')
                chip.keypress = 0xE;
            if (c == 'z')
                chip.keypress = 0xA;
            if (c == 'x')
                chip.keypress = 0x0;
            if (c == 'c')
                chip.keypress = 0xB;
            if (c == 'v')
                chip.keypress = 0xF;
        }

        while (accumulated >= PERIOD)
        {
            accumulated -= PERIOD;
            chip_tick(&chip);
        }

        chip_draw_frame_buffer(&chip);
        printf("sp=%d   \n", chip.sp);
        printf("kp=%X   \n", chip.keypress);

        Sleep(1);
    }

    return 0;
}