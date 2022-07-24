/**
 *  Sorted array data structure of rockOS
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

#include <rockos/data_struct/sorted_array.h>
#include <rockos/rockos.h>
#include <rockos/kheap.h>
#include <string.h>

char less_than(void* a, void* b) {
    return a < b;
}

SortedArray SortedArrayCreate(uint32_t size, LessThan less) {
    SortedArray myArray = {
        .data = (void*)kmalloc(size * sizeof(void*)),
        .size = 0,
        .max_size = size,
        .less_than = less,
    };
    memset(myArray.data, 0, size * sizeof(void*));
    return myArray;
}

SortedArray SortedArrayPlace(void* address, uint32_t size, LessThan less) {
    SortedArray myArray = {
        .data = (void**)address,
        .size = 0,
        .max_size = size,
        .less_than = less,
    };
    memset(myArray.data, 0, size * sizeof(void*));
    return myArray;
}

void SortedArrayDestroy(SortedArray* myArray) {
    kfree(myArray->data);
}

void SortedArrayInsert(void* element, SortedArray* myArray) {
    assert((uint32_t)myArray->less_than);
    uint32_t iter = 0;
    while(iter < myArray->size && myArray->less_than(myArray->data[iter], element)) {
        ++iter;
    }
    if(myArray->size == iter) {
        myArray->data[myArray->size++] = element;
    } else {
        void* temp = myArray->data[iter];
        myArray->data[iter] = element;
        while(iter < myArray->size) {
            ++iter;
            void* temp2 = myArray->data[iter];
            myArray->data[iter] = temp;
            temp = temp2;
        }
        myArray->size++;
    }
}

void* SortedArraySearch(uint32_t index, SortedArray* myArray) {
    assert(index < myArray->size);
    return myArray->data[index];
}

void SortedArrayDelete(uint32_t index, SortedArray* myArray) {
    assert(index < myArray->size);
    while (index < myArray->size) {
        myArray->data[index] = myArray->data[index + 1];
        ++index;
    }
    myArray->size--;
}