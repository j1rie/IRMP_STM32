# IRMP on STM32 - a USB IR receiver/sender/powerswitch with wakeup-timer

A remote control receiver with many functions based on cheap hardware.

![Stick](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/Stick.jpg)
![Stick am Board](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/Stick_am_Board.jpg)
 
## Introduction
For 2 € you can buy on ebay a ST-Link emulator or a STM32F103 development board, manufactured in China usually. Onto the STM32F103 mikrocontroller on it an open source firmware with many functions will be flashed.

## Functions
* connection via USB 
* registers as HID device, appears as /dev/hidraw or "Custom HID device", no drivers needed
* transfers the data via USB2 in hardware with fullspeed 
* IR receiver (ca. 40 protocols decoded in hardware) 
* switch PC on via remote control from S3 (STR) and S4 (STD) via USB or from S3, S4 and S5 (SoftOff) via motherboard-switch (+5V needed, on USB or from PSU or motherboard) 
* switch PC on via build in timer from S3 (STR) and S4 (STD) via USB or from S3, S4 and S5 (SoftOff) via motherboard-switch (+5V needed, on USB or from PSU or motherboard) 
* IR sender (ca. 40 protocols) 
* send IR Codes via configurable remote control button 
* the configuration is stored in the emulated eeprom
* bootloader for comfortable firmware update 

## Software Linux
* GUI config-tool stm32IRconfig_gui for configuration: set, get and reset wakeup codes, macros, alarm time, send IR and show the received IR code. It is possible to program wakeups and macros via remote control. Create the translation table with remote control and mouse, as well test and edit. Comprehensive debug messages.
* interactive command line program stm32IRconfig for configuration: set, get and reset wakeups, macros, alarm time, send IR and show the received IR code. It is possible to program wakeups and macros via remote control.
* stm32IRalarm for setting and reading alarm time via script 
* irmplircd is a daemon, running in background as an independent lirc server and passing the IR codes/events on to the application https://github.com/realglotzi/irmplircd
* Software ready for LibreELEC, Ubuntu, yaVDR, easyVDR, MLD, OpenELEC. 

## Software Windows
* GUI config-tool stm32IRconfig_gui (same as linux)
* interactive command line programm stm32IRconfig (same as linux)
* stm32IRalarm for setting and reading alarm time via script
* MediaPortal plugin: https://github.com/pikim/HIDIRT-host/tree/master/hidirt.MePo
* EventGhost with Generic HID 

## Creating a keymap
The stm32IRconfig_gui can be used for building a keymap with the remote control.
There are two methods.

![write_IR](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/write_IR.jpg)  
First method: Press "receive mode" and open the template keymap.
Click the line with the button's name, press the button on the remote, press "write IR" - repeat this for all buttons. 

![append](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/append.jpg)   
Second method: Press "receive mode".
Press the button on the remote, enter the button's name into the textfield Key and press "append" - repeat this for all buttons.

You can test the keymap immediately: After pressing the button on the remote, the red cursor should be in the coresponding line.
At the end press "save" and save the keymap.

## Learning wakeup
If the first wakeup is empty, the first received IR data will be stored into the wakeup.  
Change wakeup with stm32kbdIRconfig_gui: press set by remote - wakeup, press button on remote control.  
Important for testing: wakeup happens only, if the PC is powered off.  
If the PC is powered on, only the key is send to the PC (so you can use the same button for power on and power off).  

## EventGhost
![EG1](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/EG1.jpg)   
![EG2](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/EG2.jpg)   
![EG7](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/EG7.jpg)   
A keycode in the left event-window is pulled via drag&drop into the right configuration-window and assigned to the wanted action of a plugin. 

## Building from source
The sources are complete only after running the prepare script, the script downloads the IRMP sources, extracts and patches the sources. You have to download the ST sources manually, because you have to register unfortunately.  
This makes it possible to put the patches under GPL, without interfering with the original licences.  
It is easier to see, what has been changed, too.  
Compile with arm-none-eabi-gcc.  

## Pin-Out
See /STM32F10x/src/config.h.

## irmplircd, eventlircd, udev and systemd
See /irmplircd/README.

## Thanks to
Frank Meyer for IRMP. [1]  
Uwe Becker for adapting IRMP to the STM32F4xx and his USB-HID for the STM32F4xx. [2]  
His work helped me in getting started with the STM32F105.  
Andrew Kambaroff for his USB-HID for the STM32F103. [3]  
His work helped me in getting started with the STM32F103.  
Ole Ernst for code review, linux Makefile and linux download-extract-patch-script and new protocol. [4]  
Manuel Reimer for gcc-4.9 fix, SimpleCircuit and check if host running. [5]  

[1] https://www.mikrocontroller.net/articles/IRMP  
[2] http://mikrocontroller.bplaced.net/wordpress/?page_id=744  
[3] https://habr.com/post/208026/  
[4] https://github.com/olebowle  
[5] https://github.com/M-Reimer  

## Discussion and more information
For comments, questions, suggestions go to https://www.vdr-portal.de/forum/index.php?thread/123572-irmp-auf-stm32-ein-usb-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-timer/

Much fun with IRMP on STM32!

Copyright (C) 2014-2019 Jörg Riechardt
