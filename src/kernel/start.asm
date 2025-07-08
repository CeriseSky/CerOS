; CerOS Kernel
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

COM_OFFSET equ 100h
KERNEL_OFFSET equ 8000h       ; makes it possible to use paging but requires
                              ; moving the kernel code to this address

extern kmain
extern vgaText_puts
extern _end

bits 16
cpu 8086

section .text

global init
init:
  push ds
  pop ax
  test ax, ax
  jz start

  ; memcpy to the expected location.
  ; this allows the code to immediately stop using segmentation, which
  ; makes the process of setting up the page tables much easier
  mov cx, _end - init
  xor ax, ax
  mov si, COM_OFFSET
  mov es, ax
  mov di, KERNEL_OFFSET

  rep movsb

  mov ds, ax
  mov ss, ax
  jmp 0:KERNEL_OFFSET

start:
  mov si, test_start
  call puts16

  mov si, msg_386
  call puts16
  call _386_test16
  test ax, 1
  jz test_fail_handler16
  mov si, test_success
  call puts16

  cpu 386
  mov si, msg_cpuid
  call puts16
  call cpuid_test16
  test ax, 1
  jz test_fail_handler16
  mov si, test_success
  call puts16

  cpu 586
  mov si, msg_long_mode
  call puts16
  call longmode_test16
  test ax, 1
  jz test_fail_handler16
  mov si, test_success
  call puts16

  cpu x86-64
  cli

  SIZEOF_PAGE_TABLE equ 1000h
  PML4T_ADDR equ 1000h
  PDPT_ADDR equ PML4T_ADDR + SIZEOF_PAGE_TABLE
  PDT_ADDR equ PDPT_ADDR + SIZEOF_PAGE_TABLE
  PT_ADDR equ PDT_ADDR + SIZEOF_PAGE_TABLE
  mov edi, PML4T_ADDR
  mov cr3, edi

  xor eax, eax
  mov ecx, SIZEOF_PAGE_TABLE
  rep stosd     ; clears all 4 page tables

  mov edi, cr3

  PT_ADDR_MASK equ 0xffffffffff000
  PT_PRESENT equ 1
  PT_READABLE equ 2
  mov dword [edi], PDPT_ADDR & PT_ADDR_MASK | PT_PRESENT | PT_READABLE
  mov edi, PDPT_ADDR
  mov dword [edi], PDT_ADDR & PT_ADDR_MASK | PT_PRESENT | PT_READABLE
  mov edi, PDT_ADDR
  mov dword [edi], PT_ADDR & PT_ADDR_MASK | PT_PRESENT | PT_READABLE

  SIZEOF_PT_ENTRY equ 8
  ENTRIES_PER_PT equ SIZEOF_PAGE_TABLE / SIZEOF_PT_ENTRY
  PAGE_SIZE equ 0x1000
  mov edi, PT_ADDR
  mov ebx, PT_PRESENT | PT_READABLE
  mov ecx, ENTRIES_PER_PT
  .setEntry:
    mov dword [edi], ebx
    add ebx, PAGE_SIZE
    add edi, SIZEOF_PT_ENTRY
    loop .setEntry

  CR4_PAE_ENABLE equ 1 << 5
  mov eax, cr4
  or al, CR4_PAE_ENABLE
  mov cr4, eax

  EFER_MSR equ 0xC0000080
  EFER_LM_ENABLE equ 1 << 8

  mov ecx, EFER_MSR
  rdmsr
  or eax, EFER_LM_ENABLE
  wrmsr

  CR0_PM_ENABLE equ 1
  CR0_PG_ENABLE equ 1 << 31
  mov eax, cr0
  or eax, CR0_PG_ENABLE | CR0_PM_ENABLE
  mov cr0, eax

  lgdt [_gdtr]
  jmp (gdt.code64 - gdt):.lmode

  .lmode:
    bits 64
    mov ax, gdt.data - gdt
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, 0x1000
    mov rbp, rsp

    mov rdi, gdt.code64 - gdt
    call kmain

    VGA_TEXT_COLS equ 80
    VGA_TEXT_ROWS equ 25
    VGA_TEXT_RED equ 4
    VGA_TEXT_WHITE equ 15
    mov rdi, endMessage
    mov rsi, VGA_TEXT_COLS - endMessage.length
    mov rdx, VGA_TEXT_ROWS - 1
    mov rcx, VGA_TEXT_WHITE << 4 | VGA_TEXT_RED
    call vgaText_puts

    .lmode.loop:
      jmp .lmode.loop

  .hang:
    bits 16
    cpu 8086
    mov si, test_hang
    call puts16
    .hang.loop:
      jmp .hang.loop

endMessage: db "Kernel has stopped execution."
  .length equ $ - endMessage

test_fail_handler16:
  cpu 8086
  bits 16
  mov si, test_fail
  call puts16
  jmp start.hang

CR equ 0dh
LF equ 0ah
NUL equ 0
test_start: db "Testing CPU for required features", CR, LF, NUL
test_success: db "Supported", CR, LF, NUL
test_fail: db "Not Supported", CR, LF, NUL
test_hang: db "Kernel is now stopping execution", CR, LF, NUL

msg_386: db "  EFLAGS Register: ", NUL
msg_cpuid: db "  CPUID Instruction: ", NUL
msg_long_mode: db "  Long Mode (x86_64): ", NUL

BIOS_VIDEO_PRINT_CHAR_AH equ 14
BIOS_VIDEO_INT equ 10h

; in char *si = pointer to string to print
puts16:
  bits 16
  cpu 8086
  push si
  push bx
  xor bx, bx
  .loop:
    lodsb
    test al, al
    jz .end

    mov ah, BIOS_VIDEO_PRINT_CHAR_AH
    int BIOS_VIDEO_INT

    jmp .loop
    .end:
      pop bx
      pop si
      ret

; returns ax = 1 if the processor supports 32-bit features; 0 if not
; destroys the fs register on 32-bit systems
_386_test16:
  bits 16
  cpu 8086
  push bx

  mov ax, ds
  push ds

  cpu 386
  pop fs
  mov bx, fs
  cpu 8086

  cmp ax, bx
  je .success
  .fail:
    xor ax, ax
    jmp .end
  .success:
    mov ax, 1
  .end:
    pop bx
    ret

; returns ax = 1 if the CPUID instruction is available; 0 if not
; requires at least a 386 processor (see _386_test16)
cpuid_test16:
  bits 16
  cpu 386

  push ecx

  ; if the EFLAGS_ID bit can be flipped, then CPUID is available
  EFLAGS_ID equ 1 << 21
  pushfd
  pop eax
  mov ecx, eax

  xor eax, EFLAGS_ID
  push eax
  popfd
  pushfd
  pop eax

  push ecx
  popfd

  cmp eax, ecx
  jnz .success
  .fail:
    xor ax, ax
    jmp .end
  .success:
    mov ax, 1
  .end:
    pop ecx
    ret

; returns ax = 1 if long mode is supported; 0 if not
; requires the cpu to have support for the CPUID instruction
longmode_test16:
  bits 16
  cpu 586

  CPUID_EXTENSIONS equ 0x80000000
  CPUID_EXT_FEATURES equ 0x80000001
  mov eax, CPUID_EXTENSIONS
  cpuid
  cmp eax, CPUID_EXT_FEATURES
  jb .fail

  CPUID_EDX_EXT_FEAT_LM equ 1 << 29
  mov eax, CPUID_EXT_FEATURES
  cpuid
  test edx, CPUID_EDX_EXT_FEAT_LM
  jnz .success

  .fail:
    xor ax, ax
    jmp .end
  .success:
    mov ax, 1
  .end:
    ret

global outb
outb:
  cpu x86-64
  bits 64
  mov ax, si
  mov dx, di
  out dx, al
  ret

global idt_lidt
idt_lidt:
  cpu x86-64
  bits 64
  ;mov rax, [rdi]
  lidt [rdi]
  ret

global idt_call
idt_call:
  cpu x86-64
  bits 64

  mov rax, rdi
  mov [.self_modify + 1], al

  .self_modify: int 0

  ret

global interrupt_wrapper
extern interrupt_handler
interrupt_wrapper:
  cpu x86-64
  bits 64
  call interrupt_handler
  iretq

gdt:
  .null: dq 0
  .code64:
    dw 0ffffh
    dw 0
    db 0
    db 0b10011011
    db 0b10101111
    db 0
  .data:
    dw 0fffh
    dw 0
    db 0
    db 0b10010011
    db 0b11001111
    db 0
  .end:
_gdtr:
  .size: dw gdt.end - gdt - 1
  .offset: dq gdt

