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

static void setup()
{
    // Erase console
    printf("\f");
    // Erase graphics
    RIA.addr0 = 0;
    RIA.step0 = 1;
    for (unsigned i = 320u * 180 / 8 / 8; i--;)
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
    xregn(1, 0, 0, 1, 1);            // 320x240
    xregn(1, 0, 1, 3, 3, 8, 0xFF00); // 1bpp lsb
    xregn(1, 0, 1, 2, 0, 1);         // console overlay

    // Program direct keyboard input
    xregn(0, 0, 0, 1, 0xFF10);

    // Hi
    printf("Conway's Game of Life");
}

void waitkey()
{
    RIA.addr0 = 0xFF10;
    RIA.step0 = 0;
    while (!(RIA.rw0 & 1))
        ;
    while (RIA.rw0 & 1)
        ;
    printf("\n");
}

void gen0()
{
}

int main()
{
    setup();

    gen0();

    printf("\nPress any key to exit");
    waitkey();
}
