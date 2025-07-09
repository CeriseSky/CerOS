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
extern _image_end

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
  mov cx, _image_end - init
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
    mov rsp, KERNEL_OFFSET
    mov rbp, rsp

    mov rdi, gdt.code64 - gdt
    mov rsi, gdt.data - gdt
    call kmain

    cli

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

global io_outb
io_outb:
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

global idt_cli
idt_cli:
  cli
  ret

global idt_sti
idt_sti:
  sti
  ret

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

IDT_DUMMY_ERROR_CODE equ 0
%macro IDT_WRAP 1
global idt_wrapper%1
idt_wrapper%1:
  cpu x86-64
  bits 64
  push IDT_DUMMY_ERROR_CODE
  push %1
  jmp idt_wrapper_common
%endmacro

%macro IDT_WRAP_ERR 1
global idt_wrapper%1
idt_wrapper%1:
  cpu x86-64
  bits 64
  push %1
  jmp idt_wrapper_common
%endmacro

extern idt_defaultHandler
idt_wrapper_common:
  push rbp
  push rax
  push rbx
  push rcx
  push rdx
  push rsi
  push rdi
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  mov rdi, rsp
  call idt_defaultHandler

  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rdi
  pop rsi
  pop rdx
  pop rcx
  pop rbx
  pop rax
  pop rbp

  add rsp, 16
  iretq

IDT_WRAP 0
IDT_WRAP 1
IDT_WRAP 2
IDT_WRAP 3
IDT_WRAP 4
IDT_WRAP 5
IDT_WRAP 6
IDT_WRAP 7
IDT_WRAP_ERR 8
IDT_WRAP 9
IDT_WRAP_ERR 10
IDT_WRAP_ERR 11
IDT_WRAP_ERR 12
IDT_WRAP_ERR 13
IDT_WRAP_ERR 14
IDT_WRAP 15
IDT_WRAP 16
IDT_WRAP_ERR 17
IDT_WRAP 18
IDT_WRAP 19
IDT_WRAP 20
IDT_WRAP_ERR 21
IDT_WRAP 22
IDT_WRAP 23
IDT_WRAP 24
IDT_WRAP 25
IDT_WRAP 26
IDT_WRAP 27
IDT_WRAP 28
IDT_WRAP 29
IDT_WRAP 30
IDT_WRAP 31
IDT_WRAP 32
IDT_WRAP 33
IDT_WRAP 34
IDT_WRAP 35
IDT_WRAP 36
IDT_WRAP 37
IDT_WRAP 38
IDT_WRAP 39
IDT_WRAP 40
IDT_WRAP 41
IDT_WRAP 42
IDT_WRAP 43
IDT_WRAP 44
IDT_WRAP 45
IDT_WRAP 46
IDT_WRAP 47
IDT_WRAP 48
IDT_WRAP 49
IDT_WRAP 50
IDT_WRAP 51
IDT_WRAP 52
IDT_WRAP 53
IDT_WRAP 54
IDT_WRAP 55
IDT_WRAP 56
IDT_WRAP 57
IDT_WRAP 58
IDT_WRAP 59
IDT_WRAP 60
IDT_WRAP 61
IDT_WRAP 62
IDT_WRAP 63
IDT_WRAP 64
IDT_WRAP 65
IDT_WRAP 66
IDT_WRAP 67
IDT_WRAP 68
IDT_WRAP 69
IDT_WRAP 70
IDT_WRAP 71
IDT_WRAP 72
IDT_WRAP 73
IDT_WRAP 74
IDT_WRAP 75
IDT_WRAP 76
IDT_WRAP 77
IDT_WRAP 78
IDT_WRAP 79
IDT_WRAP 80
IDT_WRAP 81
IDT_WRAP 82
IDT_WRAP 83
IDT_WRAP 84
IDT_WRAP 85
IDT_WRAP 86
IDT_WRAP 87
IDT_WRAP 88
IDT_WRAP 89
IDT_WRAP 90
IDT_WRAP 91
IDT_WRAP 92
IDT_WRAP 93
IDT_WRAP 94
IDT_WRAP 95
IDT_WRAP 96
IDT_WRAP 97
IDT_WRAP 98
IDT_WRAP 99
IDT_WRAP 100
IDT_WRAP 101
IDT_WRAP 102
IDT_WRAP 103
IDT_WRAP 104
IDT_WRAP 105
IDT_WRAP 106
IDT_WRAP 107
IDT_WRAP 108
IDT_WRAP 109
IDT_WRAP 110
IDT_WRAP 111
IDT_WRAP 112
IDT_WRAP 113
IDT_WRAP 114
IDT_WRAP 115
IDT_WRAP 116
IDT_WRAP 117
IDT_WRAP 118
IDT_WRAP 119
IDT_WRAP 120
IDT_WRAP 121
IDT_WRAP 122
IDT_WRAP 123
IDT_WRAP 124
IDT_WRAP 125
IDT_WRAP 126
IDT_WRAP 127
IDT_WRAP 128
IDT_WRAP 129
IDT_WRAP 130
IDT_WRAP 131
IDT_WRAP 132
IDT_WRAP 133
IDT_WRAP 134
IDT_WRAP 135
IDT_WRAP 136
IDT_WRAP 137
IDT_WRAP 138
IDT_WRAP 139
IDT_WRAP 140
IDT_WRAP 141
IDT_WRAP 142
IDT_WRAP 143
IDT_WRAP 144
IDT_WRAP 145
IDT_WRAP 146
IDT_WRAP 147
IDT_WRAP 148
IDT_WRAP 149
IDT_WRAP 150
IDT_WRAP 151
IDT_WRAP 152
IDT_WRAP 153
IDT_WRAP 154
IDT_WRAP 155
IDT_WRAP 156
IDT_WRAP 157
IDT_WRAP 158
IDT_WRAP 159
IDT_WRAP 160
IDT_WRAP 161
IDT_WRAP 162
IDT_WRAP 163
IDT_WRAP 164
IDT_WRAP 165
IDT_WRAP 166
IDT_WRAP 167
IDT_WRAP 168
IDT_WRAP 169
IDT_WRAP 170
IDT_WRAP 171
IDT_WRAP 172
IDT_WRAP 173
IDT_WRAP 174
IDT_WRAP 175
IDT_WRAP 176
IDT_WRAP 177
IDT_WRAP 178
IDT_WRAP 179
IDT_WRAP 180
IDT_WRAP 181
IDT_WRAP 182
IDT_WRAP 183
IDT_WRAP 184
IDT_WRAP 185
IDT_WRAP 186
IDT_WRAP 187
IDT_WRAP 188
IDT_WRAP 189
IDT_WRAP 190
IDT_WRAP 191
IDT_WRAP 192
IDT_WRAP 193
IDT_WRAP 194
IDT_WRAP 195
IDT_WRAP 196
IDT_WRAP 197
IDT_WRAP 198
IDT_WRAP 199
IDT_WRAP 200
IDT_WRAP 201
IDT_WRAP 202
IDT_WRAP 203
IDT_WRAP 204
IDT_WRAP 205
IDT_WRAP 206
IDT_WRAP 207
IDT_WRAP 208
IDT_WRAP 209
IDT_WRAP 210
IDT_WRAP 211
IDT_WRAP 212
IDT_WRAP 213
IDT_WRAP 214
IDT_WRAP 215
IDT_WRAP 216
IDT_WRAP 217
IDT_WRAP 218
IDT_WRAP 219
IDT_WRAP 220
IDT_WRAP 221
IDT_WRAP 222
IDT_WRAP 223
IDT_WRAP 224
IDT_WRAP 225
IDT_WRAP 226
IDT_WRAP 227
IDT_WRAP 228
IDT_WRAP 229
IDT_WRAP 230
IDT_WRAP 231
IDT_WRAP 232
IDT_WRAP 233
IDT_WRAP 234
IDT_WRAP 235
IDT_WRAP 236
IDT_WRAP 237
IDT_WRAP 238
IDT_WRAP 239
IDT_WRAP 240
IDT_WRAP 241
IDT_WRAP 242
IDT_WRAP 243
IDT_WRAP 244
IDT_WRAP 245
IDT_WRAP 246
IDT_WRAP 247
IDT_WRAP 248
IDT_WRAP 249
IDT_WRAP 250
IDT_WRAP 251
IDT_WRAP 252
IDT_WRAP 253
IDT_WRAP 254
IDT_WRAP 255

