#! /bin/bash
mv *_SC_BL-jrie.bin ../binaries/firmware_for_bootloader/SimpleCircuit/
mv *_BL-jrie.bin ../binaries/firmware_for_bootloader/ 
mv *_SC-jrie.bin ../binaries/firmware_without_bootloader/SimpleCircuit/
mv *-jrie.bin ../binaries/firmware_without_bootloader/
make clean-objects