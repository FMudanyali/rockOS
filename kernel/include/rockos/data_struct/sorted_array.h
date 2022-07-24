/**
 *  Sorted array data structure header of rockOS
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

#ifndef _ROCKOS_ORDERED_ARRAY_H
#define _ROCKOS_ORDERED_ARRAY_H

#include <stdint.h>

typedef char (*LessThan)(void*, void*);

typedef struct {
    void** data;
    uint32_t size;
    uint32_t max_size;
    LessThan less_than;
} SortedArray;

char less_than(void*, void*);

SortedArray SortedArrayCreate(uint32_t, LessThan);
SortedArray SortedArrayPlace(void*, uint32_t, LessThan);
void SortedArrayDestroy(SortedArray*);
void SortedArrayInsert(void*, SortedArray*);
void* SortedArraySearch(uint32_t, SortedArray*);
void SortedArrayDelete(uint32_t, SortedArray*);

#endif