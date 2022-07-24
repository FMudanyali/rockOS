/**
 *  KHeap section header of rockOS
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

#ifndef _ROCKOS_KHEAP_H
#define _ROCKOS_KHEAP_H

#include <stdint.h>
#include <rockos/data_struct/sorted_array.h>

#define KHEAP_START 0xC0000000
#define KHEAP_INITIAL_SIZE 0x100000
#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC 0x06163474
#define HEAP_MIN_SIZE 0x70000

typedef struct {
   uint32_t magic;
   uint8_t hole;
   uint32_t size;
} Header;

typedef struct {
   uint32_t magic;
   Header* header;
} Footer;

typedef struct {
   SortedArray index;
   uint32_t addr_start;
   uint32_t addr_end;
   uint32_t addr_max;
   uint8_t supervisor;
   uint8_t readonly;
} Heap;

Heap* HeapCreate(uint32_t, uint32_t, uint32_t, uint8_t, uint8_t);
void* alloc(uint32_t, uint8_t, Heap*);
void free(void*, Heap*);
uint32_t kmalloc_int(uint32_t, uint8_t, uint32_t*);
uint32_t kmalloc(uint32_t);
void kfree(void*);

#endif