#! /bin/bash
mv *_SC_BL-*.bin ../binaries/firmware_for_bootloader/SimpleCircuit/
mv *_BL-*.bin ../binaries/firmware_for_bootloader/
mv *_SC-*.bin ../binaries/firmware_without_bootloader/SimpleCircuit/
mv *-*.bin ../binaries/firmware_without_bootloader/
make clean-objects
