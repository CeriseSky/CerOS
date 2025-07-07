# CerOS

CerOS is going to be a hobby OS where all of its components are home-baked
specifically for this project. It is designed to be as modular as possible,
likely implementing a microkernel, so that modding it is as easy as it can
be, and entirely different OSes can be made using CerOS as a template

## System Requirements

- Floppy diskette with a gendisk implementation (see docs/gendisk)
- PC compatible BIOS/CSM
- 64KiB RAM
- Any x86_64 CPU

## Installation

### Requirements

Building CerOS requires gcc, make, and mtools. Optionally, bochs can be
installed for debugging, though that isn't covered here. Distributions
marked with a * have not been tested.

#### * Debian

```
sudo apt update
sudo apt install mtools build-essential
```

#### * Gentoo

```
sudo emerge --ask sys-devel/gcc dev-build/make sys-fs/mtools
```

#### Arch

```
sudo pacman -Sy base-devel mtools
```

#### * Fedora

```
sudo dnf install gcc make mtools
```

### Building

By default, CerOS builds for a 160KiB floppy diskette. If that is fine (it
usually is if you aren't running on physical hardware), just use the following
command:

```
make
```

This will create the disk image in bin/disk.img, ready to be booted in qemu or
flashed to real hardware. This also builds the croses compiler on the first build
which might take a really long time so running parallel make is recommended:

```
make -j$(nproc)
```

### Creating the tools

The build process will create the tools automatically, but if you need to use
them manually, run this command:

```
make tools
```

This will create all the tools in the tools/bin directory. This is the rule that
creates the cross compiler, meaning it should also be run in parallel to speed
it up.

### Custom disk parameters

makefile variables are used to configure the disk parameters. Use this syntax
to change the options:

```
make OPTION=value OPTION2=value ...
```

The following options are supported:

```
DISK_TYPE - The disk type to build for (run gendisk help for options)
DISK_BOOT_PATH - the path on the target disk that the boot file should be
                 (Warning: The makefile will NOT put the default boot file in
                  this path. You will need to put a file there manually)
DISK_LABEL - The name of the target disk. Will be truncated to 11 characters
DISK_OEM - the name of the disk manufacturer/distributor. Truncated to 8 bytes
DISK_SERIAL - Serial number of the disk. Truncated to a 32 bit integer
```

DISK_LABEL does support spaces in the following format:

```
make DISK_LABEL="hello\\ world"
```

