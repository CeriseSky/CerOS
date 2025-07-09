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

#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

// allows C to use the x86 outb and inb instructions

void io_outb(uint16_t port, uint8_t data);
uint8_t io_inb(uint16_t port);
void io_wait(void);

#endif

