#include <pic.h>
#include <io.h>

void pic_init(uint8_t masterInt, uint8_t slaveInt) {
  io_outb(PIC_MASTER_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
  io_wait();
  io_outb(PIC_SLAVE_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
  io_wait();

  io_outb(PIC_MASTER_DATA, masterInt);
  io_wait();
  io_outb(PIC_SLAVE_DATA, slaveInt);
  io_wait();

  io_outb(PIC_MASTER_DATA, 1 << PIC_CASCADE_IRQ);
  io_wait();
  io_outb(PIC_SLAVE_DATA, PIC_CASCADE_IRQ);
  io_wait();

  io_outb(PIC_MASTER_DATA, PIC_ICW4_8086_MODE);
  io_wait();
  io_outb(PIC_SLAVE_DATA, PIC_ICW4_8086_MODE);
  io_wait();
}

void pic_sendEOI(uint8_t irq) {
  if(irq >= PIC_IRQS_PER_CHIP)
    io_outb(PIC_SLAVE_COMMAND, PIC_EOI);

  // Master PIC still needs EOI command even if slave PIC called the interrupt
  io_outb(PIC_MASTER_COMMAND, PIC_EOI);
}

