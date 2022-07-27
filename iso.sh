#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/rockos.kernel isodir/boot/rockos.kernel
tar -cvf sysroot/boot/rockos.initrd fs/*
cp sysroot/boot/rockos.initrd isodir/boot/rockos.initrd
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "rockOS" {
    multiboot /boot/rockos.kernel
    module /boot/rockos.initrd
}
EOF
"${GRUB_MKRESCUE}" -o rockos.iso isodir
