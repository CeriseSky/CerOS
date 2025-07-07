/*
    This file is part of the CerOS Bootloader & Bootloader Tools
    Copyright (C) 2025 Indigo Carmine (CeriseSky)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/param.h>

// DT_COPY_STATS can be used to create aliases for disk names
// (e.g. "MyFavouriteDisk" can have id DT_COPY_STATS and a copyName of
//       "5.25/IBM/PC_5150/160K" to copy its stats) and if you think that's
// useless then you're wrong!
typedef enum {
  DT_INVALID,
  DT_GENERIC_160K,  // any disk that is exactly 163840 bytes in size
  DT_GENERIC_320K,  // any disk that is exactly 327680 bytes in size
  DT_GENERIC_1440K,
  DT_GENERIC_720K,
  DT_COPY_STATS,
} DISK_TYPE;

// names should use the format <inches>/<brand>/.../<capacity>
typedef struct disk {
  char *name;
  DISK_TYPE id;
  char *copyName;  // used if id = DT_COPY_STATS to alias another disk type
} disk_t;

disk_t g_diskTypes[] = {
  { .name = "5.25/IBM/PC_5150/160K", .id = DT_GENERIC_160K },
  { .name = "5.25/IBM/PC_5150/320K", .id = DT_GENERIC_320K },
  { .name = "3.5/IBM/PC/720K", .id = DT_GENERIC_720K },
  { .name = "3.5/IBM/PS2_286/1.44M", .id = DT_GENERIC_1440K },
};
#define NUM_DISK_TYPES (sizeof(g_diskTypes)/sizeof(disk_t))

#define DEFAULT_RECURSION_LIMIT 10
#define BYTES_PER_KILOBYTE 1024

uint16_t get_sectors_per_fat12(uint16_t totalSectors, uint16_t bytesPerSector) {
  // not the most efficient algorithm because it takes into account the size of
  // the entire disk, rather than just the data section

  // I haven't put an explanation here because I plan to change the algorithm
  // at some point, and it's not too difficult to figure this out for yourself
  // if you're a fan of maths

  uint32_t numerator = 3 * totalSectors;
  uint32_t denominator = 2 * bytesPerSector;
  return numerator / denominator + (numerator % denominator ? 1 : 0);
}

// searches g_diskTypes for your disk and resolves DT_COPY_STATS links.
// Will stop resolving after recursion_limit links to prevent circular
// dependencies from causing infinite loops.
// set recursion_limit to be less than 0 if you want to disable it
DISK_TYPE get_disk_type(char *name, int recursion_limit) {
  if(recursion_limit <= 0)
    return DT_INVALID;

  for(int i = 0; i < NUM_DISK_TYPES; i++)
    if(!strcmp(name, g_diskTypes[i].name))
      return g_diskTypes[i].id != DT_COPY_STATS ?
        g_diskTypes[i].id : get_disk_type(g_diskTypes[i].copyName,
                                          recursion_limit - 1);

  return DT_INVALID;
}

enum DOS_MEDIA_TYPE {
  DOS_160K_FLOPPY = 0xfe,
  DOS_320K_FLOPPY = 0xff,
  DOS_1440K_FLOPPY = 0xf0,
  DOS_CUSTOM_FLOPPY = 0xf0,
};

int main(int argc, char **argv) {
  if(argc == 2 && !strcmp(argv[1], "help")) {
    fprintf(stderr, "Displaying all supported disk types:\n");
    for(int i = 0; i < NUM_DISK_TYPES; i++)
      fprintf(stderr, "  %s\n", g_diskTypes[i].name);
    return EXIT_SUCCESS;
  }

  if(argc < 3) {
    fprintf(stderr, "Format: genflags <type> <boot path> [label] [oem] [serial]\n");
    fprintf(stderr, "Use genflags help for a list of disk types\n");
    return EXIT_FAILURE;
  }

  uint32_t serialID = 0x43534b59;   // the characters "CSKY"
  char *label = "CeriseSkyOS";
  char *oem = "CSKYINDY";           // Me!
  if(argc >= 4)
    label = argv[3];
  if(argc >= 5)
    oem = argv[4];
  if(argc >= 6)
    serialID = strtoul(argv[5], NULL, 0);

  struct {
    uint16_t bytesPerSector;
    uint32_t totalSectors;
    uint16_t sectorsPerTrack;
    uint16_t totalHeads;
    uint8_t mediaType;
    uint16_t rootDirSectors;
  } diskInfo;

  switch(get_disk_type(argv[1], DEFAULT_RECURSION_LIMIT)) {
    case DT_GENERIC_160K:
      diskInfo.bytesPerSector = 512;
      diskInfo.totalSectors = 160 * BYTES_PER_KILOBYTE
                              / diskInfo.bytesPerSector;
      diskInfo.sectorsPerTrack = 8;
      diskInfo.totalHeads = 1;
      diskInfo.mediaType = DOS_160K_FLOPPY;
      diskInfo.rootDirSectors = 4;
      break;

    case DT_GENERIC_320K:
      diskInfo.bytesPerSector = 512;
      diskInfo.totalSectors = 320 * BYTES_PER_KILOBYTE /
                              diskInfo.bytesPerSector;
      diskInfo.sectorsPerTrack = 8;
      diskInfo.totalHeads = 2;
      diskInfo.mediaType = DOS_320K_FLOPPY;
      diskInfo.rootDirSectors = 8;
      break;

    case DT_GENERIC_720K:
      diskInfo.bytesPerSector = 512;
      diskInfo.totalSectors = 720 * BYTES_PER_KILOBYTE /
                              diskInfo.bytesPerSector;
      diskInfo.sectorsPerTrack = 9;
      diskInfo.totalHeads = 2;
      diskInfo.mediaType = DOS_CUSTOM_FLOPPY;
      diskInfo.rootDirSectors = 16;
      break;

    case DT_GENERIC_1440K:
      diskInfo.bytesPerSector = 512;
      diskInfo.totalSectors = 1440 * BYTES_PER_KILOBYTE /
                              diskInfo.bytesPerSector;
      diskInfo.sectorsPerTrack = 18;
      diskInfo.totalHeads = 2;
      diskInfo.mediaType = DOS_1440K_FLOPPY;
      diskInfo.rootDirSectors = 16;
      break;

    case DT_INVALID:
      fprintf(stderr, "Requested type is not supported.\n"
                      "Use genflags help for a list of supported disk types\n");
      return EXIT_FAILURE;
      break;

    default:
      fprintf(stderr, "Requested disk type does not have an associated size. "
                      "This is a bug so try to use a\ndifferent disk type.\n"
                      "Use genflags help for a list of supported disk types\n");
    return EXIT_FAILURE;
  }

  char *bootPath = argv[2];
  if(*bootPath == '/') bootPath++;    // saves space so beneficial even if
                                      // leading slash support is enabled

  char paddedLabel[11];
  char paddedOem[8];

  memset(paddedLabel, ' ', 11);
  memcpy(paddedLabel, label, MIN(strlen(label), 11));

  memset(paddedOem, ' ', 8);
  memcpy(paddedOem, oem, MIN(8, strlen(oem)));

  fprintf(stderr, "Generated flags for nasm\n");
  printf("-DTARGET_PATH=\"\\\"%s\\\"\" -DTOTAL_SECTORS=%u -DTOTAL_HEADS=%u "
         "-DSECTORS_PER_TRACK=%u -DMEDIA_DESC=0x%x -DSERIAL=0x%x "
         "-DVOLUME_LABEL=\"\\\"%.11s\\\"\" -DSECTORS_PER_FAT=%u "
         "-DROOT_DIR_SECTORS=%u -DOEM_ID=\"\\\"%.8s\\\"\" "
         "-DBYTES_PER_SECTOR=%u",
         bootPath, diskInfo.totalSectors, diskInfo.totalHeads,
         diskInfo.sectorsPerTrack, diskInfo.mediaType, serialID,
         paddedLabel, get_sectors_per_fat12(diskInfo.totalSectors,
                                      diskInfo.bytesPerSector),
         diskInfo.rootDirSectors, paddedOem, diskInfo.bytesPerSector);

  return EXIT_SUCCESS;
}

