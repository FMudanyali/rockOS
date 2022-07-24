/**
 *  TTY section header of rockOS
 *  Copyright (C) 2022 Furkan Mudanyali
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, _either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <rockos/tty.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;
static uint16_t* const VGA_OLDMEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;
static uint16_t* terminal_oldbuffer;

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = VGA_MEMORY;
    terminal_oldbuffer = VGA_OLDMEMORY;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void scroll_screen_fwd() {
    memcpy(terminal_oldbuffer, &terminal_buffer[VGA_WIDTH], (VGA_HEIGHT*2-1) * VGA_WIDTH);
    for(size_t x = 0; x < VGA_WIDTH; ++x) {
        terminal_oldbuffer[(VGA_HEIGHT-1)*VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
    memcpy(terminal_buffer, terminal_oldbuffer, VGA_HEIGHT*VGA_WIDTH);
    --terminal_row;
}

void terminal_putchar(char c) {
    unsigned char uc = c;

    switch(uc) {
        case '\n':
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT)
                scroll_screen_fwd();
            break;
        case 0x08:
            if(!terminal_column & !terminal_row)
                return;
            if(--terminal_column == 0) {
                terminal_column = VGA_WIDTH;
                if(--terminal_row == 0) {
                    terminal_row =VGA_HEIGHT;
                }
            }
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
            break;
        default:
            terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
            if (++terminal_column == VGA_WIDTH) {
                terminal_column = 0;
                if (++terminal_row == VGA_HEIGHT) {
                    scroll_screen_fwd();
                }
            }
            break;
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}
