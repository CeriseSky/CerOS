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

DISK_TYPE=5.25/IBM/PC_5150/320K
DISK_BOOT_PATH=/boot.com
DISK_LABEL=CEROS\ BOOT
DISK_OEM=CSKYINDY
DISK_SERIAL=0x$(COMMIT_HASH)

SRC=src
BOOTLOADER_SRC=$(SRC)/boot
TOOLS_SRC=tools
KERNEL_SRC=$(SRC)/kernel

INCLUDE=include
KERNEL_INCLUDE=$(INCLUDE)/kernel

PREFIX=bin
TOOLS_PREFIX=tools/bin
CROSS_PREFIX=tools/xcompile/bin
TMP=$(PREFIX)/tmp
OUTPUT_DIRS=$(PREFIX) \
            $(TOOLS_PREFIX) \
            $(TMP)

COMMIT_HASH=$(shell git rev-parse HEAD | head -c 8)

ASM=nasm
CC=$(CROSS_PREFIX)/x86_64-elf-gcc
LD=$(CROSS_PREFIX)/x86_64-elf-gcc

BOOTSECTOR_FEATURES=-DFEAT_ERROR_CODES -DFEAT_DISK_RETRY
BOOTSECTOR_OPTIONS=$(shell tools/bin/genflags \
                           $(DISK_TYPE) $(DISK_BOOT_PATH) $(DISK_LABEL) \
                           $(DISK_OEM) $(DISK_SERIAL))
BOOTSECTOR_ASMFLAGS=-f bin $(BOOTSECTOR_FEATURES) $(BOOTSECTOR_OPTIONS)

KERNEL_ASMFLAGS=-f elf64
KERNEL_CFLAGS=-ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse \
              -mno-sse2 -Wall -Werror -Wextra -Wpedantic -I$(KERNEL_INCLUDE) \
              -DCOMMIT_HASH=\"$(COMMIT_HASH)\"
KERNEL_LDFLAGS=-ffreestanding -nostdlib -lgcc -T$(SRC)/kernel/linker.ld \
               -Wl,-Map=$(PREFIX)/kernel.map

# in future, update to support directories in $(KERNEL_SRC)
KERNEL_C_OBJS=$(patsubst $(KERNEL_SRC)/%.c,$(TMP)/k_%.o,\
                         $(wildcard $(KERNEL_SRC)/*.c))

TOOLS=$(patsubst tools/%,tools/bin/%,\
      $(shell find tools -mindepth 1 -maxdepth 1 -type d -not -name bin \
                         -not -name xcompile))

SCRIPTS=scripts

.PHONY: all tools disk bootloader install install_bootloader install_programs \
        programs kernel install_kernel clean cleanbuild clean_os clean_osbuild \

.NOTPARALLEL: install

all: dirs .WAIT tools .WAIT disk bootloader kernel .WAIT install

# doesn't recreate the cross compiler
clean_osbuild: clean_os .WAIT all

cleanbuild: clean .WAIT all

dirs: $(OUTPUT_DIRS)

install: install_bootloader install_kernel

run:
	qemu-system-x86_64 -drive if=floppy,format=raw,file=$(PREFIX)/disk.img

clean_os:
	rm -rvf $(TMP) $(TOOLS_PREFIX)

clean: clean_os
	rm -rfv $(TOOLS_SRC)/xcompile

install_bootloader: $(TOOLS) $(PREFIX)/disk.img $(TMP)/boot86.bin
	dd if=$(TMP)/boot86.bin of=$(PREFIX)/disk.img conv=notrunc
	$(TOOLS_PREFIX)/fillfs $(PREFIX)/disk.img

kernel: $(KERNEL_C_OBJS)
	$(ASM) $(KERNEL_ASMFLAGS) $(KERNEL_SRC)/start.asm -o $(TMP)/kstart.o
	$(LD) $(KERNEL_LDFLAGS) $(TMP)/kstart.o $(KERNEL_C_OBJS) -o $(TMP)/boot.com

$(KERNEL_C_OBJS): $(TMP)/k_%.o: $(KERNEL_SRC)/%.c
	$(CC) -c $(KERNEL_CFLAGS) $< -o $@

install_kernel: $(PREFIX)/disk.img $(TMP)/boot.com
	mcopy -i $(PREFIX)/disk.img $(TMP)/boot.com ::/boot.com

disk: $(TOOLS) $(PREFIX)
	$(TOOLS_PREFIX)/gendisk $(DISK_TYPE) $(PREFIX)/disk.img

bootloader: $(TMP)
	$(ASM) $(BOOTSECTOR_ASMFLAGS) $(BOOTLOADER_SRC)/boot86.asm -o \
  $(TMP)/boot86.bin

tools: $(TOOLS) $(CROSS_PREFIX)

$(CROSS_PREFIX):
	$(SCRIPTS)/xcompile.sh

$(TOOLS): $(TOOLS_PREFIX)/%: $(TOOLS_SRC)/%
	mkdir -p $(TOOLS_PREFIX)
	$</build.sh
	cp $</$*.out $@

$(OUTPUT_DIRS):
	mkdir -p $@

