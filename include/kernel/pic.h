#ifndef _PIC_H_
#define _PIC_H_

#include <stdint.h>

enum pic_port {
  PIC_MASTER_COMMAND = 0x20,
  PIC_MASTER_DATA,

  PIC_SLAVE_COMMAND = 0xa0,
  PIC_SLAVE_DATA,
};

enum pic_commands {
  PIC_EOI = 0x20,
};

void pic_sendEOI(uint8_t irq);

// masterInt is the starting interrupt number for IRQs 0-7
// slaveInt is the starting interrupt number fo IRQs 8-15
void pic_init(uint8_t masterInt, uint8_t slaveInt);

enum pic_icw1 {
  PIC_ICW1_INIT = 1 << 0,
  PIC_ICW1_ICW4 = 1 << 4,        // Makes PIC expect ICW4
};

enum pic_icw4 {
  PIC_ICW4_8086_MODE = 1 << 0,
};

#define PIC_IRQS_PER_CHIP 8
#define PIC_CASCADE_IRQ 2
#define PIC_COUNT 2   // number of PICs in most systems

#endif

