#include "chip.h"

struct Chip chip_new()
{
    struct Chip chip = {0};

    // Popular font address
    for (uint16_t i = 0x050; i < 0x09F; i++)
    {
        chip.memory[i] = font[i - 0x050];
    }

    return chip;
}
