/**
 *  VFS section header of rockOS
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

#ifndef _ROCKOS_VFS_H
#define _ROCKOS_VFS_H

#define FS_FILE 1
#define FS_DIR 2
#define FS_CHARDEV 3
#define FS_BLOCKDEV 4
#define FS_PIPE 5
#define FS_SYMLINK 6
#define FS_MOUNTPOINT 8

#include <stdint.h>

#define F_REGULAR   0
#define F_NORMAL    '0'
#define F_HLINK     '1'
#define F_SYMLINK   '2'
#define F_CHARDEV   '3'
#define F_BLKDEV    '4'
#define F_DIR       '5'
#define F_FIFO      '6'
#define F_CONT      '7'

typedef struct {
    char filename[100]; // Filename
    char mode[8];       // File mode
    char uid[8];        // User ID
    char gid[8];        // Group ID
    char size[12];      // File size in bytes in octal string
    char mtime[12];     // Modified time in UNIX Timestamp in octal string
    char chksum[8];     // Checksum
    char typeflag[1];   // File type
    char linkname[100]; // Name of linked file
    char magic[8];      // "ustar\000"
} TarHeader;

typedef struct {
    TarHeader* header;
    uint32_t* data;
} Tar;

uint32_t TarGetSize(const char*);

uint32_t TarParse(uint32_t);

#endif