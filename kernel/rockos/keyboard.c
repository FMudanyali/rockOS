/**
 *  Keyboard driver of rockOS
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

#include <rockos/keyboard.h>
#include <rockos/pic.h>
#include <rockos/timer.h>
#include <rockos/hal.h>
#include <string.h>	

#define ESC_P 0x01
#define BACKSPACE_P 0x0F
#define ENTER_P 0x1C
#define LEFT_C_P 0x1D
#define LEFT_SHIFT_P 0x2A
#define RIGHT_SHIFT_P 0x36
#define LEFT_ALT_P 0x38

#define CAPS_LOCK_P 0x3A
#define CAPS_LOCK_R 0xBA

static unsigned char interrupt_key;
static uint8_t keyset[89];
static uint64_t keytimes[89];

static unsigned char keymap[89] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08,
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0
};

static unsigned char keymapshift[89] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0x08,
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '`', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0
};


void initialize_keyset() {
    memset(keyset, 0, 89);
    memset(keytimes, 0, 89);
    memset(&keymap[59], 0, 89 - 59);
    memset(&keymapshift[59], 0, 89 - 59);
}

uint8_t caps_lock = 0;

__attribute__((interrupt)) void keyboard_handler(__attribute__((unused)) void* frame) {
    interrupt_key = inb(0x60);
    pic_eoi(1);
    if((interrupt_key & 128) == 128) {
        if(interrupt_key == CAPS_LOCK_R)
            caps_lock = !caps_lock;
        keyset[interrupt_key - 0x80] = 0;
    } else {
        keyset[interrupt_key] = 1;
    }
}


unsigned char readkey() {
    if((interrupt_key & 128) != 128) {
        unsigned char tempkey = interrupt_key;
        interrupt_key = 0;
        if ((!caps_lock && (keyset[LEFT_SHIFT_P] || keyset[RIGHT_SHIFT_P])) ||
            (caps_lock && !keyset[LEFT_SHIFT_P] && !keyset[RIGHT_SHIFT_P])) {
            return keymapshift[tempkey];
        } else {
            return keymap[tempkey];
        }
    }
    return 0;
}