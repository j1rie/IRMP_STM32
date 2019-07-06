#! /bin/bash
mv *_BL5k_SC_*.bin ../binaries/firmware_for_bootloader/SimpleCircuit/
mv *_BL5k_*.bin ../binaries/firmware_for_bootloader/
mv *_SC_*.bin ../binaries/firmware_without_bootloader/SimpleCircuit/
mv *_*.bin ../binaries/firmware_without_bootloader/
make clean-objects
