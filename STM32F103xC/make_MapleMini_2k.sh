make clean-objects
make -j3 Bootloader=1 SimpleCircuit=1 Platform=MapleMini_2k
#make clean-objects
rm *.elf
rm *.map
