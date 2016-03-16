#!/bin/bash

# Build
cd src
sudo schroot -p -- make
cd ../

# Update image
sudo losetup /dev/loop0 floppy.img
sudo mount /dev/loop0 /mnt
sudo cp src/kernel /mnt/kernel
sudo umount /dev/loop0
sudo losetup -d /dev/loop0

# Clean
cd src
sudo make clean
cd ../

# Run in QEMU
qemu-system-i386 -fda floppy.img
