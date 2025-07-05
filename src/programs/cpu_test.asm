; tests the CPU for the following features:
; - 32 bit support
; - cpuid instruction support
; - 64 bit support
; - pml5 support

COM_OFFSET equ 100h

EFLAGS_ID equ 1 << 21         ; if this bit can be flipped, the CPUID
                              ; instruction is available

CPUID_GET_FEATURES equ 7
CPUID_EXTENSIONS equ 0x80000000 ; returns the maximum extended request for cpuid
CPUID_EXT_FEATURES equ 0x80000001 ; returns feature flags such as long mode
                                  ; support and other things

CPUID_FEATURE_PML5 equ 1 << 16

CPUID_EDX_EXT_FEAT_LM equ 1 << 29 ; if this bit is set, the CPU supports long
                                  ; mode.

bits 16
cpu 8086
org COM_OFFSET

main:
  mov si, test_start
  call puts

  ; attempts to use the fs register which is only available on 386 and beyond.
  ; sets ax to ds, fs to ds, and bx to fs in order, then if ax == bx then fs
  ; was properly set and 32 bit mode is supported.
  mov si, _386_test
  .386:
    call puts
    mov ax, ds
    push ds

    cpu 386
    pop fs
    mov bx, fs
    cpu 8086

    cmp bx, ax
    jne .nosupport
  .386_support:
    cpu 386
    mov si, test_success
    call puts

  mov si, _cpuid_test
  call puts

  ; attempts to flip the id flag in the eflags register. If it can without the
  ; CPU resetting it, then cpuid is supported
  .cpuid:
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

    xor eax, ecx
    jz .nosupport
  .cpuid_supported:
    cpu 586
    mov si, test_success
    call puts

  mov si, _long_mode_test
  call puts
  .long_mode:
    mov eax, CPUID_EXTENSIONS
    cpuid
    cmp eax, CPUID_EXT_FEATURES
    jb .nosupport

    mov eax, CPUID_EXT_FEATURES
    cpuid
    test edx, CPUID_EDX_EXT_FEAT_LM
    jz .nosupport
  .long_mode_supported:
    mov si, test_success
    call puts

  mov si, _pml5_test
  call puts
  .pml5:
    mov eax, CPUID_GET_FEATURES
    xor ecx, ecx
    cpuid
    test ecx, CPUID_FEATURE_PML5
    jz .nosupport
    mov si, test_success
    call puts

  ; set up code would go here
  jmp .hang

  cpu 8086
  .nosupport:
    mov si, test_fail
    call puts
  .hang:
    mov si, exec_stopped
    call puts
    .hang.loop:
      jmp .hang.loop

test_start: db "Running CPU feature test suite", 0ah, 0dh, 0
test_success: db "Passed", 0ah, 0dh, 0
test_fail: db "Failed", 0ah, 0dh, 0

exec_stopped: db "Execution has stopped", 0ah, 0dh, 0
invalid_opcode: db "Invalid opcode error", 0ah, 0dh, 0

_386_test: db "Testing 32-bit support: ", 0
_cpuid_test: db "Testing CPUID support: ", 0
_long_mode_test: db "Testing 64-bit support: ", 0
_pml5_test: db "Testing PML5 support: ", 0

puts:
  push si
  push bx
  xor bx, bx
  .loop:
    lodsb
    test al, al
    jz .end

    mov ah, 14
    int 10h

    jmp .loop
  .end:
    pop bx
    pop si
    ret

