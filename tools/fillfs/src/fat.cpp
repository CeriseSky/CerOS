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

#include "fat.hpp"

namespace fat12 {

  FileSystem::FileSystem(char *path) {
    if( !(image = fopen(path, "rb+")) )
      throw std::runtime_error("Failed to open disk image");

    if(!readBootRecord())
      throw std::runtime_error("Failed to read boot record");
    if(!readFAT())
      throw std::runtime_error("Failed to read file allocation table");
    if(!readRootDirectory())
      throw std::runtime_error("Failed to read root directory");
  }

  bool FileSystem::sync() {
    if(!writeBootRecord()) return false;
    if(!writeFAT()) return false;
    if(!writeRootDirectory()) return false;

    return true;
  }

  void FileSystem::reset() {
    // fat12 entries are 1.5 bytes each, so only 2/3 as many
    // can fit in a sector compared to bytes
    size_t fatEntries = bootRecord.bpb.sectorsPerFAT *
                        bootRecord.bpb.bytesPerSector * 2/3;

    for(size_t i = 0; i < fatEntries; i++) {
      setFATEntry(i, 0);
    }
    setFATEntry(0, 0xf00 | bootRecord.bpb.mediaType);
    setFATEntry(1, 0xfff);

    for(size_t i = 0; i < bootRecord.bpb.rootDirEntries; i++)
      rootDirectory[i].name[0] = DIR_DELETED;
  }

  void FileSystem::setFATEntry(uint16_t entry, uint16_t value) {
    // 12 bits = 3/2 bytes
    size_t byteOffset = entry * 3 / 2;

    // fat12:  001 002 003 004
    // bytes:  00 10 02 00 30 04
    // uint16: 0010 1002 0030 3004
    // for even entries: << 4
    // for odd entries: & 0xfff
    *(uint16_t *)(fat.data() + byteOffset) &=
      entry & 1 ? 0xf: 0xf000;
    *(uint16_t *)(fat.data() + byteOffset) |=
      entry & 1 ? value << 4 : value & 0xfff;
  }

  bool FileSystem::readBootRecord() {
    fseek(image, 0, SEEK_SET);
    return fread(&bootRecord, sizeof(BootRecord), 1, image) > 0;
  }

  bool FileSystem::readFAT() {
    uint16_t fatLBA = bootRecord.bpb.reservedSectors;
    fat.resize(bootRecord.bpb.bytesPerSector *
               bootRecord.bpb.sectorsPerFAT);
    return readSectors(fatLBA, bootRecord.bpb.sectorsPerFAT, fat.data());
  }

  bool FileSystem::readRootDirectory() {
    uint16_t rootLBA = bootRecord.bpb.reservedSectors +
                       bootRecord.bpb.totalFATs *
                       bootRecord.bpb.sectorsPerFAT;
    uint32_t rootDirBytes = bootRecord.bpb.rootDirEntries *
                            sizeof(DirectoryEntry);
    uint16_t rootSectors = rootDirBytes /
                           bootRecord.bpb.bytesPerSector +
                           (rootDirBytes %
                            bootRecord.bpb.bytesPerSector ? 1 : 0);
    uint16_t roundedEntries = rootSectors *
                              bootRecord.bpb.bytesPerSector /
                              sizeof(DirectoryEntry);
    rootDirectory.resize(roundedEntries);
    return readSectors(rootLBA, rootSectors, rootDirectory.data());
  }

  bool FileSystem::writeBootRecord() {
    fseek(image, 0, SEEK_SET);
    return fwrite(&bootRecord, sizeof(BootRecord), 1, image) > 0;
  }

  bool FileSystem::writeFAT() {
    uint16_t fatLBA = bootRecord.bpb.reservedSectors;
    for(size_t i = 0; i < bootRecord.bpb.totalFATs; i++) {
      if(!writeSectors(fatLBA + bootRecord.bpb.sectorsPerFAT * i,
                       bootRecord.bpb.sectorsPerFAT, fat.data()))
        return false;
    }

    return true;
  }

  bool FileSystem::writeRootDirectory() {
    uint16_t rootLBA = bootRecord.bpb.reservedSectors +
                       bootRecord.bpb.totalFATs *
                       bootRecord.bpb.sectorsPerFAT;
    uint16_t rootSectors = rootDirectory.size() * sizeof(DirectoryEntry) /
                           bootRecord.bpb.bytesPerSector;
    return writeSectors(rootLBA, rootSectors, rootDirectory.data());
  }

  bool FileSystem::readSectors(std::uint16_t lba, std::uint8_t count, void *data) {
    fseek(image, lba * bootRecord.bpb.bytesPerSector, SEEK_SET);
    return fread(data, bootRecord.bpb.bytesPerSector, count, image) == count;
  }

  bool FileSystem::writeSectors(std::uint16_t lba, std::uint8_t count, void *data) {
    fseek(image, lba * bootRecord.bpb.bytesPerSector, SEEK_SET);
    return fwrite(data, bootRecord.bpb.bytesPerSector, count, image) == count;
  }

}

