/**
 *  Paging section of rockOS
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

#include <rockos/paging.h>
#include <rockos/kheap.h>
#include <rockos/multiboot.h>
#include <rockos/rockos.h>
#include <string.h>

#define INDEX_FROM_BIT(x) (x/(8*4))
#define OFFSET_FROM_BIT(x) (x%(8*4))

PageDirectory* kernel_dir = 0;
PageDirectory* current_dir = 0;

uint32_t* frames;
uint32_t nframes;

extern uint32_t start_addr;
extern multiboot_info_t* multiboot_info;
extern Heap* kheap;

void FrameSet(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    frames[INDEX_FROM_BIT(frame)] |= (1 << OFFSET_FROM_BIT(frame));
}

void FrameClear(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    frames[INDEX_FROM_BIT(frame)] &= ~(1 << OFFSET_FROM_BIT(frame));
}

uint32_t FrameTest(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    return (frames[INDEX_FROM_BIT(frame)] & (1 << OFFSET_FROM_BIT(frame)));
}

uint32_t FrameFirst() {
    for(uint32_t i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
        if(frames[i] != 0xFFFFFFFF) {
            for(uint32_t j = 0; j < 32; ++j) {
                uint32_t test = 1 << j;
                if(!(frames[i]&test)) {
                    return i*4*8 + j;
                }
            }
        }
    }
    return 0;
}

void FrameAlloc(Page* page, char kernel, char writeable) {
    if (page->frame) {
        return;
    }
    uint32_t i = FrameFirst();
    if(i == (uint32_t)-1) {
        panic("NO FREE FRAMES LEFT!");
    }
    FrameSet(i * 0x1000);
    page->present = 1;
    page->rw = writeable;
    page->user = !kernel;
    page->frame = i;
}

void FrameFree(Page* page) {
    uint32_t frame;
    if(!(frame = page->frame)) {
        return;
    }
    FrameClear(frame);
    page->frame = 0;
}

Page* PageGet(uint32_t addr, int32_t make, PageDirectory* dir) {
    addr /= 0x1000;
    uint32_t table_i = addr / 1024;
    if(dir->tables[table_i]) {
        return &dir->tables[table_i]->pages[addr%1024];
    }

    if(make) {
        uint32_t temp;
        dir->tables[table_i] = (PageTable*)kmalloc_int(sizeof(PageTable), 1, &temp);
        memset(dir->tables[table_i], 0, 0x1000);
        dir->tablesPhys[table_i] = temp | 0x7;
        return &dir->tables[table_i]->pages[addr%1024];
    }

    return 0;
}

void paging_switch_dir(PageDirectory* dir) {
    current_dir = dir;
    asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhys));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void paging_init() {
    start_addr = *(uint32_t*)(multiboot_info->mods_addr + 4);
    printf("Initializing paging.\n");
    uint32_t page_end_mem = 0x1000000;
    nframes = page_end_mem / 0x1000;

    frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));

    kernel_dir = (PageDirectory*)kmalloc_int(sizeof(PageDirectory), 1, 0);
    memset(kernel_dir, 0, sizeof(PageDirectory));
    current_dir = kernel_dir;

    for(uint32_t i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000) {
        PageGet(i, 1, kernel_dir);
    }

    for(uint32_t i = 0; i < start_addr + 0x1000; i += 0x1000) {
        FrameAlloc(PageGet(i, 1, kernel_dir), 0, 0);
    }

    for(uint32_t i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000) {
        FrameAlloc(PageGet(i, 1, kernel_dir), 0, 0);
    }

    paging_switch_dir(kernel_dir);
    kheap = HeapCreate(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);
}