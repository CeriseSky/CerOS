# This file is part of the CerOS Tools
# Copyright (C) 2025 Indigo Carmine (CeriseSky)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

#!/bin/bash

# builds the gcc & binutils cross compiler
# run in the project root

set -e

# these packages are needed and must be installed by root
pacman -Q base-devel gmp libmpc mpfr

PREFIX=$PWD/tools/xcompile
TARGET=x86_64-elf

mkdir -p tools/xcompile
pushd tools/xcompile

curl https://ftp.gnu.org/gnu/binutils/binutils-2.44.tar.xz -o binutils.tar.xz
curl https://ftp.gnu.org/gnu/gcc/gcc-15.1.0/gcc-15.1.0.tar.xz -o gcc.tar.xz

tar xfv binutils.tar.xz
tar xfv gcc.tar.xz

pushd binutils-2.44
mkdir build
cd build
../configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls \
             --disable-werror
make -j$(nproc)
make install
popd

pushd gcc-15.1.0
mkdir build
cd build
../configure --target=$TARGET --prefix="$PREFIX" --disable-nls \
             --enable-languages=c,c++ --without-headers \
             --disable-hosted-libstdcxx
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make -j$(nproc) all-target-libstdc++-v3
make install-gcc
make install-target-libgcc
make install-target-libstdc++-v3
popd

popd

