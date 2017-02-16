#! /bin/bash
make clean-objects
make -j3 Bootloader=1 Platform=F105
make clean-objects
make -j3 Platform=F105
make clean-objects
rm *.elf
rm *.map
