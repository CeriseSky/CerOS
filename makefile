# CerOS
# Copyright (C) 2025 Indigo Carmine (CeriseSky)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# note: It is recommended to use the genflags tool to generate the options,
#       rather than trying to define them manually or not at all

DISK_TYPE=5.25/IBM/PC_5150/160K
DISK_BOOT_PATH=/boot.com
DISK_LABEL=CEROS\ BOOT
DISK_OEM=CSKYINDY
DISK_SERIAL=0x$(COMMIT_HASH)

SRC=src
BOOTLOADER_SRC=$(SRC)/boot
TOOLS_SRC=tools
PROGRAM_SRC=$(SRC)/programs

PREFIX=bin
TOOLS_PREFIX=tools/bin
TMP=$(PREFIX)/tmp
OUTPUT_DIRS=$(PREFIX) \
            $(TOOLS_PREFIX) \
            $(TMP)

COMMIT_HASH=$(shell git rev-parse HEAD | head -c 8)

ASM=nasm

BOOTSECTOR_FEATURES=-DFEAT_ERROR_CODES -DFEAT_DISK_RETRY
BOOTSECTOR_OPTIONS=$(shell tools/bin/genflags \
                           $(DISK_TYPE) $(DISK_BOOT_PATH) $(DISK_LABEL) \
                           $(DISK_OEM) $(DISK_SERIAL))
BOOTSECTOR_ASMFLAGS=-f bin $(BOOTSECTOR_FEATURES) $(BOOTSECTOR_OPTIONS)

PROGRAM_ASMFLAGS=-f bin

TOOLS=$(patsubst tools/%,tools/bin/%,\
      $(shell find tools -mindepth 1 -maxdepth 1 -type d -not -name bin))

.PHONY: all tools disk bootloader install install_bootloader install_programs \
        programs

.NOTPARALLEL: install install_programs

all: dirs tools disk bootloader programs .WAIT install

dirs: $(OUTPUT_DIRS)

install: install_bootloader install_programs

install_bootloader: $(TOOLS) $(PREFIX)/disk.img $(TMP)/boot86.bin
	dd if=$(TMP)/boot86.bin of=$(PREFIX)/disk.img conv=notrunc
	$(TOOLS_PREFIX)/fillfs $(PREFIX)/disk.img

install_programs: $(TMP)/boot.com
	mcopy -i $(PREFIX)/disk.img $(TMP)/boot.com ::/boot.com

programs:
	$(ASM) $(PROGRAM_ASMFLAGS) $(PROGRAM_SRC)/cpu_test.asm -o $(TMP)/boot.com

disk: $(TOOLS) $(PREFIX)
	$(TOOLS_PREFIX)/gendisk $(DISK_TYPE) $(PREFIX)/disk.img

bootloader: $(TMP)
	$(ASM) $(BOOTSECTOR_ASMFLAGS) $(BOOTLOADER_SRC)/boot86.asm -o \
  $(TMP)/boot86.bin

tools: $(TOOLS)

$(TOOLS): $(TOOLS_PREFIX)/%: $(TOOLS_SRC)/%
	mkdir -p $(TOOLS_PREFIX)
	$</build.sh
	cp $</$*.out $@

$(OUTPUT_DIRS):
	mkdir -p $@

