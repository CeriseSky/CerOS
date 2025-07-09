#ifndef _IRQ_H_
#define _IRQ_H_

// functions for interacting with hardware that triggers interrupts

#include <idt.h>
#include <stdint.h>
#include <pic.h>

#define IRQ0_PIT 0

// arguments will be added as needed. This is because they might need more
// information than idt handlers would
typedef void (*irq_handler)(idt_stackFrame *stack);

// can be set by the OS.
extern irq_handler irq_handlers[PIC_IRQS_PER_CHIP * PIC_COUNT];

// startingInt is the expected interrupt number of IRQ0
// The irq handlers will take up the next 16 entries in the IDT
void irq_init(uint8_t startingInt);

void irq_defaultHandler(idt_stackFrame *stack);

#endif

