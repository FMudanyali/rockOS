/**
 *  putchar implementation of rockOS
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

#include <stdio.h>

#if defined(__is_libk)
#include <rockos/tty.h>
#endif

int putchar(int ic) {
#if defined(__is_libk)
    char c = (char) ic;
    terminal_write(&c, sizeof(c));
#else
    // TODO: Implement stdio and the write system call.
#endif
    return ic;
}
