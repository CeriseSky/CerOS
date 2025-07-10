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
#include <bss.h>
#include <pic.h>
#include <irq.h>
#include <thread.h>
#include <memory.h>
#include <io.h>

#ifndef COMMIT_HASH
#define COMMIT_HASH "0"
#endif

#define MSG_LOG "[LOG] - "
#define MSG_ERR "[ERROR] - "

static idt_t idt;
static idt_idtr idtr;

static volatile int counter = 0;

#define MT_QUANTUM 5
thread_t threads[4];
size_t numThreads = sizeof(threads)/sizeof(thread_t);
int currentThread = -1;

void taskSwitchHandler(idt_stackFrame *stack);

static int alreadySwitching = 0;
static int systemTime = 0, processTime = 0, idleTime = 0, totalTime = 0;

void thread_sleep(int ms);

static int unresolved = 0;

void timerHandler(idt_stackFrame *stack) {
  counter++;
  vgaText_putc('0' + threads[0].requestAge, 30, 0, 0x0d);
  vgaText_putc('0' + threads[1].requestAge, 31, 0, 0x0d);
  vgaText_putc('0' + currentThread, 32, 0, 0x0d);

  totalTime++;
  if(currentThread < 0 || unresolved > 0)
    systemTime++;
  else if(currentThread == 0)
    idleTime++;
  else
    processTime++;

  unresolved++;

  int total = totalTime;
  vgaText_puts("CPU Total Time: 000ms", 0, VGA_TEXT_ROWS - 4, 0x0d);
  vgaText_putc('0' + (total % 10), 18, VGA_TEXT_ROWS - 4,
               0x0d);
  total /= 10;
  vgaText_putc('0' + (total % 10), 17, VGA_TEXT_ROWS - 4,
               0x0d);
  total /= 10;
  vgaText_putc('0' + (total % 10), 16, VGA_TEXT_ROWS - 4,
               0x0d);

  if(totalTime >= 1000) {
    vgaText_puts("CPU Usage: 000%", 0, VGA_TEXT_ROWS - 2,
                 0x0d);
    int usage = (100 * (totalTime - idleTime)) / totalTime;
    vgaText_putc('0' + (usage % 10), 13, VGA_TEXT_ROWS - 2,
                 0x0d);
    usage /= 10;
    vgaText_putc('0' + (usage % 10), 12, VGA_TEXT_ROWS - 2,
                 0x0d);
    usage /= 10;
    vgaText_putc('0' + (usage % 10), 11, VGA_TEXT_ROWS - 2,
                 0x0d);

    usage = idleTime;
    vgaText_puts("CPU Idle Time: 0000ms", 0, VGA_TEXT_ROWS - 3, 0x0d);
    vgaText_putc('0' + (usage % 10), 18, VGA_TEXT_ROWS - 3,
                 0x0d);
    usage /= 10;
    vgaText_putc('0' + (usage % 10), 17, VGA_TEXT_ROWS - 3,
                 0x0d);
    usage /= 10;
    vgaText_putc('0' + (usage % 10), 16, VGA_TEXT_ROWS - 3,
                 0x0d);
    usage /= 10;
    vgaText_putc('0' + (usage % 10), 15, VGA_TEXT_ROWS - 3,
                 0x0d);

    totalTime = 0;
    idleTime = 0;
    systemTime = 0;
    processTime = 0;

  }

  for(size_t i = 0; i < numThreads; i++)
    if(threads[i].sleepLeft > 0)
      threads[i].sleepLeft--;

  unresolved--;
  if(counter >= MT_QUANTUM && !alreadySwitching) {
    counter = 0;

    taskSwitchHandler(stack);
  }

  return;
}

#define TASK_SWITCH_INT 0x80
void taskSwitchHandler(idt_stackFrame *stack) {
  if(alreadySwitching)
    return;

  unresolved++;

  alreadySwitching = 1;

  if(currentThread >= 0)
    memcpy(&threads[currentThread].state, stack, sizeof(idt_stackFrame));

  int newThread = threadChooser(stack, threads, numThreads);
  currentThread = newThread >= 0 ? newThread : 0;

  if(currentThread == 0)
    memcpy(stack, &threads[0].state, sizeof(idt_stackFrame));

  unresolved--;
  alreadySwitching = 0;
}

#define IRQ_START 0x20

uint8_t thread0_stack[1024];
void thread0();
uint8_t thread1_stack[1024];
void thread1();
uint8_t thread2_stack[1024];
void thread2();
uint8_t thread3_stack[1024];
void thread3();

void thread_sleep(int ms) {
  if(currentThread <= 0)
    return;

  threads[currentThread].sleepLeft = ms;

  idt_call(TASK_SWITCH_INT);
}

void kmain(uint16_t selector, uint16_t dataSelector) {
  bss_clear();

  uint8_t logColour = vgaText_genColour(VGA_TEXT_WHITE, VGA_TEXT_BLACK);
  // uint8_t errColour = vgaText_genColour(VGA_TEXT_RED, VGA_TEXT_WHITE);

  vgaText_disableCursor();
  vgaText_clear(logColour);
  vgaText_puts("PrOS Kernel Commit " COMMIT_HASH, 0, 0, logColour);

  vgaText_puts(MSG_LOG "Initialising IDT", 0, 1, logColour);

  idt_init(&idt, &idtr, selector);
  idt_lidt(&idtr);

  vgaText_puts(MSG_LOG "Initialising IRQs (PIC)", 0, 2, logColour);
  pic_init(IRQ_START, IRQ_START + PIC_IRQS_PER_CHIP);

  vgaText_puts(MSG_LOG "Initialising threads", 0, 3, logColour);

  idt_customHandlers[TASK_SWITCH_INT] = taskSwitchHandler;

  irq_init(IRQ_START);
  irq_handlers[IRQ0_PIT] = timerHandler;
  threads[0].state.cs = selector;
  threads[0].state.ss = dataSelector;
  threads[0].state.rsp = (uint64_t)thread0_stack + 1024;
  threads[0].state.rbp = (uint64_t)thread0_stack + 1024;
  threads[0].state.rip = (uint64_t)thread0;
  threads[0].state.rflags = 0x202;
  threads[0].requestAge = 0;
  threads[0].present = 1;

  threads[1].state.cs = selector;
  threads[1].state.ss = dataSelector;
  threads[1].state.rsp = (uint64_t)thread1_stack + 1024;
  threads[1].state.rbp = (uint64_t)thread1_stack + 1024;
  threads[1].state.rip = (uint64_t)thread1;
  threads[1].state.rflags = 0x202;
  threads[1].requestAge = 0;
  threads[1].present = 1;

  threads[2].state.cs = selector;
  threads[2].state.ss = dataSelector;
  threads[2].state.rsp = (uint64_t)thread2_stack + 1024;
  threads[2].state.rbp = (uint64_t)thread2_stack + 1024;
  threads[2].state.rip = (uint64_t)thread2;
  threads[2].state.rflags = 0x202;
  threads[2].requestAge = 0;
  threads[2].present = 1;

  threads[3].state.cs = selector;
  threads[3].state.ss = dataSelector;
  threads[3].state.rsp = (uint64_t)thread3_stack + 1024;
  threads[3].state.rbp = (uint64_t)thread3_stack + 1024;
  threads[3].state.rip = (uint64_t)thread3;
  threads[3].state.rflags = 0x202;
  threads[3].requestAge = 0;
  threads[3].present = 1;

  #define DIVISOR 1193
  io_outb(0x43, 0x34);
  io_outb(0x40, DIVISOR & 0xff);
  io_outb(0x40, DIVISOR >> 8);
  idt_sti();

  // tmp
  while(1);

  return;
}

void thread1() {
  uint8_t logColour = vgaText_genColour(VGA_TEXT_WHITE, VGA_TEXT_BLACK);
  while(1) {
    for(int i = 0; i < 10000; i++) {
      vgaText_colour bg = (vgaText_getBackground(logColour) + 1) % 16;
      vgaText_colour fg = 15 - bg;
      logColour = vgaText_genColour(fg, bg);
      vgaText_puts("Hello from thread 1", 0, 11, logColour);
    }

    thread_sleep(100);
  }
}

void thread2() {
  uint8_t logColour = vgaText_genColour(VGA_TEXT_WHITE, VGA_TEXT_BLACK);
  while(1) {
    vgaText_colour bg = (vgaText_getBackground(logColour) + 1) % 16;
    vgaText_colour fg = 15 - bg;
    logColour = vgaText_genColour(fg, bg);
    vgaText_puts("Hello from thread 2", 0, 12, logColour);

    thread_sleep(200);
  }
}

void thread3() {
  uint8_t logColour = vgaText_genColour(VGA_TEXT_WHITE, VGA_TEXT_BLACK);
  while(1) {
    vgaText_colour bg = (vgaText_getBackground(logColour) + 1) % 16;
    vgaText_colour fg = 15 - bg;
    logColour = vgaText_genColour(fg, bg);
    vgaText_puts("Hello from thread 3", 0, 13, logColour);

    thread_sleep(500);
  }
}

