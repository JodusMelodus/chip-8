#include <stdio.h>
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

    while (1)
    {
        QueryPerformanceCounter(&now);
        accumulated += (double)(now.QuadPart - prev.QuadPart) / freq.QuadPart;
        prev = now;

        while (accumulated >= PERIOD)
        {
            accumulated -= PERIOD;
            chip_tick(&chip);
            chip_draw_frame_buffer(&chip);
        }

        if (_kbhit())
        {
            int c = _getch();
            if (c == 27)
                break; /* Esc to quit */
        }

        Sleep(1);
    }

    return 0;
}