#! /bin/bash
make clean-objects
make -j3 Bootloader=1 SimpleCircuit=1 Platform=Dev
make clean-objects
make -j3 Bootloader=1 SimpleCircuit=1 Platform=blueDev
make clean-objects
make -j3 Bootloader=1 SimpleCircuit=1 Platform=blackDev
make clean-objects
make -j3 Bootloader=1 SimpleCircuit=1 Platform=Red
make clean-objects
make -j3 Bootloader=1 SimpleCircuit=1 Platform=Blue
make clean-objects
make -j3 Bootloader=1 SimpleCircuit=1 Platform=Stick
make clean-objects
make -j3 Bootloader=1 SimpleCircuit=1 Platform=MapleMini
make clean-objects
make -j3 Bootloader=1 Platform=Dev
make clean-objects
make -j3 Bootloader=1 Platform=blueDev
make clean-objects
make -j3 Bootloader=1 Platform=blackDev
make clean-objects
make -j3 Bootloader=1 Platform=Red
make clean-objects
make -j3 Bootloader=1 Platform=Blue
make clean-objects
make -j3 Bootloader=1 Platform=Stick
make clean-objects
make -j3 Bootloader=1 Platform=MapleMini
make clean-objects
make SimpleCircuit=1 Platform=Dev
make clean-objects
make -j3 SimpleCircuit=1 Platform=blueDev
make clean-objects
make -j3 SimpleCircuit=1 Platform=blackDev
make clean-objects
make -j3 SimpleCircuit=1 Platform=Red
make clean-objects
make -j3 SimpleCircuit=1 Platform=Blue
make clean-objects
make -j3 SimpleCircuit=1 Platform=Stick
make clean-objects
make -j3 Platform=Dev
make clean-objects
make -j3 Platform=blueDev
make clean-objects
make -j3 Platform=blackDev
make clean-objects
make -j3 Platform=Red
make clean-objects
make -j3 Platform=Blue
make clean-objects
make -j3 Platform=Stick
rm *.elf
rm *.map
