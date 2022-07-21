/**
 *  PIT section header of rockOS
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

#include <stdint.h>

#define SUBTICKS_PER_TICK 100

#define PIT_0 0x40
#define PIT_1 0x41
#define PIT_2 0x42
#define PIT_CONTROL 0x43

#define PIT_MASK 0xFF
#define PIT_SCALE 1193180
#define PIT_SET 0x36

void timer_phase(uint32_t hz);
void timer_handler(void* frame);
uint64_t get_ticks();