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

#ifndef COMMIT_HASH
#define COMMIT_HASH "0"
#endif

void kmain(void) {
  uint8_t defColour = vgaText_genColour(VGA_TEXT_WHITE, VGA_TEXT_BLACK);
  vgaText_disableCursor();
  vgaText_clear(defColour);

  vgaText_puts("CerOS Kernel Commit " COMMIT_HASH, 0, 0, defColour);

  while(1);
}

