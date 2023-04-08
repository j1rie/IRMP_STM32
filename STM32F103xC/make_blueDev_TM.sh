#! /bin/bash
make clean-objects
make -j3 Bootloader=1 SimpleCircuit=1 Platform=blueDev TM1637=1
#make Bootloader=1 SimpleCircuit=1 Platform=blackDev clean-objects
make clean-objects TM1637=1
rm *.elf
rm *.map
