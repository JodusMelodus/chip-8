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
            if (c == 'a')
                chip_stack_push(&chip, 5);
        }

        while (accumulated >= PERIOD)
        {
            accumulated -= PERIOD;
            chip_tick(&chip);
        }

        chip_draw_frame_buffer(&chip);
        printf("sp=%d   \n", chip.sp);

        Sleep(1);
    }

    return 0;
}