/**
 *  VFS section of rockOS
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

#include <rockos/vfs.h>
#include <rockos/kheap.h>
#include <string.h>
#include <stdio.h>

extern Tar* files;

uint32_t TarGetSize(const char* in) {
    uint32_t size = 0;
    uint32_t count = 1;
    for(uint32_t i = 11; i > 0; i--, count *= 8) {
        size += ((in[i - 1] - '0') * count);
    }
    return size;
}

uint32_t TarParse(uint32_t addr) {
    uint32_t i;
    for(i = 0;; i++) {
        TarHeader* header = (TarHeader*)addr;

        if(memcmp("ustar", header->magic, 5) &&
            memcmp("ustar\0", header->magic, 6) &&
            memcmp("ustar\000", header->magic, 8)) {
            break;
        }

        uint32_t size = TarGetSize(header->size);
        files[i].header = addr;
        files[i].data = addr + 512;

        addr += ((size / 512) + 1) * 512;
        if(size % 512) {
            addr += 512;
        }
    }
    return i;
}