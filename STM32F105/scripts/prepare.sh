#!/bin/bash
# go into the parent directory of the directory, in which the script is located
cd "$(dirname "$(readlink -e "$0")")"
cd ..

# download
mkdir -p ext_src
[[ -e ./ext_src/prepared ]] && exit 0
cd ./ext_src
for i in en.stm32_f105-07_f2_f4_usb-host-device_lib stsw-stm32010; do
	if [[ ! -s $i.zip ]]; then
		wget "http://www.st.com/resource/en/firmware/$i.zip"
	fi
done
if [[ ! -s irmp.tar.gz ]]; then
	wget "http://www.mikrocontroller.net/svnbrowser/irmp/?view=tar" -O irmp.tar.gz
fi

# extract
cd ..
ar2='../../ext_src/en.stm32_f105-07_f2_f4_usb-host-device_lib.zip'
ar1='../ext_src/en.stm32_f105-07_f2_f4_usb-host-device_lib.zip'
ver='2.2.0'
path="STM32_USB-Host-Device_Lib_V$ver"
mkdir -p cmsis_boot
cd cmsis_boot
unzip -j $ar1 \
      $path/Libraries/CMSIS/Device/ST/STM32F10x/Include/stm32f10x.h \
      $path/Libraries/CMSIS/Device/ST/STM32F10x/Include/system_stm32f10x.h \
      $path/Project/USB_Device_Examples/CustomHID/inc/stm32f10x_conf.h
mkdir -p startup
cd startup
unzip -j $ar2 $path/Libraries/CMSIS/Device/ST/STM32F10x/Source/Templates/gcc_ride7/startup_stm32f10x_cl.s
cd ../..
mkdir -p cmsis
cd cmsis
unzip -j $ar1 \
       $path/Libraries/CMSIS/Include/core_cm3.h \
       $path/Libraries/CMSIS/Include/core_cmFunc.h \
       $path/Libraries/CMSIS/Include/core_cmInstr.h \
       $path/Libraries/CMSIS/Include/cmsis_gcc.h \
       $path/Libraries/CMSIS/Include/cmsis_armcc.h \
       $path/Libraries/CMSIS/Include/cmsis_armcc_V6.h
cd ..
mkdir -p stm_lib
cd  stm_lib
mkdir -p inc
cd inc
unzip -j $ar2 \
      $path/Libraries/STM32F10x_StdPeriph_Driver/inc/misc.h \
      $path/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_flash.h \
      $path/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_gpio.h \
      $path/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_rcc.h \
      $path/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_tim.h \
      $path/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_usart.h
cd ..
mkdir -p src
cd src
unzip -j $ar2 \
      $path/Libraries/STM32F10x_StdPeriph_Driver/src/misc.c \
      $path/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c \
      $path/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
      $path/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
      $path/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c \
      $path/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c
cd ../..

cd cmsis_boot
unzip -j $ar1 $path/Libraries/CMSIS/Device/ST/STM32F10x/Source/Templates/system_stm32f10x.c
cd ..
mkdir -p usb_hid
cd usb_hid
mkdir -p inc
cd inc
unzip -j $ar2 \
      $path/Libraries/STM32_USB_Device_Library/Class/hid/inc/usbd_hid_core.h \
      $path/Libraries/STM32_USB_Device_Library/Core/inc/usbd_core.h \
      $path/Libraries/STM32_USB_Device_Library/Core/inc/usbd_def.h \
      $path/Libraries/STM32_USB_Device_Library/Core/inc/usbd_ioreq.h \
      $path/Libraries/STM32_USB_Device_Library/Core/inc/usbd_req.h \
      $path/Libraries/STM32_USB_Device_Library/Core/inc/usbd_usr.h \
      $path/Libraries/STM32_USB_OTG_Driver/inc/usb_bsp.h \
      $path/Libraries/STM32_USB_OTG_Driver/inc/usb_core.h \
      $path/Libraries/STM32_USB_OTG_Driver/inc/usb_dcd.h \
      $path/Libraries/STM32_USB_OTG_Driver/inc/usb_dcd_int.h \
      $path/Libraries/STM32_USB_OTG_Driver/inc/usb_defines.h \
      $path/Libraries/STM32_USB_OTG_Driver/inc/usb_regs.h \
      $path/Project/USB_Device_Examples/HID/inc/usb_conf.h \
      $path/Project/USB_Device_Examples/HID/inc/usbd_conf.h \
      $path/Project/USB_Device_Examples/HID/inc/usbd_desc.h
cd ..
mkdir -p src 
cd src
unzip -j $ar2 \
      $path/Libraries/STM32_USB_Device_Library/Class/hid/src/usbd_hid_core.c \
      $path/Libraries/STM32_USB_Device_Library/Core/src/usbd_core.c \
      $path/Libraries/STM32_USB_Device_Library/Core/src/usbd_ioreq.c \
      $path/Libraries/STM32_USB_Device_Library/Core/src/usbd_req.c \
      $path/Libraries/STM32_USB_OTG_Driver/src/usb_core.c \
      $path/Libraries/STM32_USB_OTG_Driver/src/usb_dcd.c \
      $path/Libraries/STM32_USB_OTG_Driver/src/usb_dcd_int.c \
      $path/Project/USB_Device_Examples/HID/src/usb_bsp.c \
      $path/Project/USB_Device_Examples/HID/src/usbd_desc.c \
      $path/Project/USB_Device_Examples/HID/src/usbd_usr.c

ar='../../ext_src/stsw-stm32010.zip'
ver='3.1.0'
path="STM32F10x_AN2594_FW_V$ver"
cd ../..
cd  stm_lib
cd inc
unzip -j $ar $path/Project/EEPROM_Emulation/inc/eeprom.h
cd ..
cd src
unzip -j $ar $path/Project/EEPROM_Emulation/src/eeprom.c
cd ../..

ar='../ext_src/irmp.tar.gz'
path="irmp"
mkdir -p irmp
cd irmp
tar -xvf $ar --strip-components=1 \
    $path/irmp.c \
    $path/irmp.h \
    $path/irmpconfig.h \
    $path/irmpprotocols.h \
    $path/irmpsystem.h \
    $path/irsnd.c \
    $path/irsnd.h \
    $path/irsndconfig.h \
    $path/README.txt
cd ..

# patch
patch -d usb_hid -p1 -i ../patches/usb_hid.patch
patch -d stm_lib -p1 -i ../patches/eeprom.patch
patch -d cmsis_boot -p1 -i ../patches/stm32f10x_conf.patch
patch -d cmsis_boot -p1 -i ../patches/startup.patch
patch -d cmsis_boot -p1 -i ../patches/system.patch
patch -d irmp -p1 -i ../patches/irmp.patch

# mark as prepared
touch ./ext_src/prepared
