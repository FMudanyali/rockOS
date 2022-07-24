# rockOS
Welcome to rockOS project, a hobby OS that aims to run rogue and nethack.

## Currently implemented
- GDT
- IDT
- PIC initialization
- PIT timer
- Keyboard driver
- Paging and Kheap


## Usage
Just grab the latest ISO from the releases and run it on a virtual machine
such as QEMU or Virtualbox, bare-metal not tested.

## Build
### Dependencies
- a decent compiler such as gcc or clang
- grub2 (if under GNU/Linux)
- xorriso
- make
- tar (for compiling the toolchain)
- wget/curl (for compiling the toolchain)

If it is your first time building, you need to compile the toolchain first.
For that, you just need to run build-toolchain.sh, which will download binutils,
gcc, grub if youre on macOS, and then compile them to $HOME/opt/cross-i686 by default
which can be changed from the prompt.

After you compiled the toolchain and added $PREFIX/bin to your PATH as instructed by the script,
you can run qemu.sh to compile and open the OS on QEMU or iso.sh to just generate the ISO.


## Credits
- Furkan Mudanyalı
- Uko Kokņevičs


## Acknowledgements
- osdev.org community for huge documentation on OS Development
- musl for libc implementation
- James Molloy for his paging and kheap tutorial
- Free Software Foundation for multiboot specification


## License
This project is licensed under GPLv3.0 or later.