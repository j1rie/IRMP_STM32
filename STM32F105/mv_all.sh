#! /bin/bash
mv *_SC_BL_*.bin ../binaries/firmware_for_bootloader/SimpleCircuit/
mv *_BL_*.bin ../binaries/firmware_for_bootloader/
mv *_SC_*.bin ../binaries/firmware_without_bootloader/SimpleCircuit/
mv *_*.bin ../binaries/firmware_without_bootloader/
make clean-objects
