/*
  This file is part of CerOS
  Copyright (C) 2025 Indigo Carmine (CeriseSky)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <vgatext.h>
#include <idt.h>

#ifndef COMMIT_HASH
#define COMMIT_HASH "0"
#endif

#define MSG_LOG "[LOG] - "
#define MSG_ERR "[ERROR] - "

void interrupt_wrapper(void);
static idt_t idt;
static idt_idtr idtr;

void kmain(uint16_t selector) {
  uint8_t logColour = vgaText_genColour(VGA_TEXT_WHITE, VGA_TEXT_BLACK);
  // uint8_t errColour = vgaText_genColour(VGA_TEXT_RED, VGA_TEXT_WHITE);

  vgaText_disableCursor();
  vgaText_clear(logColour);
  vgaText_puts("PrOS Kernel Commit " COMMIT_HASH, 0, 0, logColour);

  vgaText_puts(MSG_LOG "Initialising IDT", 0, 1, logColour);

  idt_clear(&idt);
  idt_setEntry(&idt, 0x20, (void *)interrupt_wrapper, selector,
               IDT_PRESENT | IDT_RING0 | IDT_INTERRUPT);

  idtr.size = sizeof(idt) - 1;
  idtr.address = (uint64_t)&idt;
  idt_lidt(&idtr);

  idt_call(0x20);
  vgaText_puts("Interrupt returned", 0, 2, logColour);

  return;
}

void interrupt_handler(void) {
  vgaText_puts("Interrupt Called", 0, VGA_TEXT_ROWS - 1,
               vgaText_genColour(VGA_TEXT_WHITE, VGA_TEXT_RED));
  return;
}

