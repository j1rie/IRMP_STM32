#! /bin/bash
make clean-objects
make Bootloader=1 SimpleCircuit=1 Platform=Dev -j3
make clean-objects
make Bootloader=1 SimpleCircuit=1 Platform=Red -j3
make clean-objects
make Bootloader=1 SimpleCircuit=1 Platform=Blue -j3
make clean-objects
make Bootloader=1 Platform=Dev -j3
make clean-objects
make Bootloader=1 Platform=Red -j3
make clean-objects
make Bootloader=1 Platform=Blue -j3
make clean-objects
make SimpleCircuit=1 Platform=Dev -j3
make clean-objects
make SimpleCircuit=1 Platform=Red -j3
make clean-objects
make SimpleCircuit=1 Platform=Blue -j3
make clean-objects
make Platform=Red -j3
make clean-objects
make Platform=Blue -j3
make clean-objects
make Platform=Dev -j3
rm *.elf
rm *.map
