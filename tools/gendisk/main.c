/*
MIT License

Copyright (c) 2025 Indigo Carmine (CeriseSky)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/param.h>

// DT_COPY_STATS can be used to create aliases for disk names
// (e.g. "MyFavouriteDisk" can have id DT_COPY_STATS and a copyName of
//       "5.25/IBM/PC_5150/160K" to copy its stats) and if you think that's
// useless then you're wrong!
typedef enum {
  DT_INVALID,
  DT_GENERIC_160K,  // any disk that is exactly 163840 bytes in size
  DT_GENERIC_320K,  // any disk that is exactly 327680 bytes in size
  DT_GENERIC_720K,
  DT_GENERIC_1440K, // any disk that is exactly 1474560 bytes in size
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
  { .name = "3.5/IBM/PC/720K", .id = DT_GENERIC_720K},
  { .name = "3.5/IBM/PS2_286/1.44M", .id = DT_GENERIC_1440K},
};
#define NUM_DISK_TYPES (sizeof(g_diskTypes)/sizeof(disk_t))

#define DEFAULT_RECURSION_LIMIT 10
#define BYTES_PER_KILOBYTE 1024

// searches g_diskTypes for your disk and resolves DT_COPY_STATS links.
// Will stop resolving after recursion_limit links to prevent circular
// dependencies from causing infinite loops.
// set recursion_limit to be less than 0 if you want to disable it
DISK_TYPE get_disk_type(char *name, int recursion_limit) {
  if(!recursion_limit)
    return DT_INVALID;

  for(int i = 0; i < NUM_DISK_TYPES; i++)
    if(!strcmp(name, g_diskTypes[i].name))
      return g_diskTypes[i].id != DT_COPY_STATS ?
        g_diskTypes[i].id : get_disk_type(g_diskTypes[i].copyName,
                                          recursion_limit - 1);

  return DT_INVALID;
}

int main(int argc, char **argv) {
  if(argc == 2 && !strcmp(argv[1], "help")) {
    for(int i = 0; i < NUM_DISK_TYPES; i++)
      fprintf(stderr, "%s\n", g_diskTypes[i].name);
    return EXIT_SUCCESS;
  }

  if(argc != 3) {
    fprintf(stderr, "Format: gendisk <type> <filename>\n");
    fprintf(stderr, "Use gendisk help for a list of disk types\n");
    return EXIT_FAILURE;
  }

  DISK_TYPE requested_type = get_disk_type(argv[1], DEFAULT_RECURSION_LIMIT);
  if(requested_type == DT_INVALID) {
    fprintf(stderr, "Requested type is not supported.\nUse gendisk help for a "
                    "list of supported disk types\n");
    return EXIT_FAILURE;
  }

  uint64_t diskSize;
  switch(get_disk_type(argv[1], DEFAULT_RECURSION_LIMIT)) {
    case DT_GENERIC_160K: diskSize = 160*BYTES_PER_KILOBYTE; break;
    case DT_GENERIC_1440K: diskSize = 1440*BYTES_PER_KILOBYTE; break;
    case DT_GENERIC_320K: diskSize = 320*BYTES_PER_KILOBYTE; break;
    case DT_GENERIC_720K: diskSize = 720*BYTES_PER_KILOBYTE; break;

    case DT_INVALID:
      fprintf(stderr, "Requested type is not supported.\n"
                      "Use gendisk help for a list of supported disk types\n");
      return EXIT_FAILURE;
      break;

    default:
      fprintf(stderr, "Requested disk type does not have an associated size. "
                      "This is a bug so try to use a\ndifferent disk type.\n"
                      "Use gendisk help for a list of supported disk types\n");
    return EXIT_FAILURE;
  }

  FILE *newDisk = fopen(argv[2], "w");
  if(!newDisk) {
    int err = errno;    // the man page said to do this
    fprintf(stderr, "Failed to open new disk image: %s\n", strerror(err));
    return EXIT_FAILURE;
  }

  #define BLOCK_SIZE 512
  #define INIT_BYTE 0xe5    // chosen because it is the same for the format
                            // value used in PC-DOS
  uint8_t *buffer = malloc(BLOCK_SIZE);
  if(!buffer) {
    fprintf(stderr,
            "Failed to allocate memory for the initialisation buffer\n");
    fclose(newDisk);
    return EXIT_FAILURE;
  }
  memset(buffer, INIT_BYTE, BLOCK_SIZE);

  for(size_t i = 0; i < diskSize; i += BLOCK_SIZE) {
    size_t writeSize = MIN(BLOCK_SIZE, diskSize - i);
    if(!fwrite(buffer, writeSize, 1, newDisk)) {
      fprintf(stderr, "Failed to write to disk image\n");
      free(buffer);
      fclose(newDisk);
      return EXIT_FAILURE;
    }
  }

  free(buffer);
  fclose(newDisk);
  return EXIT_SUCCESS;
}

