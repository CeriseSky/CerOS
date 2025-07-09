#ifndef _IDT_H_
#define _IDT_H_

// tools for working with the x86_64 interrupt descriptor table.
// does not provide tools for IRQs

#include <stdint.h>
#include <idt_wrap.h>

typedef struct {
  uint16_t size;
  uint64_t address;
} __attribute__((packed)) idt_idtr;

enum idt_flags {
  IDT_PRESENT = 1 << 7,

  IDT_RING0 = 0 << 5,
  IDT_RING1 = 1 << 5,
  IDT_RING2 = 2 << 5,
  IDT_RING3 = 3 << 5,

  IDT_INTERRUPT = 0xe,
  IDT_TRAP = 0xf,
};

typedef struct {
  uint16_t address_low;
  uint16_t selector;
  uint8_t ist;
  uint8_t flags;
  uint16_t address_mid;
  uint32_t address_high;
  uint32_t _reserved;
} __attribute__((packed)) idt_entry;

#define IDT_MAX_ENTRIES 256
typedef idt_entry idt_t[IDT_MAX_ENTRIES];

void idt_clear(idt_t *idt);
void idt_setEntry(idt_t *idt, uint8_t index, void *addr, uint16_t selector,
                  uint8_t flags);

void idt_lidt(idt_idtr *descriptor);
void idt_call(uint8_t index);   // equivalent to asm: int index
void idt_cli();
void idt_sti();

void idt_init(idt_t *idt, idt_idtr *idtr, uint16_t selector);

typedef struct {
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8,
           rdi, rsi, rdx, rcx, rbx, rax,
           rbp,
           index, errorCode,
           rip, cs, rflags, rsp, ss;
} __attribute__((packed)) idt_stackFrame;
typedef void (*idt_handler)(idt_stackFrame *);
void idt_defaultHandler(idt_stackFrame *stack);

extern idt_handler idt_customHandlers[256];

#endif

