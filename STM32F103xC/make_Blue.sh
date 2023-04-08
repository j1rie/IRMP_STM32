#! /bin/bash
make clean-objects
make -j3 Bootloader=1 SimpleCircuit=1 Platform=Blue
#make Bootloader=1 SimpleCircuit=1 Platform=blackDev clean-objects
#make clean-objects
rm *.elf
rm *.map
