/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <rp6502.h>

// Obligatory Example
// https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

// RAM screen buffer
uint8_t buf[320 / 8 * 180];

static void setup()
{
    // Erase console
    printf("\f");
    // Erase graphics
    RIA.addr0 = 0;
    RIA.step0 = 1;
    for (unsigned i = sizeof(buf); i--;)
        RIA.rw0 = 0;

    // Configure graphics mode
    xram0_struct_set(0xFF00, vga_mode3_config_t, x_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, y_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, width_px, 320);
    xram0_struct_set(0xFF00, vga_mode3_config_t, height_px, 180);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_data_ptr, 0x0000);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_palette_ptr, 0xFFFF);

    // Program video system
    xregn(1, 0, 0, 1, 2);            // 320x240
    xregn(1, 0, 1, 3, 3, 0, 0xFF00); // 1bpp
    xregn(1, 0, 1, 2, 0, 1);         // console overlay
}

static inline void set(int x, int y, bool state)
{
    RIA.addr0 = (x / 8) + (320 / 8 * y);
    RIA.step0 = 0;
    uint8_t bit = 128 >> (x % 8);
    if (state)
        RIA.rw0 |= bit;
    else
        RIA.rw0 &= ~bit;
}

static inline uint8_t get(int x, int y)
{
    unsigned addr = (x / 8) + (320 / 8 * y);
    uint8_t bit = 128 >> (x % 8);
    return buf[addr] & bit ? 1 : 0;
}

static void next()
{
    // Copy screen to memory buffer
    RIA.addr0 = 0;
    RIA.step0 = 1;
    for (unsigned i = 0; i < sizeof(buf); i++)
        buf[i] = RIA.rw0;

    // Process all pixels excluding the edges
    for (int x = 1; x < 319; x++)
    {
        for (int y = 1; y < 179; y++)
        {
            uint8_t neighbors =
                get(x - 1, y - 1) +
                get(x, y - 1) +
                get(x + 1, y - 1) +
                get(x - 1, y) +
                get(x + 1, y) +
                get(x - 1, y + 1) +
                get(x, y + 1) +
                get(x + 1, y + 1);

            if (get(x, y))
            { // alive
                if (neighbors != 2 && neighbors != 3)
                    set(x, y, false);
            }
            else
            { // dead
                if (neighbors == 3)
                    set(x, y, true);
            }
        }
    }
}

// Glider moves right and down
static void glider()
{
    set(11, 20, true);
    set(12, 21, true);
    set(10, 22, true);
    set(11, 22, true);
    set(12, 22, true);
}

// Light-weight spaceship moves right
static void lwss()
{
    set(20, 20, true);
    set(23, 20, true);
    set(24, 21, true);
    set(20, 22, true);
    set(24, 22, true);
    set(21, 23, true);
    set(22, 23, true);
    set(23, 23, true);
    set(24, 23, true);
}

int main()
{
    setup();

    // Hi
    printf("Conway's Game of Life");

    // Starting generation
    glider();
    lwss();

    // Iterate generations
    for (int i = 0; i < 500; i++)
        next();

    // Done
    printf("\nPress any key to exit");
    xregn(0, 0, 0, 1, 0xFF10);
    RIA.addr0 = 0xFF10;
    RIA.step0 = 0;
    while (RIA.rw0 & 1)
        ;
    printf("\n");
}
