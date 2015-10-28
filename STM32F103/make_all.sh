#! /bin/bash
make clean-objects
make Bootloader=1 SimpleCircuit=1 Platform=Dev
make clean-objects
make Bootloader=1 SimpleCircuit=1 Platform=Red
make clean-objects
make Bootloader=1 SimpleCircuit=1 Platform=Blue
make clean-objects
make Bootloader=1 Platform=Dev
make clean-objects
make Bootloader=1 Platform=Red
make clean-objects
make Bootloader=1 Platform=Blue
make clean-objects
make SimpleCircuit=1 Platform=Dev
make clean-objects
make SimpleCircuit=1 Platform=Red
make clean-objects
make SimpleCircuit=1 Platform=Blue
make clean-objects
make Platform=Red
make clean-objects
make Platform=Blue
make clean-objects
make Platform=Dev
rm *.elf
rm *.map
