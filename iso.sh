#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/rockos.kernel isodir/boot/rockos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "rockOS" {
    multiboot /boot/rockos.kernel
}
EOF
"${GRUB_MKRESCUE}" -o rockos.iso isodir
