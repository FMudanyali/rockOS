#!/bin/sh
#export PREFIX="$HOME/opt/cross-i686"

echo "Welcome to rockOS toolchain compilation script, where would you like to install your toolchain?\nLeave blank for $HOME/opt/cross-i686"

read PREFIX

if [ "$PREFIX" == "" ]; then
    export PREFIX="$HOME/opt/cross-i686"
fi

mkdir -p "$PREFIX"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

export BINUTILS_LINK=https://ftp.gnu.org/gnu/binutils/binutils-2.38.tar.gz
export GCC_LINK=https://ftp.gnu.org/gnu/gcc/gcc-12.1.0/gcc-12.1.0.tar.gz
export GRUB_LINK=https://ftp.gnu.org/gnu/grub/grub-2.06.tar.gz

if command -V wget >/dev/null 2>&1; then
    export DL_CMD="wget"
    export DL_OUTPUT="-O"
elif command -V curl >/dev/null 2>&1; then
    export DL_CMD="curl"
    export DL_OUTPUT="-o"
else
    echo "Please install curl or wget"
    exit 1
fi

mkdir -p "$PREFIX"
cd /tmp
mkdir rockos-toolchain-src
cd rockos-toolchain-src
"${DL_CMD}" $DL_OUTPUT binutils.tar.gz $BINUTILS_LINK
mkdir binutils-src
tar -xvf binutils.tar.gz -C ./binutils-src/
mkdir build-binutils
cd build-binutils
../binutils-src/*/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make install

cd ..
"${$DL_CMD}" $DL_OUTPUT gcc.tar.gz $GCC_LINK
mkdir gcc-src
tar -xvf gcc.tar.gz -C ./gcc-src/
mkdir build-gcc
cd build-gcc
../gcc-src/*/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc
cd ..

if [ "$(uname)" == "Darwin" ]; then
    "${DL_CMD}" $DL_OUTPUT grub.tar.gz $GRUB_LINK
    mkdir grub-src
    tar -xvf grub.tar.gz -C ./grub-src/
    mkdir build-grub
    cd build-grub
    ../grub-src/*/configure --target=$TARGET --prefix="$PREFIX" TARGET_CC=$TARGET-gcc TARGET_OBJCOPY=$TARGET-objcopy TARGET_STRIP=$TARGET-strip TARGET_NM=$TARGET-nm TARGET_RANLIB=$TARGET-ranlib --disable-nls --disable-werror
    make -j$(nproc)
    make install
    cd ..
fi

rm -r rockos-toolchain-src
cd

echo "Toolchain installation is complete!"
echo "Please add this to your bashrc or zshrc or whatever"
echo "export \$PATH=\"$PREFIX/bin:\$PATH\""