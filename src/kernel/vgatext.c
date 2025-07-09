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

#include <vgatext.h>
#include <io.h>

void vgaText_putc(char c, uint8_t x, uint8_t y, uint8_t colour) {
  if(y >= VGA_TEXT_ROWS || x >= VGA_TEXT_COLS)
    return;

  VGA_TEXT_BUFFER[y][x].glyph = c;
  VGA_TEXT_BUFFER[y][x].colour = colour;
}

void vgaText_puts(char *s, uint8_t x, uint8_t y, uint8_t colour) {
  for(;*s; s++)
    vgaText_putc(*s, x++, y, colour);
}

void vgaText_clear(uint8_t colour) {
  for(int x = 0; x < VGA_TEXT_COLS; x++)
    for(int y = 0; y < VGA_TEXT_ROWS; y++) {
      VGA_TEXT_BUFFER[y][x].glyph = 0;
      VGA_TEXT_BUFFER[y][x].colour = colour;
    }
}

// Apparently, the CRTC registers can also be at 0x3bX but this works until
// a VGA driver is properly implemented.
// Conditional compilation allows the code to be moved somewhere else in
// the future.
#ifndef VGA_CRTC
#define VGA_CRTC
  #define VGA_CRTC_ADDRESS_PORT 0x3d4
  #define VGA_CRTC_DATA_PORT 0x3d5

  enum vga_crtRegs {
    VGA_CRTC_CURSOR_START = 0xa,
  };

  enum vga_cursorShapes {
    VGA_CURSOR_SHAPE_OFF = 0x20,
  };
#endif

void vgaText_disableCursor(void) {
  io_outb(VGA_CRTC_ADDRESS_PORT, VGA_CRTC_CURSOR_START);
  io_outb(VGA_CRTC_DATA_PORT, VGA_CURSOR_SHAPE_OFF);
}

