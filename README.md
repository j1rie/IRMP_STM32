# IRMP on STM32 - a USB IR receiver/sender/power switch with wake-up timer

A remote control receiver with many functions based on cheap hardware.

![Stick](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/Stick.jpg)
  
![Stick on Board](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/Stick_am_Board.jpg)

## Introduction
For 2 € you can buy an ST-Link emulator or a STM32F103 development board, usually made in China, on ebay. The STM32F103 microcontroller on it is flashed with an open source firmware with many functions.

## Features
* connection over USB 
* registers as HID device, appears as /dev/hidraw or "Custom HID device", no drivers needed
* transfers the data over USB2 in hardware at fullspeed 
* IR receiver (about 40 protocols decoded in hardware) 
* power on PC via remote control from S3 (STR) and S4 (STD) via USB or from S3, S4 and S5 (SoftOff) via motherboard switch (+5V required, on USB or from PSU or motherboard) 
* power on PC via built-in timer from S3 (STR) and S4 (STD) via USB or from S3, S4 and S5 (SoftOff) via motherboard switch (+5V required, on USB or from PSU or motherboard) 
* IR transmitter (about 40 protocols) 
* the configuration is stored in the emulated eeprom
* bootloader for easy firmware update
* macro support

A macro allows a sequence of IR commands to be sent via the transmitter diode when a trigger is received.
 macroslot0 is the trigger, macroslot1 … macroslot7 are the commands to be sent.

## Software Linux
* GUI configuration tool stm32IRconfig_gui for configuration: set, get and reset wakeup codes, macros, alarm time, send IR and show the received IR code. It is possible to program wakeups and macros by the remote control. Create, test and edit the translation table using remote control and mouse. Comprehensive debug messages. Firmware update.
* interactive command line program stm32IRconfig for configuration: set, get and reset wakeups, macros, alarm time, send IR and display the received IR code. It is possible to program wakeups and macros by the remote control.
* stm32IRalarm for setting and reading alarm time via script 
* stm32FWupgrade for command line firmware upgrade
* irmplircd is a daemon that runs in the background as an independent lirc server and passes the IR codes/events on to the application https://github.com/realglotzi/irmplircd
* irctl for configuration: https://github.com/olebowle/irctl
* Software available for Ubuntu, yaVDR, easyVDR, MLD.

## Software Windows
* GUI configuration tool stm32IRconfig_gui (same as linux)
* interactive command line programm stm32IRconfig (same as linux)
* stm32IRalarm to set and read alarm time via script
* stm32FWupgrade for command line firmware upgrade
* MediaPortal plugin: https://github.com/pikim/HIDIRT-host/tree/master/hidirt.MePo
* EventGhost with generic HID

## Creating a keymap
The stm32IRconfig_gui can be used to create a keymap using the remote control.  
There are two methods.  
  
(1)  
![write_IR](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/write_IR.jpg)   
First method: Press "receive mode" and open the template keymap.
Click on the line with the name of the button, press the button on the remote, press "write IR" - repeat for all the buttons.  
  
(2)  
![append](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/append.jpg)   
Second method: Press "receive mode".
Press the button on the remote, type the name of the button into the Key text box and press "append" - repeat for all the buttons.

You can now test the keymap: After pressing the button on the remote, the red cursor should be in the corresponding line.
When you are done, press "save" to save the keymap.

## Learning wakeup
If the first wakeup is empty, the first IR data received will be stored in the wakeup.  
Change wakeup with stm32kbdIRconfig_gui: press set by remote - wakeup, press button on remote control.  
Important for testing: wakeup only happens, if the PC is switched off.  
If the PC is powered on, only the key is sent to the PC (so you can use the same button to switch on and off).  

## EventGhost
![EG1](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/EG1.jpg)   

![EG2](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/EG2.jpg)   

![EG7](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/EG7.jpg)   

A keycode in the left event window is dragged into the right configuration window and assigned to the desired action of a plugin. 

## Building from source
The sources are not complete until you run the prepare script, the script downloads the IRMP sources, extracts and patches the sources. You have to download the ST sources manually, because unfortunately you have to register.  
This makes it possible to put the patches under the GPL, without interfering with the original licences.  
It is also easier to see, what has been changed, too.  
Compile with arm-none-eabi-gcc.  

## Pin-Out
See /STM32Fxxxxx/src/config.h.

## Flashing the bootloader with a ST-Link
Connect  
Programmer SWDIO -> IO  
Programmer SWCLK -> CLK  
Programmer GND -> G  
Programmer 3V3 -> V3  
Windows:  
Get the STM32 ST-Link Utility.  
File - Open File - boot.blackDev.bin (https://github.com/j1rie/STM32F103-bootloader/tree/master/binaries)  
Target - Program & Verify (confirm a message about read out protection, if it appears)  
Linux:  
openocd -f ocd-program.cfg  
If the flash is locked, first run openocd -f ocd-unlock.cfg first, disconnect and reconnect the device, and then 
run openocd -f ocd-program.cfg again.  
The files are in the STM32Fxxx(yy)-bootloader repos.  
https://github.com/j1rie/IRMP_STM32_KBD/wiki/Herstellungsprozess-Schwarze#bootloader-flashen  

## Flashing the firmware
Disconnect from USB.  
Linux: Start ./stm32FWupgrade ./2019-07-06_16-32_blackDev_BL_SC_jrie.bin  
Windows: Start stm32FWupgrade.exe 2020-02-15_00-29_blueDev_BL_SC_jrie.bin  
Attach to USB.  
Instead of stm32FWupgrade you can use stm32IRconfig_gui.  
https://github.com/j1rie/IRMP_STM32_KBD/wiki/Herstellungsprozess-Schwarze#firmware-flashen  

## Other ways how to flash
Flashing with an USB-serial-TTL is possible on the developer boards.  
Flashing over USB is possible on the boards, that support it.  
See https://www.mikrocontroller.net/articles/IRMP_auf_STM32_-_ein_USB_IR_Empf%C3%A4nger/Sender/Einschalter_mit_Wakeup-Timer#Firmware_Flashing  

## irmplircd, eventlircd, udev and systemd
See /irmplircd/README.

## Thanks to
Frank Meyer for IRMP. [1]  
Uwe Becker for adapting IRMP to the STM32F4xx and his USB-HID for the STM32F4xx. [2]  
His work helped me to get started with the STM32F105.  
Andrew Kambaroff for his USB-HID for the STM32F103. [3]  
His work helped me to get started with the STM32F103.  
Ole Ernst for code review, linux Makefile and linux download-extract-patch-script and new protocol. [4]  
Manuel Reimer for gcc-4.9 fix, SimpleCircuit and check if host is running. [5]  
Martin Neubauer for integration into EasyVDR, addon board development and selling ready-to-use receiver sets. [6]  
Alexander Grothe for integration into yaVDR and much help with difficult questions. [7]  
Helmut Emmerich for selling ready-to-use receiver sets and user support. [8]  
Claus Muus for integration into MLD. [9]  
All Users for questions, feature requests, feedback and ideas  

[1] https://www.mikrocontroller.net/articles/IRMP  
[2] http://mikrocontroller.bplaced.net/wordpress/?page_id=744  
[3] https://habr.com/post/208026/  
[4] https://github.com/olebowle  
[5] https://github.com/M-Reimer  
[6] https://www.vdr-portal.de/user/4786-ranseyer/  
    https://github.com/ranseyer/STM32-IRMP-Hardware  
[7] https://www.vdr-portal.de/user/24681-seahawk1986/  
[8] https://www.vdr-portal.de/user/13499-emma53/  
[9] https://www.vdr-portal.de/user/942-clausmuus/  

## Discussion and further information
Comments, questions and suggestions can be sent to https://www.vdr-portal.de/forum/index.php?thread/123572-irmp-auf-stm32-ein-usb-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-timer/

## Construction manuals
https://github.com/j1rie/IRMP_STM32_KBD/wiki/Herstellungsprozess-Schwarze  
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_%E2%80%93_stark_vereinfachte_Bauanleitung  
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_-_Bauanleitung  
The latter two are also in /doc/old.  

Have fun with IRMP on STM32!

Copyright (C) 2014-2025 Jörg Riechardt
