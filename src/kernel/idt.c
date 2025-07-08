#include <idt.h>
#include <stddef.h>

void idt_setEntry(idt_t *idt, uint8_t index, void *addr, uint16_t selector,
                  uint8_t flags) {
  (*idt)[index].address_low = (uint64_t)addr & 0xffff;
  (*idt)[index].selector = selector;
  (*idt)[index].ist = 0;
  (*idt)[index].flags = flags;
  (*idt)[index].address_mid = (uint64_t)addr >> 16 & 0xffff;
  (*idt)[index].address_high = (uint64_t)addr >> 32;
  (*idt)[index]._reserved = 0;
}

void idt_clear(idt_t *idt) {
  for(int i = 0; i < IDT_MAX_ENTRIES; i++)
    idt_setEntry(idt, i, NULL, 0, 0);
}

