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

uint8_t buf[320 / 8 * 180];

static void setup()
{
    // Erase console
    printf("\f");
    // Erase graphics
    RIA.addr0 = 0;
    RIA.step0 = 1;
    for (unsigned i = sizeof(buf) / 8; i--;)
    {
        RIA.rw0 = 0;
        RIA.rw0 = 0;
        RIA.rw0 = 0;
        RIA.rw0 = 0;
        RIA.rw0 = 0;
        RIA.rw0 = 0;
        RIA.rw0 = 0;
        RIA.rw0 = 0;
    }

    // Configure graphics mode
    // xram0_struct_set(0xFF00, vga_mode3_config_t, x_wrap, true);
    // xram0_struct_set(0xFF00, vga_mode3_config_t, y_wrap, true);
    xram0_struct_set(0xFF00, vga_mode3_config_t, x_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, y_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, width_px, 320);
    xram0_struct_set(0xFF00, vga_mode3_config_t, height_px, 180);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_data_ptr, 0x0000);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_palette_ptr, 0xFFFF);

    // Program video system
    xregn(1, 0, 0, 1, 2);            // 320x240
    xregn(1, 0, 1, 3, 3, 8, 0xFF00); // 1bpp lsb
    xregn(1, 0, 1, 2, 0, 1);         // console overlay

    // Program direct keyboard input
    xregn(0, 0, 0, 1, 0xFF10);

    // Hi
    printf("Conway's Game of Life");
}

static void waitkey()
{
    RIA.addr0 = 0xFF10;
    RIA.step0 = 0;
    while (!(RIA.rw0 & 1))
        ;
    while (RIA.rw0 & 1)
        ;
    printf("\n");
}

static inline void set(int x, int y, bool state)
{
    RIA.addr0 = (x / 8) + (320 / 8 * y);
    RIA.step0 = 0;
    uint8_t bit = 1 << (x % 8);
    if (state)
        RIA.rw0 |= bit;
    else
        RIA.rw0 &= ~bit;
}

static inline uint8_t get(int x, int y)
{
    if (x < 0 || x >= 320 || y < 0 || y >= 180)
        return 0;
    unsigned addr = (x / 8) + (320 / 8 * y);
    uint8_t bit = 1 << (x % 8);
    return buf[addr] & bit ? 1 : 0;
}

static void next()
{
    RIA.addr0 = 0;
    RIA.step0 = 1;
    for (unsigned i = 0; i < sizeof(buf); i++)
        buf[i] = RIA.rw0;
    for (int x = 0; x < 320; x++)
    {
        for (int y = 0; y < 180; y++)
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

static void gen0()
{
    // glider
    set(11, 20, true);
    set(12, 21, true);
    set(10, 22, true);
    set(11, 22, true);
    set(12, 22, true);
}

int main()
{
    setup();

    gen0();

    for (int i = 0; i < 100; i++)
        next();

    printf("\nPress any key to exit");
    waitkey();
}
