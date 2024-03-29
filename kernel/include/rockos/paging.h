/**
 *  Paging section header of rockOS
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

#ifndef _ROCKOS_PAGING_H
#define _ROCKOS_PAGING_H

#include <stdint.h>

typedef struct {
   uint32_t present: 1;
   uint32_t rw: 1;
   uint32_t user: 1;
   uint32_t accessed: 1;
   uint32_t dirty: 1;
   uint32_t unused: 7;
   uint32_t frame: 20;
} Page;

typedef struct {
   Page pages[1024];
} PageTable;

typedef struct {
   PageTable* tables[1024];
   uint32_t tablesPhys[1024];
   uint32_t physAddr;
} PageDirectory;

void paging_init();
void paging_switch_dir(PageDirectory*);
Page* PageGet(uint32_t, int32_t, PageDirectory*);
void FrameSet(uint32_t);
void FrameClear(uint32_t);
uint32_t FrameTest(uint32_t);
uint32_t FrameFirst();
void FrameFree(Page*);
void FrameAlloc(Page*, char, char);

#endif