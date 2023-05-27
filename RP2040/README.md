## IRMP on RP2040

For boards with the RP2040, e.g. the Raspberry Pi Pico and many others.  
This is additional information, basic information in https://github.com/j1rie/IRMP_STM32#readme.

## Flashing the firmware
Disconnect from USB.
Attach to USB while holding BOOTSEL button down, than release. The device will show up as a mass storage.
Drag and drop the firmware.uf2 file onto it. The device will reboot and start as an IRMP HID-Device.

Sending the "reboot" command brings device into mass storage state as well.

## First test
When you press the BOOTSEL button in suspend mode, the PC should wake up.  
For wiring see https://www.mikrocontroller.net/articles/IRMP_auf_STM32_-_Bauanleitung#Minimalistic_assembly_for_experienced_users  
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_%E2%80%93_stark_vereinfachte_Bauanleitung#Solder_Cables  
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_%E2%80%93_stark_vereinfachte_Bauanleitung#Connect_Cables

## Emulated Eeprom
Any configuration done by one of the configuration programs goes first into cache only. In order to save
those changes into flash permanently, you have to do an eeprom commit.
Exception: first wakeup is commited by firmware for bnackwards compability.

## Building from source
See [Getting Started with the Raspberry Pi Pico](https://rptl.io/pico-get-started)  
-> Get the SDK and examples  
-> Install the Toolchain  
->  Building "Blink"  
->  Load and run "Blink"  
