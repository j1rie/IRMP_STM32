REM  *
REM  * Copyright (C) 2014 Joerg Riechardt
REM  *
REM  * This program is free software; you can redistribute it and/or modify
REM  * it under the terms of the GNU General Public License as published by
REM  * the Free Software Foundation; either version 2 of the License, or
REM  * (at your option) any later version.
REM  *

REM go into the parent directory of the directory, in which the script is located
cd /D %~dp0..\

REM save directory 
set PWD="%CD%"

REM download
mkdir ext_src
if exist %PWD%\ext_src\prepared goto EXIT
if not exist %PWD%\ext_src\stsw-stm32010.zip powershell.exe -Command (new-object System.Net.WebClient).DownloadFile('http://www.st.com/st-web-ui/static/active/en/st_prod_software_internet/resource/technical/software/firmware/stsw-stm32010.zip','%PWD%\ext_src\stsw-stm32010.zip')
if not exist %PWD%\ext_src\stsw-stm32121.zip powershell.exe -Command (new-object System.Net.WebClient).DownloadFile('http://www.st.com/st-web-ui/static/active/en/st_prod_software_internet/resource/technical/software/firmware/stsw-stm32121.zip', '%PWD%\ext_src\stsw-stm32121.zip')
if not exist %PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip powershell.exe -Command (new-object System.Net.WebClient).DownloadFile('http://www.st.com/st-web-ui/static/active/en/st_prod_software_internet/resource/technical/software/firmware/stm32_f105-07_f2_f4_usb-host-device_lib.zip', '%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip')
REM if not exist %PWD%\ext_src\irmp.tar.gz powershell.exe -Command (new-object System.Net.WebClient).DownloadFile('http://www.mikrocontroller.net/svnbrowser/irmp/?view=tar', '%PWD%\ext_src\irmp.tar.gz')
if not exist %PWD%\ext_src\patch.exe powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\ext_src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\patch-2.5.9-7-bin.zip\bin\patch.exe'))
if not exist %PWD%\ext_src\Irmp.zip powershell.exe -Command (new-object System.Net.WebClient).DownloadFile('http://www.mikrocontroller.net/wikifiles/7/79/Irmp.zip','%PWD%\ext_src\Irmp.zip')
if not exist %PWD%\ext_src\Irsnd.zip powershell.exe -Command (new-object System.Net.WebClient).DownloadFile('http://www.mikrocontroller.net/wikifiles/c/c7/Irsnd.zip','%PWD%\ext_src\Irsnd.zip')

REM extract
mkdir cmsis_boot\startup
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\cmsis_boot').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\CMSIS\Device\ST\STM32F10x\Include\stm32f10x.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\cmsis_boot').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\CMSIS\Device\ST\STM32F10x\Include\system_stm32f10x.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\cmsis_boot').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Projects\Custom_HID\inc\stm32f10x_conf.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\cmsis_boot\startup').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\CMSIS\Device\ST\STM32F10x\Source\Templates\gcc_ride7\startup_stm32f10x_cl.s'))
mkdir cmsis
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\cmsis').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\CMSIS\Include\core_cm3.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\cmsis').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\CMSIS\Include\core_cmFunc.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\cmsis').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\CMSIS\Include\core_cmInstr.h'))
mkdir stm_lib\inc
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\STM32F10x_StdPeriph_Driver\inc\misc.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\STM32F10x_StdPeriph_Driver\inc\stm32f10x_flash.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\STM32F10x_StdPeriph_Driver\inc\stm32f10x_gpio.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\STM32F10x_StdPeriph_Driver\inc\stm32f10x_rcc.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\STM32F10x_StdPeriph_Driver\inc\stm32f10x_tim.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\STM32F10x_StdPeriph_Driver\inc\stm32f10x_usart.h'))
mkdir stm_lib\src
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\STM32F10x_StdPeriph_Driver\src\misc.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\STM32F10x_StdPeriph_Driver\src\stm32f10x_flash.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\STM32F10x_StdPeriph_Driver\src\stm32f10x_gpio.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\STM32F10x_StdPeriph_Driver\src\stm32f10x_rcc.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\STM32F10x_StdPeriph_Driver\src\stm32f10x_tim.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32121.zip\STM32_USB-FS-Device_Lib_V4.0.0\Libraries\STM32F10x_StdPeriph_Driver\src\stm32f10x_usart.c'))

powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\cmsis_boot').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\CMSIS\Device\ST\STM32F10x\Source\Templates\system_stm32f10x.c'))

mkdir usb_hid\inc
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_Device_Library\Class\hid\inc\usbd_hid_core.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_Device_Library\Core\inc\usbd_core.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_Device_Library\Core\inc\usbd_def.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_Device_Library\Core\inc\usbd_ioreq.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_Device_Library\Core\inc\usbd_req.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_Device_Library\Core\inc\usbd_usr.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_OTG_Driver\inc\usb_bsp.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_OTG_Driver\inc\usb_core.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_OTG_Driver\inc\usb_dcd.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_OTG_Driver\inc\usb_dcd_int.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_OTG_Driver\inc\usb_defines.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_OTG_Driver\inc\usb_regs.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Project\USB_Device_Examples\HID\inc\usb_conf.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Project\USB_Device_Examples\HID\inc\usbd_conf.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Project\USB_Device_Examples\HID\inc\usbd_desc.h'))
mkdir usb_hid\src
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_Device_Library\Class\hid\src\usbd_hid_core.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_Device_Library\Core\src\usbd_core.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_Device_Library\Core\src\usbd_ioreq.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_Device_Library\Core\src\usbd_req.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_OTG_Driver\src\usb_core.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_OTG_Driver\src\usb_dcd.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Libraries\STM32_USB_OTG_Driver\src\usb_dcd_int.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Project\USB_Device_Examples\HID\src\usb_bsp.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Project\USB_Device_Examples\HID\src\usbd_desc.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\usb_hid\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stm32_f105-07_f2_f4_usb-host-device_lib.zip\STM32_USB-Host-Device_Lib_V2.1.0\Project\USB_Device_Examples\HID\src\usbd_usr.c'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\inc').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32010.zip\STM32F10x_AN2594_FW_V3.1.0\Project\EEPROM_Emulation\inc\eeprom.h'))
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\stm_lib\src').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\stsw-stm32010.zip\STM32F10x_AN2594_FW_V3.1.0\Project\EEPROM_Emulation\src\eeprom.c'))
powershell.exe -Command (new-object System.Net.WebClient).DownloadFile('http://sourceforge.net/projects/gnuwin32/files/patch/2.5.9-7/patch-2.5.9-7-bin.zip/download', '%PWD%\ext_src\patch-2.5.9-7-bin.zip')
mkdir ext_src\irmp
mkdir irmp
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\ext_src\irmp').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\Irmp.zip').Items())
REM powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\irmp').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\Irmp.zip\irmp.c'))
copy %PWD%\ext_src\irmp\irmp.c %PWD%\irmp\irmp.c
REM powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\irmp').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\Irmp.zip\irmp.h'))
copy %PWD%\ext_src\irmp\irmp.h %PWD%\irmp\irmp.h
REM powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\irmp').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\Irmp.zip\irmpconfig.h'))
copy %PWD%\ext_src\irmp\irmpconfig.h %PWD%\irmp\irmpconfig.h
REM powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\irmp').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\Irmp.zip\irmpprotocols.h'))
copy %PWD%\ext_src\irmp\irmpprotocols.h %PWD%\irmp\irmpprotocols.h
REM powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\irmp').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\Irmp.zip\irmpsystem.h'))
copy %PWD%\ext_src\irmp\irmpsystem.h %PWD%\irmp\irmpsystem.h
powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\ext_src\irmp').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\Irsnd.zip').Items(), 20)
REM powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\irmp').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\Irsnd.zip\irsnd.c'))
copy %PWD%\ext_src\irmp\irsnd.c %PWD%\irmp\irsnd.c
REM powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\irmp').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\Irsnd.zip\irsnd.h'))
copy %PWD%\ext_src\irmp\irsnd.h %PWD%\irmp\irsnd.h
REM powershell.exe -Command (new-object -ComObject shell.application).Namespace('%PWD%\irmp').copyhere((new-object -ComObject shell.application).Namespace('%PWD%\ext_src\Irsnd.zip\irsndconfig.h'))
copy %PWD%\ext_src\irmp\irsndconfig.h %PWD%\irmp\irsndconfig.h
del /F /S /Q %PWD%\ext_src\irmp\
rd /s /q ext_src\irmp

REM patch
%PWD%\ext_src\patch.exe -d usb_hid -p1 -i %PWD%\patches\usb_hid.patch --binary
%PWD%\ext_src\patch.exe -d stm_lib -p1 -i %PWD%\patches\eeprom.patch --binary
%PWD%\ext_src\patch.exe -d cmsis_boot -p1 -i %PWD%\patches\stm32f10x_conf.patch --binary
%PWD%\ext_src\patch.exe -d cmsis_boot -p1 -i %PWD%\patches\startup.patch --binary
%PWD%\ext_src\patch.exe -d irmp -p1 -i %PWD%\patches\irmp.patch --binary

REM mark as prepared
echo. > %PWD%\ext_src\prepared
:EXIT
