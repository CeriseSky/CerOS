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

#ifndef _VGA_TEXT_H_
#define _VGA_TEXT_H_

#include <stdint.h>

// contains tools for working with vga text mode

typedef struct {
  char glyph;
  uint8_t colour;
} __attribute__((packed)) vgaText_char;

typedef enum {
  VGA_TEXT_BLACK = 0,
  VGA_TEXT_BLUE,
  VGA_TEXT_GREEN,
  VGA_TEXT_CYAN,
  VGA_TEXT_RED,
  VGA_TEXT_PURPLE,
  VGA_TEXT_BROWN,
  VGA_TEXT_GREY,
  VGA_TEXT_DARK_GREY,
  VGA_TEXT_LIGHT_BLUE,
  VGA_TEXT_LIGHT_GREEN,
  VGA_TEXT_LIGHT_CYAN,
  VGA_TEXT_LIGHT_RED,
  VGA_TEXT_LIGHT_PURPLE,
  VGA_TEXT_YELLOW,
  VGA_TEXT_WHITE,
} vgaText_colour;

// This is preferable to a macro to me because of type safety
// and... not being a macro
static inline uint8_t vgaText_genColour(vgaText_colour fg,
                                        vgaText_colour bg) {
  return bg << 4 | fg;
}

static inline vgaText_colour vgaText_getBackground(uint8_t colour) {
  return colour >> 4;
}
static inline vgaText_colour vgaText_getForeground(uint8_t colour) {
  return colour & 0xf;
}

#define VGA_TEXT_ROWS 25
#define VGA_TEXT_COLS 80
typedef vgaText_char vgaText_buffer[VGA_TEXT_ROWS][VGA_TEXT_COLS];
#define VGA_TEXT_BUFFER (*(volatile vgaText_buffer *)0xb8000)

void vgaText_putc(char c, uint8_t x, uint8_t y, uint8_t colour);
void vgaText_puts(char *s, uint8_t x, uint8_t y, uint8_t colour);
void vgaText_clear(uint8_t colour);
void vgaText_disableCursor(void);

#endif

