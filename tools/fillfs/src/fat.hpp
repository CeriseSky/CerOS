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

#ifndef _FAT_HPP_
#define _FAT_HPP_

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <cstdio>

namespace fat12 {

  struct __attribute__((packed)) Bpb {
    std::uint8_t dataSkip[3];
    char oem[8];
    std::uint16_t bytesPerSector;
    std::uint8_t sectorsPerCluster;
    std::uint16_t reservedSectors;
    std::uint8_t totalFATs;
    std::uint16_t rootDirEntries;
    std::uint16_t totalSectors16;
    std::uint8_t mediaType;
    std::uint16_t sectorsPerFAT;
    std::uint16_t sectorsPerTrack;
    std::uint16_t totalHeads;
    std::uint32_t hiddenSectors;
    std::uint32_t totalSectors32;
  };

  struct __attribute__((packed)) Ebpb {
    std::uint8_t driveNum;
    std::uint8_t _reserved;
    std::uint8_t signature;
    std::uint32_t serial;
    char label[11];
    char type[8];
  };

  struct __attribute__((packed)) BootRecord {
    Bpb bpb;
    Ebpb ebpb;
  };

  struct __attribute__((packed)) DirectoryEntry {
    char name[8];
    char ext[3];
    std::uint8_t attributes;
    std::uint8_t _reserved1;
    std::uint8_t creationTimeCents;
    std::uint16_t creationTime;
    std::uint16_t creationDate;
    std::uint16_t lastAccessDate;
    std::uint16_t _reserved2;
    std::uint16_t modificationTime;
    std::uint16_t modificationDate;
    std::uint16_t firstCluster;
    std::uint32_t size;
  };

  // if DirectoryEntry::name[0] == DIR_DELETED
  // then the entry has been marked as free
  constexpr char DIR_DELETED = 0xe5;

  class FileSystem {
    public:
      FileSystem() { throw std::runtime_error("No disk specified"); }
      FileSystem(char *path);

      // writes all the current file system structures onto the disk
      // warning: usually corrupts the disk if an error occurs
      bool sync();

      // initialises the file system using the values already in
      // the boot record rather than generating them automatically
      void reset();

    private:
      FILE *image;
      BootRecord bootRecord;
      std::vector<std::uint8_t> fat;
      std::vector<DirectoryEntry> rootDirectory;

      bool readBootRecord();
      bool readFAT();
      bool readRootDirectory();

      bool writeBootRecord();
      bool writeFAT();
      bool writeRootDirectory();

      bool readSectors(std::uint16_t lba, std::uint8_t count, void *data);
      bool writeSectors(std::uint16_t lba, std::uint8_t count, void *data);

      void setFATEntry(uint16_t entry, uint16_t value);
  };

}

#endif

