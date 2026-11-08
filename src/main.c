#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include <conio.h>

#include "chip.h"

#define PERIOD (1.0 / 60.0)

int main(void)
{
    struct Chip chip = chip_new();
    chip_load_rom(&chip, "../../ibm.ch8");

    LARGE_INTEGER freq, prev, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&prev);

    printf("\033[?25l");
    fflush(stdout);

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
                chip.keypress = 0x01;
            if (c == '2')
                chip.keypress = 0x02;
            if (c == '3')
                chip.keypress = 0x03;
            if (c == '4')
                chip.keypress = 0x0C;
            if (c == 'q')
                chip.keypress = 0x04;
            if (c == 'w')
                chip.keypress = 0x05;
            if (c == 'e')
                chip.keypress = 0x06;
            if (c == 'r')
                chip.keypress = 0x0D;
            if (c == 'a')
                chip.keypress = 0x07;
            if (c == 's')
                chip.keypress = 0x08;
            if (c == 'd')
                chip.keypress = 0x09;
            if (c == 'f')
                chip.keypress = 0x0E;
            if (c == 'z')
                chip.keypress = 0x0A;
            if (c == 'x')
                chip.keypress = 0x00;
            if (c == 'c')
                chip.keypress = 0x0B;
            if (c == 'v')
                chip.keypress = 0x0F;
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

    printf("\033[?25h");
    fflush(stdout);
    return 0;
}