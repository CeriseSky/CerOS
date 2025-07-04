; CerOS Bootloader & Bootloader Tools
; Copyright (C) 2025 Indigo Carmine (CeriseSky)
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <https://www.gnu.org/licenses/>.
;
; notices:
;   - It is recommended to use the genflags tool to generate the options,
;     rather than trying to define them manually or not at all
;   - This file is intended to be distributed as a single source file, so is
;     choosing not to include any header files.

; =====OPTIONAL FEATURES=====

; see the boot86 documentation for more details on these features

; Feature size differences in bytes
; Notes:
; - Usually I prefer to give years with 4 digits but if it gets to the point
;   where it has actually been 100 years since the last check, its time to
;   check again.
; - features marked with * are considered too large to be recommended for use
;   and should probably be rewritten
; - features that start with DBG are for testing and debugging, they are not
;   intended to produce a usable boot record and generally they don't work
; - Sizes are compared to a build with no flags enabled. Some flags influence
;   each other and make the size bigger than what's shown here
;
; List:
; FEAT_example: +bytes (checked date YY.MM.DD) *
; FEAT_DISK_RETRY: +20 (25.06.30) *
; FEAT_LEADING_SLASH: +6 (25.06.30)
; DBG_READ_FAIL: -2 (25.06.30)
; FEAT_ERROR_CODES: +16 (25.06.30) *

; Forces read_sectors to always fail
;%define DBG_READ_FAIL

; Error code is stored in cl before jumping to hang
; List of errors:
;   - hA = Disk read error
;   - hB = File not found
;%define FEAT_ERROR_CODES

; ===========================


; =====Disk Information=====

; nasm will give a negative times error if the path is too long
%ifndef TARGET_PATH
%define TARGET_PATH "boot.com"
%endif

; if no flags were passed to the assembler, the boot record defaults to the
; values for a 320K floppy diskette with serial 0x43534b59 and label
; "CERISESKYOS"
%ifndef TOTAL_SECTORS
%define TOTAL_SECTORS 640
%endif

%ifndef TOTAL_HEADS
%define TOTAL_HEADS 2
%endif

%ifndef SECTORS_PER_TRACK
%define SECTORS_PER_TRACK 8
%endif

DOS_320K_DISKETTE_DESCRIPTOR equ 0ffh
%ifndef MEDIA_DESC
%define MEDIA_DESC DOS_320K_DISKETTE_DESCRIPTOR
%endif

%ifndef SERIAL
%define SERIAL 0x43534b59     ; The characters "CSKY" in big endian
%endif

%ifndef VOLUME_LABEL
%define VOLUME_LABEL "CEROS BOOT "
%endif

%ifndef SECTORS_PER_FAT
%define SECTORS_PER_FAT 2
%endif

%ifndef ROOT_DIR_SECTORS
%define ROOT_DIR_SECTORS 8
%endif
ROOT_DIR_CAPACITY equ 16*ROOT_DIR_SECTORS

%ifndef OEM_ID
%define OEM_ID "CSKYINDY"
%endif

%ifndef BYTES_PER_SECTOR
%define BYTES_PER_SECTOR 512
%endif

; ==========================

bits 16
cpu 8086

BIOS_LOAD_ADDRESS equ 7c00h
org BIOS_LOAD_ADDRESS

BIOS_VIDEO_PRINT_CHAR_AH equ 0eh
BIOS_VIDEO_INT equ 10h

BIOS_DISK_READ_SECTORS_AH equ 02h
BIOS_DISK_RESET_AH equ 00h
BIOS_DISK_INT equ 13h
BIOS_DRIVE_A equ 00h

CR equ 0dh
LF equ 0ah

struc FAT_NAME
  .name: resb FAT_NAME_LENGTH
  .ext: resb FAT_EXT_LENGTH
endstruc
FAT_NAME_LENGTH equ 8
FAT_EXT_LENGTH equ 3
FAT_NAME_EXT_LENGTH equ 11
FAT_SIGNATURE equ 29h
struc FAT_directory_entry
  .name: resb FAT_NAME_EXT_LENGTH
  .attributes: resb 1
  ._reserved: resb 1
  .creation_time_cents: resb 1
  .creation_time: resw 1
  .creation_date: resw 1
  .access_date: resw 1
  ._reserved2: resw 1
  .modified_time: resw 1
  .modified_date: resw 1
  .first_cluster: resw 1
  .size: resd 1

  .__struc_size:
endstruc

jmp setup       ; I wasted so much time trying to figure out why the code
nop             ; wouldn't work because I forgot these 2 lines.

SECTORS_PER_CLUSTER equ 2
RESERVED_SECTORS equ 1
TOTAL_FATS equ 2

FAT_LBA equ RESERVED_SECTORS
ROOT_DIR_LBA equ FAT_LBA + (SECTORS_PER_FAT * TOTAL_FATS)
DATA_SECTION_LBA equ ROOT_DIR_LBA + ROOT_DIR_SECTORS

bpb:
  .oem: db OEM_ID
  .bytes_per_sector: dw BYTES_PER_SECTOR
  .sectors_per_cluster: db SECTORS_PER_CLUSTER
  .reserved_sectors: dw RESERVED_SECTORS
  .total_FATs: db TOTAL_FATS
  .root_dir_capacity: dw ROOT_DIR_CAPACITY
  .total_sectors: dw TOTAL_SECTORS
  .media_descriptor: db MEDIA_DESC
  .sectors_per_fat: dw SECTORS_PER_FAT
  .sectors_per_track: dw SECTORS_PER_TRACK
  .total_heads: dw TOTAL_HEADS
  .hidden_sectors: dd 0             ; mbr support likely won't be added
                                    ; until after fat16 is implemented
  .total_sectors32: dd 0
ebpb:
  .drive_number: db BIOS_DRIVE_A
  ._reserved: db 0
  .signature: db FAT_SIGNATURE
  .serial: dd SERIAL
  .label: db VOLUME_LABEL
  .type: db "FAT12   "

; Different BIOSes aren't expected to put the boot record program in a well
; defined state before executing it so one must be created now
setup:
  cli         ; prevents a bug when resetting ss on 8088 CPUs
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov ss, ax
  sti

  mov sp, BIOS_LOAD_ADDRESS
  ; None of the boot record code uses bp so it's not set until the end

  jmp 0:main

; Modern BIOSes use the high bits of cl as part of the cylinder but this
; function doesn't which means that 2.88M floppies aren't supported
; in ax = lba
; out ch = cylinder
; out cl = sector
; out dh = head
lba2chs:
  push ax
  push dx
  xor dx, dx
  div word [bpb.sectors_per_track]
  inc dx      ; sector = (lba MOD SPT) + 1
  mov cl, dl
  xor dx, dx
  %if TOTAL_HEADS != 1        ; saves bytes on single-sided diskettes
  div word [bpb.total_heads]
  %endif
  mov ch, al  ; cylinder = (lba / SPT) / heads
  mov al, dl  ; head = (lba / SPT) MOD heads
  pop dx
  mov dh, al
  pop ax
  ret

; in ax = start lba
; in es:bx = location to read into
; in cl = count
; in dl = drive
; destroys di if FEAT_DISK_RETRY is enabled
read_sectors:
  push cx
  call lba2chs
  pop ax
  %ifdef FEAT_DISK_RETRY
    mov di, 3       ;  3 tries
  %endif
  .try:
    mov ah, BIOS_DISK_READ_SECTORS_AH
    stc ; some BIOSes don't set this properly
  %ifndef DBG_READ_FAIL
    int BIOS_DISK_INT
  %endif
  %ifdef FEAT_DISK_RETRY
    jnc .end
  .catch:
    test di, di
    %ifdef FEAT_ERROR_CODES
      jnz .catch.reset
      mov cl, 'A'
      jmp hang
    %else
      jz hang
    %endif
    .catch.reset:
      dec di

      mov ah, BIOS_DISK_RESET_AH
      stc
      int BIOS_DISK_INT
      jnc .try
      %ifdef FEAT_ERROR_CODES
        mov cl, 'A'
      %endif
      jmp hang
  %else
    %ifdef FEAT_ERROR_CODES
      jnc .end
      mov cl, 'A'
      jmp hang
    %else
      jc hang
    %endif
  %endif
  .end:
    ret

; in si = string
; in dl = character
; out di = pointer to found character, or NULL
; destroys ax
strchr:
  push si
  xor di, di
  .loop:
    lodsb

    test al, al
    jz .end

    cmp al, dl
    je .found

    jmp .loop
  .found:
    mov di, si
    dec di      ; si was incremented past where it should be by lodsb
  .end:
    pop si
    ret

; in si = string
; out cx = length
; destroys ax
strlen:
  push si
  ;push ax
  xor cx, cx
  .loop:
    lodsb
    test al, al
    jz .end
    inc cx
    jmp .loop
  .end:
    ;pop ax
    pop si
    ret

; in ax = cluster
; out ax = lba
; destroys dx
FIRST_CLUSTER equ 2
cluster2lba:
  sub ax, FIRST_CLUSTER
  mul byte [bpb.sectors_per_cluster]
  add ax, DATA_SECTION_LBA
  ret

main:
  mov si, path
  ;mov byte [flags], 0
  mov byte [ebpb.drive_number], dl


  FAT_BUFFER equ 500h ; earliest memory address not in use
  .read_fat:
    mov ax, RESERVED_SECTORS
    mov bx, FAT_BUFFER
    mov cl, SECTORS_PER_FAT
    ;mov dl, [ebpb.drive_number]
    call read_sectors

  DIRECTORY_BUFFER equ FAT_BUFFER + (SECTORS_PER_FAT * BYTES_PER_SECTOR)
  .read_root_directory:
    mov ax, ROOT_DIR_LBA
    mov bx, DIRECTORY_BUFFER
    mov cl, ROOT_DIR_SECTORS
    ;mov dl, [ebpb.drive_number]
    call read_sectors

  %ifdef FEAT_LEADING_SLASH
    .check_start:
        cmp byte [si], '/'
        jne .check_start.end
        inc si
    .check_start.end:
  %endif
  .loop:
    ;test byte [flags], FLAG_FINISHED
    ;jnz hang

    .empty_name_buffer:
      mov al, ' '
      mov cx, FAT_NAME_EXT_LENGTH
      mov di, name_buffer
      rep stosb

    .split:
      mov dl, '/'
      call strchr
      mov word [next_section], 0
      test di, di
      jz .split.end
      mov byte [di], 0
      inc di
      mov word [next_section], di
    .split.end:

    push si
    .handle_ext:
      mov dl, '.'
      call strchr

      test di, di
      jz .handle_ext.end

      mov byte [di], 0
      inc di

      mov si, di
      call strlen
      mov di, name_buffer + FAT_NAME.ext
      cmp cx, FAT_EXT_LENGTH
      jb .shorter_ext

      .longer_ext:
        mov cx, FAT_EXT_LENGTH
      .shorter_ext:

      rep movsb
    .handle_ext.end:
      pop si

    .handle_name:
      call strlen
      mov di, name_buffer + FAT_NAME.name
      cmp cx, FAT_NAME_LENGTH
      jb .shorter_name

      .longer_name:
        mov cx, FAT_NAME_LENGTH
      .shorter_name:

      rep movsb
    .handle_name.end:

    mov cx, FAT_NAME_EXT_LENGTH
    mov si, name_buffer
    .convert_to_upper:
      lodsb
      cmp al, 'a'
      jb .convert_to_upper.continue
      cmp al, 'z'
      ja .convert_to_upper.continue
      sub al, 'a' - 'A'
      mov byte [si - 1], al
      .convert_to_upper.continue:
        loop .convert_to_upper
      .convert_to_upper.end:

      mov si, DIRECTORY_BUFFER
      xor bx, bx
      .find_file:
        push si
        mov di, name_buffer
        mov cx, FAT_NAME_EXT_LENGTH
        repe cmpsb
        pop si
        je .find_file.end

        add si, FAT_directory_entry.__struc_size
        inc bx
        cmp bx, ROOT_DIR_CAPACITY

        jb .find_file
        %ifdef FEAT_ERROR_CODES
          mov cl, 'B'
        %endif
        jmp hang
      .find_file.end:

      FAT_ATTR_DIRECTORY equ 10h
      .get_properties:
        ;test byte [si + FAT_directory_entry.attributes], FAT_ATTR_DIRECTORY
        ;jz .get_location
        ;or byte [flags], FLAG_FINISHED

      .get_location:
        mov ax, [si + FAT_directory_entry.first_cluster]

      FAT_LAST_CLUSTER equ 0ff8h
      mov bx, DIRECTORY_BUFFER
      .read_file:
        cmp ax, FAT_LAST_CLUSTER
        jae .read_file.end

        push ax
        call cluster2lba
        mov cl, SECTORS_PER_CLUSTER
        mov dl, [ebpb.drive_number]
        call read_sectors

        mov ax, BYTES_PER_SECTOR
        mul word [bpb.sectors_per_cluster]
        add bx, ax

        pop ax
        push ax
        mov cx, 3
        mul cx
        shr ax, 1
        mov si, ax
        mov ax, [FAT_BUFFER + si]
        pop cx
        test cl, 1
        jz .even_cluster

        .odd_cluster:
          mov cl, 4
          shr ax, cl

        .even_cluster:
          and ax, 0fffh

        jmp .read_file
      .read_file.end:

      .check_loop:
        mov di, [next_section]
        test di, di
        jz .end

      mov si, di
      jmp .loop

    .end:

    ; should be a check for if flags say we are done or not here

    COM_OFFSET equ 100h
    mov ax, DIRECTORY_BUFFER
    ;test al, 0fh
    ;jnz hang
    sub ax, COM_OFFSET
    mov cl, 4
    shr ax, cl

    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, COM_OFFSET
    mov bp, sp
    push ax

    mov ax, COM_OFFSET
    push ax
    retf

hang:
  %ifdef FEAT_ERROR_CODES
    xor bx, bx
    mov ah, BIOS_VIDEO_PRINT_CHAR_AH
    mov al, 'h'
    int BIOS_VIDEO_INT
    mov al, cl
    int BIOS_VIDEO_INT
  %endif
  hang.here:
    jmp hang.here

path: db TARGET_PATH, 0

BOOT_SIGNATURE equ 0aa55h
pad times BYTES_PER_SECTOR-($-$$+2) db 0
dw BOOT_SIGNATURE

buffer:

section .bss

FLAG_FINISHED equ 1
flags: resb 1
next_section: resw 1
name_buffer: resb FAT_NAME_EXT_LENGTH
;directory_pointer: resw 1
data_section_lba: resw 1

