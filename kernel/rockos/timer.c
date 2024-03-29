/**
 *  PIT section of rockOS
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

#include <rockos/timer.h>
#include <rockos/hal.h>
#include <rockos/pic.h>
#include <stdio.h>

uint64_t ticks = 0;
uint8_t subticks = 0;

__attribute__((interrupt)) void timer_handler(__attribute__((unused)) void* frame) {
    if(++subticks == SUBTICKS_PER_TICK) {
        ++ticks;
        subticks = 0;
    }
    pic_eoi(0);
}

void timer_phase(uint32_t hz) {
    uint32_t divisor = PIT_SCALE / hz;
    outb(PIT_CONTROL, PIT_SET);
    outb(PIT_0, divisor & PIT_MASK);
    outb(PIT_0, (divisor >> 8) & PIT_MASK);
}

uint64_t get_ticks() {
    return ticks*100 + subticks;
}