#include <irq.h>
#include <vgatext.h>

irq_handler irq_handlers[PIC_IRQS_PER_CHIP * PIC_COUNT];
static uint8_t irq_startingInt;

void irq_defaultHandler(idt_stackFrame *stack) {
  if(irq_handlers[stack->index - irq_startingInt]) {
    irq_handlers[stack->index - irq_startingInt](stack);
    pic_sendEOI(stack->index - irq_startingInt);
    return;
  }

  static uint8_t logColour = 0;
  if(logColour) {
    vgaText_colour bg = (vgaText_getBackground(logColour) + 1) % 16;
    vgaText_colour fg = 15 - bg;
    logColour = vgaText_genColour(fg, bg);
  } else
    logColour = vgaText_genColour(VGA_TEXT_BLACK, VGA_TEXT_WHITE);

  vgaText_puts("Unhandled IRQ: Ignoring...", 0, VGA_TEXT_ROWS - 1, logColour);

  pic_sendEOI(stack->index - irq_startingInt);
}

void irq_init(uint8_t startingInt) {
  irq_startingInt = startingInt;
  for(uint8_t i = startingInt;
      i < startingInt + PIC_IRQS_PER_CHIP * PIC_COUNT;
      i++)
    idt_customHandlers[i] = irq_defaultHandler;
}

