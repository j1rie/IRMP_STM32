## IRMP on RP2040 - a USB IR receiver/sender/powerswitch with wakeup-timer

For boards with the RP2040, e.g. the Raspberry Pi Pico and many others.

## Flashing the firmware
Disconnect from USB.
Attach to USB while holding BOOTSEL button down, than release. The device will show up as a mass storage.
Drag and drop the firmware.uf2 file onto it. The device will reboot and start as an IRMP HID-Device.

Sending the "reboot" command brings device into mass storage state as well.

## First test
When you press the BOOTSEL button in suspend mode, the PC should wake up.
