## IRMP on RP2xxx

<img src="https://www.vdr-portal.de/index.php?attachment/48154-20230825-130009-jpg" width="33%"> [1]  
[Receiver inside thin client](https://www.vdr-portal.de/index.php?attachment/49235-ir-sensor-1-jpg) [2]  
[Receiver inside thin client](https://www.vdr-portal.de/index.php?attachment/49236-ir-sensor-2-jpg) [2]

For boards with the RP2xxx, e.g. the Raspberry Pi Pico, Pico2 and many others.  
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
Exception: first wakeup is commited by firmware for backwards compability.

## Building from source
See [Getting Started with the Raspberry Pi Pico](https://rptl.io/pico-get-started)  
-> Get the SDK and examples  
-> Install the Toolchain  
->  Building "Blink"  
->  Load and run "Blink"  

## Pin-Out
See ./src/config.h.

##
  \
[1] Waveshare RP2040-One with TSOP by clausmuus, see https://www.vdr-portal.de/forum/index.php?thread/123572-irmp-auf-stm32-ein-usb-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-timer/&postID=1361220#post1361220  
[2] Waveshare RP2040-One with TSOP by FireFly, see https://www.vdr-portal.de/forum/index.php?thread/132289-irmp-auf-stm32-ein-usb-hid-keyboard-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-t/&postID=1371419#post1371419
