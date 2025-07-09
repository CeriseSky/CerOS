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

ENTRY(init)
OUTPUT_FORMAT(binary)

SECTIONS {
  . = 0x8000;

  .text : { *(.text) }
  .data : { *(.data) }
  .rodata : { *(.rodata*) }

  _image_end = .;

  _bss_start = .;
  .bss : { *(.bss) }
  _bss_end = .;

  _end = .;
}

