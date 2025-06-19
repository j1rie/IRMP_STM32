## IRMP on RP2xxx

<img src="https://www.vdr-portal.de/index.php?attachment/48154-20230825-130009-jpg" width="33%"> [1]  
<img src="https://www.vdr-portal.de/index.php?attachment/49235-ir-sensor-1-jpg" width="100%">
<img src="https://www.vdr-portal.de/index.php?attachment/49236-ir-sensor-2-jpg" width="60%"> [2]  

For boards with the RP2xxx, e.g. the Raspberry Pi Pico, Pico2 and many others.  
This is additional information, basic information in https://github.com/j1rie/IRMP_STM32#readme.

## Flashing the firmware
Disconnect from USB.
Attach to USB while holding down the BOOTSEL button, then release. The device will appear as a mass storage (USB ID 2e8a:0003).  
Drag and drop the firmware.uf2 file onto it. The device will reboot and start as an IRMP HID Device.

Sending the "reboot" command will also put the device into mass storage mode.

If there is already an older firmware on the device, stm32IRconfig - b will put the device into mass storage mode, and
picotool load -v -x firmware.uf2 will flash the firmware, verify and start it.

## Motherboard test
If one of the powerswitch pins is on ground and the other on ca. +3,3V or +5V, then the motherboard is suitable for the following simple wiring.  
If not (very rare), you need an optocoupler.

## Solder and connect cables
Split a 10 cm dupont cable, connect both ends and the 220 ohm resistor, connect the other end of the resistor with the 20 cm dupont cable, from which one socket is cut off. Shrink the solder connections and attach the other 10 cm cable with shrink tube.  
The TSOP is connected directly to the 3,3V, GND and IR_IN pins, the pair of cables is inserted between the power-on pins of the mainboard and the connector from the power button.  

![cables](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/cables.jpg)
![connected](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/connected.jpg)

## First test
Press the BOOTSEL button in suspend mode, and the PC should wake up.  

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
The pico(2) has a regular led, the one and the zero have an RGB led, and the XIAO-RP2350 has a dual and an RGBW led.  
An external led or external RGB led (WS2812 or APA106) can be connected.  
They show what is happening inside the firmware.  
Then there is the Status LED (controlled over hidraw), which shows status messages from the vdr-plugin-statusleds (and blinks on power-on, storage of first wakeup and reboot).

| Receiver              | Board/External RGB-LED                           | Board/External LED | External Status-LED                              |
|-----------------------|--------------------------------------------------|--------------------|--------------------------------------------------|
| disconnected          | off                                              |                    |                                                  |
| USB resumed           | white (or custom)                                |                    |                                                  |
| USB suspended         | orange                                           |                    |                                                  |
| IR reception          | flickers blue                                    | flickers           |                                                  |
| save wakeup           | flashes red quickly                              | flashes quickly    | flashes quickly                                  |
| Wakeup                | flashes red quickly                              | flashes quickly    | flashes quickly                                  |
| Reboot                | flashes red quickly                              | flashes quickly    | flashes quickly                                  |
| Send IR               | short yellow                                     | short on           |                                                  |
| VDR running           | red(*)                                           |                    | on(*)                                            |
| VDR recording         | flashes red according to number of recordings(*) |                    | flashes according to number of recordings(*)     |
| configuration command | short green                                      | short blink        |                                                  |

(*) needs vdr-plugin-statusled [3]

## External RGB-LEDs
It is recommended to use two resistors in the data cable, see http://stefanfrings.de/ws2812/.

##
  \
[1] Waveshare RP2040-One with TSOP by clausmuus, see https://www.vdr-portal.de/forum/index.php?thread/123572-irmp-auf-stm32-ein-usb-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-timer/&postID=1361220#post1361220  
[2] Waveshare RP2040-One with TSOP by FireFly, see https://www.vdr-portal.de/forum/index.php?thread/132289-irmp-auf-stm32-ein-usb-hid-keyboard-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-t/&postID=1371419#post1371419  
[3] https://github.com/j1rie/vdr-plugin-statusleds  
