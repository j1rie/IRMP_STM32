#! /bin/bash
make clean-objects
make -j3 Bootloader=1 SimpleCircuit=1 Platform=blueDev
make clean-objects
make -j3 Bootloader=1 Platform=blueDev
make clean-objects
make -j3 SimpleCircuit=1 Platform=blueDev
make clean-objects
make -j3 Platform=blueDev
make clean-objects
rm *.elf
rm *.map
