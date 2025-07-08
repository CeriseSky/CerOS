#include <idt.h>
#include <stddef.h>
#include <vgatext.h>

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

void idt_defaultHandler(idt_stack_frame *stack) {
  uint8_t colour = vgaText_genColour(VGA_TEXT_WHITE, VGA_TEXT_BLUE);
  vgaText_clear(colour);
  vgaText_puts("Unhandled interrupt. Stopping kernel execution now.",
               0, 0, colour);

  // hacky way to print interrupt number in hex while there is no print_int
  // function
  vgaText_puts("Interrupt: 0x", 0, 1, colour);
  char characters[] = "0123456789ABCDEF";
  vgaText_putc(characters[stack->index >> 4], 13, 1, colour);
  vgaText_putc(characters[stack->index & 0xf], 14, 1, colour);

  while(1);
}

// generated with the genisr tool. Could be simplified with a macro but it would
// still be a gazillion lines
void idt_init(idt_t *idt, idt_idtr *idtr, uint16_t selector) {
  idtr->address = (uint64_t)idt;
  idtr->size = IDT_MAX_ENTRIES * sizeof(idt_entry) - 1;

  idt_setEntry(idt, 0, &idt_wrapper0, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 1, &idt_wrapper1, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 2, &idt_wrapper2, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 3, &idt_wrapper3, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 4, &idt_wrapper4, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 5, &idt_wrapper5, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 6, &idt_wrapper6, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 7, &idt_wrapper7, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 8, &idt_wrapper8, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 9, &idt_wrapper9, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 10, &idt_wrapper10, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 11, &idt_wrapper11, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 12, &idt_wrapper12, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 13, &idt_wrapper13, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 14, &idt_wrapper14, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 15, &idt_wrapper15, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 16, &idt_wrapper16, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 17, &idt_wrapper17, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 18, &idt_wrapper18, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 19, &idt_wrapper19, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 20, &idt_wrapper20, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 21, &idt_wrapper21, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 22, &idt_wrapper22, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 23, &idt_wrapper23, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 24, &idt_wrapper24, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 25, &idt_wrapper25, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 26, &idt_wrapper26, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 27, &idt_wrapper27, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 28, &idt_wrapper28, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 29, &idt_wrapper29, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 30, &idt_wrapper30, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 31, &idt_wrapper31, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_TRAP);
  idt_setEntry(idt, 32, &idt_wrapper32, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 33, &idt_wrapper33, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 34, &idt_wrapper34, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 35, &idt_wrapper35, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 36, &idt_wrapper36, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 37, &idt_wrapper37, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 38, &idt_wrapper38, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 39, &idt_wrapper39, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 40, &idt_wrapper40, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 41, &idt_wrapper41, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 42, &idt_wrapper42, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 43, &idt_wrapper43, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 44, &idt_wrapper44, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 45, &idt_wrapper45, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 46, &idt_wrapper46, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 47, &idt_wrapper47, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 48, &idt_wrapper48, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 49, &idt_wrapper49, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 50, &idt_wrapper50, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 51, &idt_wrapper51, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 52, &idt_wrapper52, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 53, &idt_wrapper53, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 54, &idt_wrapper54, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 55, &idt_wrapper55, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 56, &idt_wrapper56, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 57, &idt_wrapper57, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 58, &idt_wrapper58, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 59, &idt_wrapper59, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 60, &idt_wrapper60, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 61, &idt_wrapper61, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 62, &idt_wrapper62, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 63, &idt_wrapper63, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 64, &idt_wrapper64, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 65, &idt_wrapper65, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 66, &idt_wrapper66, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 67, &idt_wrapper67, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 68, &idt_wrapper68, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 69, &idt_wrapper69, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 70, &idt_wrapper70, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 71, &idt_wrapper71, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 72, &idt_wrapper72, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 73, &idt_wrapper73, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 74, &idt_wrapper74, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 75, &idt_wrapper75, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 76, &idt_wrapper76, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 77, &idt_wrapper77, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 78, &idt_wrapper78, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 79, &idt_wrapper79, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 80, &idt_wrapper80, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 81, &idt_wrapper81, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 82, &idt_wrapper82, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 83, &idt_wrapper83, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 84, &idt_wrapper84, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 85, &idt_wrapper85, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 86, &idt_wrapper86, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 87, &idt_wrapper87, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 88, &idt_wrapper88, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 89, &idt_wrapper89, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 90, &idt_wrapper90, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 91, &idt_wrapper91, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 92, &idt_wrapper92, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 93, &idt_wrapper93, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 94, &idt_wrapper94, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 95, &idt_wrapper95, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 96, &idt_wrapper96, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 97, &idt_wrapper97, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 98, &idt_wrapper98, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 99, &idt_wrapper99, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 100, &idt_wrapper100, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 101, &idt_wrapper101, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 102, &idt_wrapper102, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 103, &idt_wrapper103, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 104, &idt_wrapper104, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 105, &idt_wrapper105, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 106, &idt_wrapper106, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 107, &idt_wrapper107, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 108, &idt_wrapper108, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 109, &idt_wrapper109, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 110, &idt_wrapper110, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 111, &idt_wrapper111, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 112, &idt_wrapper112, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 113, &idt_wrapper113, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 114, &idt_wrapper114, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 115, &idt_wrapper115, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 116, &idt_wrapper116, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 117, &idt_wrapper117, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 118, &idt_wrapper118, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 119, &idt_wrapper119, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 120, &idt_wrapper120, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 121, &idt_wrapper121, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 122, &idt_wrapper122, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 123, &idt_wrapper123, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 124, &idt_wrapper124, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 125, &idt_wrapper125, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 126, &idt_wrapper126, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 127, &idt_wrapper127, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 128, &idt_wrapper128, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 129, &idt_wrapper129, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 130, &idt_wrapper130, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 131, &idt_wrapper131, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 132, &idt_wrapper132, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 133, &idt_wrapper133, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 134, &idt_wrapper134, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 135, &idt_wrapper135, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 136, &idt_wrapper136, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 137, &idt_wrapper137, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 138, &idt_wrapper138, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 139, &idt_wrapper139, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 140, &idt_wrapper140, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 141, &idt_wrapper141, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 142, &idt_wrapper142, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 143, &idt_wrapper143, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 144, &idt_wrapper144, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 145, &idt_wrapper145, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 146, &idt_wrapper146, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 147, &idt_wrapper147, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 148, &idt_wrapper148, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 149, &idt_wrapper149, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 150, &idt_wrapper150, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 151, &idt_wrapper151, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 152, &idt_wrapper152, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 153, &idt_wrapper153, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 154, &idt_wrapper154, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 155, &idt_wrapper155, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 156, &idt_wrapper156, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 157, &idt_wrapper157, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 158, &idt_wrapper158, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 159, &idt_wrapper159, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 160, &idt_wrapper160, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 161, &idt_wrapper161, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 162, &idt_wrapper162, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 163, &idt_wrapper163, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 164, &idt_wrapper164, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 165, &idt_wrapper165, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 166, &idt_wrapper166, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 167, &idt_wrapper167, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 168, &idt_wrapper168, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 169, &idt_wrapper169, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 170, &idt_wrapper170, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 171, &idt_wrapper171, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 172, &idt_wrapper172, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 173, &idt_wrapper173, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 174, &idt_wrapper174, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 175, &idt_wrapper175, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 176, &idt_wrapper176, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 177, &idt_wrapper177, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 178, &idt_wrapper178, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 179, &idt_wrapper179, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 180, &idt_wrapper180, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 181, &idt_wrapper181, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 182, &idt_wrapper182, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 183, &idt_wrapper183, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 184, &idt_wrapper184, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 185, &idt_wrapper185, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 186, &idt_wrapper186, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 187, &idt_wrapper187, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 188, &idt_wrapper188, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 189, &idt_wrapper189, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 190, &idt_wrapper190, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 191, &idt_wrapper191, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 192, &idt_wrapper192, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 193, &idt_wrapper193, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 194, &idt_wrapper194, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 195, &idt_wrapper195, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 196, &idt_wrapper196, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 197, &idt_wrapper197, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 198, &idt_wrapper198, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 199, &idt_wrapper199, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 200, &idt_wrapper200, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 201, &idt_wrapper201, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 202, &idt_wrapper202, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 203, &idt_wrapper203, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 204, &idt_wrapper204, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 205, &idt_wrapper205, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 206, &idt_wrapper206, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 207, &idt_wrapper207, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 208, &idt_wrapper208, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 209, &idt_wrapper209, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 210, &idt_wrapper210, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 211, &idt_wrapper211, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 212, &idt_wrapper212, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 213, &idt_wrapper213, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 214, &idt_wrapper214, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 215, &idt_wrapper215, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 216, &idt_wrapper216, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 217, &idt_wrapper217, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 218, &idt_wrapper218, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 219, &idt_wrapper219, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 220, &idt_wrapper220, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 221, &idt_wrapper221, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 222, &idt_wrapper222, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 223, &idt_wrapper223, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 224, &idt_wrapper224, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 225, &idt_wrapper225, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 226, &idt_wrapper226, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 227, &idt_wrapper227, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 228, &idt_wrapper228, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 229, &idt_wrapper229, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 230, &idt_wrapper230, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 231, &idt_wrapper231, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 232, &idt_wrapper232, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 233, &idt_wrapper233, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 234, &idt_wrapper234, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 235, &idt_wrapper235, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 236, &idt_wrapper236, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 237, &idt_wrapper237, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 238, &idt_wrapper238, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 239, &idt_wrapper239, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 240, &idt_wrapper240, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 241, &idt_wrapper241, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 242, &idt_wrapper242, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 243, &idt_wrapper243, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 244, &idt_wrapper244, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 245, &idt_wrapper245, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 246, &idt_wrapper246, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 247, &idt_wrapper247, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 248, &idt_wrapper248, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 249, &idt_wrapper249, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 250, &idt_wrapper250, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 251, &idt_wrapper251, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 252, &idt_wrapper252, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 253, &idt_wrapper253, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 254, &idt_wrapper254, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
  idt_setEntry(idt, 255, &idt_wrapper255, selector, 
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);
}

