/**
 *  KHeap section of rockOS
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

#include <rockos/kheap.h>
#include <rockos/paging.h>
#include <rockos/rockos.h>
#include <rockos/data_struct/sorted_array.h>

extern char _rockos_end;
uint32_t start_addr = (uint32_t)&_rockos_end;
extern PageDirectory* kernel_dir;
Heap* kheap = 0;

static int32_t smallest_hole(uint32_t size, uint8_t aligned, Heap* heap) {
    uint32_t i = 0;
    while(i < heap->index.size) {
        Header* header = (Header*)SortedArraySearch(i, &heap->index);
        if(aligned) {
            uint32_t location = (uint32_t)header;
            uint32_t offset = 0;
            if((location + sizeof(Header) & ~0xFFF) != 0) {
                offset = 0x1000 - (location + sizeof(Header)) % 0x1000;
            }
            uint32_t hole_size = (uint32_t)header->size - offset;
            if(hole_size >= (uint32_t)size) {
                break;
            }
        } else if (header->size >= size) {
            break;
        }
        i++;
    }
    if(i == heap->index.size) {
        return -1;
    }
    return i;
}

static void expand(uint32_t size, Heap* heap) {
    assert(size > heap->addr_end - heap->addr_start);
    if((size & ~0xFFF) != 0) {
        size &= ~0xFFF;
        size += 0x1000;
    }
    assert(heap->addr_start + size <= heap->addr_max);
    uint32_t size_old = heap->addr_end - heap->addr_start;
    for(uint32_t i = size_old; i < size; i+= 0x1000) {
        FrameAlloc( PageGet(heap->addr_start + i, 1, kernel_dir), heap->supervisor, heap->readonly );
    }
    heap->addr_end = heap->addr_start + size;
}

static uint32_t shrink(uint32_t size, Heap* heap) {
    assert(size < heap->addr_end - heap->addr_start);
    if(size & 0x1000) {
        size &= 0x1000;
        size += 0x1000;
    }
    if(size < HEAP_MIN_SIZE) {
        size = HEAP_MIN_SIZE;
    }

    uint32_t size_old = heap->addr_end - heap->addr_start;
    for(uint32_t i = size_old - 0x1000; size < i; i -= 0x1000) {
        FrameFree(PageGet(heap->addr_start + i, 0, kernel_dir));
    }

    heap->addr_end = heap->addr_start + size;
    return size;
}

void* alloc(uint32_t size, uint8_t aligned, Heap* heap) {
    uint32_t size_new = size + sizeof(Header) + sizeof(Footer);
    int32_t iter = smallest_hole(size_new, aligned, heap);
    if(iter == -1) {
        uint32_t len_old = heap->addr_end - heap->addr_start;
        uint32_t end_old_addr = heap->addr_end;
        expand(len_old + size_new, heap);
        uint32_t len_new = heap->addr_end - heap->addr_start;
        iter = 0;
        uint32_t idx = -1;
        uint32_t val = 0;
        while((uint32_t)iter < heap->index.size) {
            uint32_t temp = (uint32_t)SortedArraySearch(iter, &heap->index);
            if (temp > val) {
                val = temp;
                idx = iter;
            }
            ++iter;
        }

        if(idx == -1) {
            Header* header = (Header*)end_old_addr;
            header->magic = HEAP_MAGIC;
            header->size = len_new - len_old;
            header->hole = 1;
            Footer* footer = (Footer*)(end_old_addr + header->size - sizeof(Footer));
            footer->magic = HEAP_MAGIC;
            footer->header = header;
            SortedArrayInsert((void*)header, &heap->index);
        } else {
            Header* header = SortedArraySearch(idx, &heap->index);
            header->size += len_new - len_old;
            Footer* footer = (Footer*)((uint32_t)header + header->size - sizeof(Footer));
            footer->magic = HEAP_MAGIC;
            footer->header = header;
        }
        return alloc(size, aligned, heap);
    }

    Header* hole_header_old = (Header*)SortedArraySearch(iter, &heap->index);
    uint32_t hole_pos_old = (uint32_t)hole_header_old;
    uint32_t hole_size_old = hole_header_old->size;

    if(hole_size_old - size_new < sizeof(Header) + sizeof(Footer)) {
        size += hole_size_old - size_new;
        size_new = hole_size_old;
    }

    if(aligned && (hole_pos_old & ~0xFFF)) {
        uint32_t location_new = hole_pos_old + 0x1000 - (hole_pos_old & 0xFFF) - sizeof(Header);
        Header* hole_header = (Header*) hole_pos_old;
        hole_header->size = 0x1000 - (hole_pos_old & 0xFFF) - sizeof(Header);
        hole_header->magic = HEAP_MAGIC;
        hole_header->hole = 1;
        Footer* hole_footer = (Footer*)((uint32_t)location_new - sizeof(Footer));
        hole_footer->magic = HEAP_MAGIC;
        hole_footer->header = hole_header;
        hole_pos_old = location_new;
        hole_size_old -= hole_header->size;
    } else {
        SortedArrayDelete(iter, &heap->index);
    }

    Header* block_header = (Header*)hole_pos_old;
    block_header->magic = HEAP_MAGIC;
    block_header->hole = 0;
    block_header->size = size_new;
    Footer* block_footer = (Footer*)(hole_pos_old + sizeof(Header) + size);
    block_footer->magic = HEAP_MAGIC;
    block_footer->header = block_header;

    if(hole_size_old - size_new) {
        Header* hole_header = (Header*)(hole_pos_old + sizeof(Header) + size + sizeof(Footer));
        hole_header->magic = HEAP_MAGIC;
        hole_header->hole = 1;
        hole_header->size = hole_size_old - size_new;
        Footer* hole_footer = (Footer*)((uint32_t)hole_header + hole_size_old - size_new - sizeof(Footer));
        if ((uint32_t)hole_footer < heap->addr_end) {
            hole_footer->magic = HEAP_MAGIC;
            hole_footer->header = hole_header;
        }
        SortedArrayInsert((void*)hole_header, &heap->index);
    }
    return (void*)((uint32_t)block_header + sizeof(Header));
}

void free(void* pointer, Heap* heap) {
    if(pointer == 0)
        return;
    
    Header* header = (Header*)((uint32_t)pointer - sizeof(Header));
    Footer* footer = (Footer*)((uint32_t)header + header->size - sizeof(Footer));

    assert(header->magic == HEAP_MAGIC);
    assert(footer->magic == HEAP_MAGIC);

    header->hole = 1;
    char add = 1;

    Footer* temp_footer = (Footer*)((uint32_t)header - sizeof(Footer));
    if(temp_footer->magic == HEAP_MAGIC && temp_footer->header->hole) {
        uint32_t cache_size = header->size;
        header = temp_footer->header;
        footer->header = header;
        header->size += cache_size;
        add = 0;
    }

    Header* temp_header = (Header*)((uint32_t)footer + sizeof(footer));
    if(temp_header->magic == HEAP_MAGIC && temp_header->hole) {
        header->size += temp_header->size;
        temp_footer = (Footer*)((uint32_t)temp_header + temp_header->size - sizeof(Footer));
        footer = temp_footer;
        uint32_t iter = 0;
        while((iter < heap->index.size) && (SortedArraySearch(iter, &heap->index) != (void*)temp_header)) {
            ++iter;
        }
        assert(iter < heap->index.size);
        SortedArrayDelete(iter, &heap->index);
    }

    if((uint32_t)footer + sizeof(Footer) == heap->addr_end) {
        uint32_t len_old = heap->addr_end - heap->addr_start;
        uint32_t len_new = shrink((uint32_t)header - heap->addr_start, heap);
        if(header->size - (len_old - len_new) > 0) {
            header->size -= len_old - len_new;
            footer = (Footer*)((uint32_t)header + header->size - sizeof(Footer));
            footer->magic = HEAP_MAGIC;
            footer->header = header;
        } else {
            uint32_t iter = 0;
            while((iter < heap->index.size) && (SortedArraySearch(iter, &heap->index) != (void*)temp_header)) {
                ++iter;
            }
            if(iter < heap->index.size) {
                SortedArrayDelete(iter, &heap->index);
            }
        }
    }

    if(add) {
        SortedArrayInsert((void*)header, &heap->index);
    }
}

uint32_t kmalloc_int(uint32_t size, uint8_t aligned, uint32_t* phys_addr) {
    if(kheap) {
        void* addr = alloc(size, aligned, kheap);
        if (phys_addr) {
            Page* page = PageGet((uint32_t)addr, 0, kernel_dir);
            *phys_addr = (page->frame * 0x1000) + ((uint32_t)addr & 0xFFF);
        }
        return (uint32_t)addr;
    }
    if(aligned && (start_addr & 0xFFF)) {
        start_addr &= ~0xFFF;
        start_addr += 0x1000;
    }
    if(phys_addr) {
        *phys_addr = start_addr;
    }
    uint32_t temp = start_addr;
    start_addr += size;
    return temp;
}

uint32_t kmalloc(uint32_t size) {
    return kmalloc_int(size, 0, 0);
}

void kfree(void* pointer) {
    free(pointer, kheap);
}

static char HeaderLessThan(void* a, void* b) {
    return ((Header*)a)->size < ((Header*)b)->size;
}

Heap* HeapCreate(uint32_t start, uint32_t end, uint32_t max, uint8_t supervisor, uint8_t readonly) {
    Heap* heap = (Heap*)kmalloc(sizeof(Heap));

    assert(start % 0x1000 == 0);
    assert(end % 0x1000 == 0);

    heap->index = SortedArrayPlace((void*)start, HEAP_INDEX_SIZE, &HeaderLessThan);
    start += sizeof(void*) * HEAP_INDEX_SIZE;

    if(start & ~0xFFF) {
        start &= ~0xFFF;
        start += 0x1000;
    }

    heap->addr_start = start;
    heap->addr_end = end;
    heap->addr_max = max;
    heap->supervisor = supervisor;
    heap->readonly = readonly;

    Header* hole = (Header*)start;
    hole->size = end - start;
    hole->magic = HEAP_MAGIC;
    hole->hole = 1;
    SortedArrayInsert((void*)hole, &heap->index);

    return heap;
}