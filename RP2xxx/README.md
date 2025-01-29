## IRMP on RP2xxx

<img src="https://www.vdr-portal.de/index.php?attachment/48154-20230825-130009-jpg" width="33%"> [1]  
<img src="https://www.vdr-portal.de/index.php?attachment/49235-ir-sensor-1-jpg" width="100%">
<img src="https://www.vdr-portal.de/index.php?attachment/49236-ir-sensor-2-jpg" width="60%"> [2]  

For boards with the RP2xxx, e.g. the Raspberry Pi Pico, Pico2 and many others.  
This is additional information, basic information in https://github.com/j1rie/IRMP_STM32#readme.

## Flashing the firmware
Disconnect from USB.
Attach to USB while holding down the BOOTSEL button, then release. The device will appear as a mass storage.
Drag and drop the firmware.uf2 file onto it. The device will reboot and start as an IRMP HID Device.

Sending the "reboot" command will also put the device into mass storage mode.

## First test
Press the BOOTSEL button in suspend mode, and the PC should wake up.  
For wiring see https://www.mikrocontroller.net/articles/IRMP_auf_STM32_-_Bauanleitung#Minimalistic_assembly_for_experienced_users  
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_%E2%80%93_stark_vereinfachte_Bauanleitung#Solder_Cables  
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_%E2%80%93_stark_vereinfachte_Bauanleitung#Connect_Cables

## Emulated Eeprom
Any configuration made by one of the configuration programs goes first into cache only. To permanently save
these changes  to flash, you have to do an eeprom commit. This applies to macros and multiple wakeups.
Exception: the first wakeup is committed by the firmware for backward compatibility.

## Building from source
See [Getting Started with the Raspberry Pi Pico](https://rptl.io/pico-get-started)  
-> Get the SDK and examples  
-> Install the toolchain  
-> Build "Blink"  
-> Load and run "Blink"  

## Pin-Out
See ./src/config.h.

## 5 V from power supply instead via USB
If you need to power the device from the power supply, it must not be powered via USB in order to avoid cross-current.
One way is to cut the copper on the pcb:  

![cut VBUS](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/RP2040-One_VBUS_trennen.jpg)

## Signals from LEDs
The pico(2) has a regular LED, the one and the zero have an RGB LED, and the XIAO-RP2350 has an RGBW LED.
An external LED or RGB LED can be connected.
They show what is happening inside the firmware.

| Receiver              | RGB-LED                                       |
|-----------------------|-----------------------------------------------|
| disconnected          | off                                           |
| USB resumed           | white (or custom)                             |
| USB suspended         | orange                                        |
| IR reception          | flickers blue                                 |
| save wakeup           | flashes red quickly                           |
| Wakeup                | flashes red quickly                           |
| Reboot                | flashes red quickly                           |
| Send IR               | short yellow                                  |
| VDR running           | red                                           |
| VDR recording         | flashes red according to number of recordings |
| configuration command | short green                                   |

Then there is the Status LED (controlled over hidraw), which shows status messages from the vdr-plugin-statusleds and blinks on power-on, storage of first wakeup and reboot.

##
  \
[1] Waveshare RP2040-One with TSOP by clausmuus, see https://www.vdr-portal.de/forum/index.php?thread/123572-irmp-auf-stm32-ein-usb-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-timer/&postID=1361220#post1361220  
[2] Waveshare RP2040-One with TSOP by FireFly, see https://www.vdr-portal.de/forum/index.php?thread/132289-irmp-auf-stm32-ein-usb-hid-keyboard-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-t/&postID=1371419#post1371419
