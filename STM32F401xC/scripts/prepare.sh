#!/bin/bash
# go into the parent directory of the directory, in which the script is located
cd "$(dirname "$(readlink -e "$0")")"
cd ..

# download
mkdir -p ext_src
[[ -e ./ext_src/prepared ]] && exit 0
cd ./ext_src
for i in en.stm32_f105-07_f2_f4_usb-host-device_lib en.stsw-stm32066; do
	if [[ ! -s $i.zip ]]; then
		echo 'unfortunately you have to download these files from ST manually and put them into ext_src'
		echo 'https://www.st.com/en/embedded-software/stsw-stm32046.html'
		echo 'https://www.st.com/en/embedded-software/stsw-stm32066.html'
		exit
		#wget "http://www.st.com/resource/en/firmware/$i.zip"
	fi
done
#if [[ ! -s irmp.tar.gz ]]; then
#	wget "http://www.mikrocontroller.net/svnbrowser/irmp/?view=tar" -O irmp.tar.gz
#fi

if [[ ! -s IRMP-master.zip ]]; then
	wget "https://github.com/j1rie/IRMP/archive/refs/heads/master.zip"  -O IRMP-master.zip
fi

# extract
cd ..
ar2='../../ext_src/en.stm32_f105-07_f2_f4_usb-host-device_lib.zip'
ar1='../ext_src/en.stm32_f105-07_f2_f4_usb-host-device_lib.zip'
ver='2.2.1'
path="STM32_USB-Host-Device_Lib_V$ver"
mkdir -p cmsis_boot
cd cmsis_boot
unzip -j $ar1 \
      Libraries/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h \
      Libraries/CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h \
      Project/USB_Device_Examples/CustomHID/inc/stm32f4xx_conf.h
mkdir -p startup
cd startup
unzip -j $ar2 Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f401xx.s
cd ../..
mkdir -p cmsis
cd cmsis
unzip -j $ar1 \
       Libraries/CMSIS/Include/core_cm4.h \
       Libraries/CMSIS/Include/core_cmFunc.h \
       Libraries/CMSIS/Include/core_cmInstr.h \
       Libraries/CMSIS/Include/core_cmSimd.h \
       Libraries/CMSIS/Include/cmsis_gcc.h
#        \
#       Libraries/CMSIS/Include/cmsis_armcc.h \
#       Libraries/CMSIS/Include/cmsis_armcc_V6.h
cd ..
mkdir -p stm_lib
cd  stm_lib
mkdir -p inc
cd inc
unzip -j $ar2 \
      Libraries/STM32F4xx_StdPeriph_Driver/inc/misc.h \
      Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_flash.h \
      Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_gpio.h \
      Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rcc.h \
      Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_tim.h \
      Libraries/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_usart.h
cd ..
mkdir -p src
cd src
unzip -j $ar2 \
      Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c \
      Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c \
      Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c \
      Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c \
      Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c \
      Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c
cd ../..

cd cmsis_boot
unzip -j $ar1 Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
cd ..
mkdir -p usb_hid
cd usb_hid
mkdir -p inc
cd inc
unzip -j $ar2 \
      Libraries/STM32_USB_Device_Library/Class/hid/inc/usbd_hid_core.h \
      Libraries/STM32_USB_Device_Library/Core/inc/usbd_core.h \
      Libraries/STM32_USB_Device_Library/Core/inc/usbd_def.h \
      Libraries/STM32_USB_Device_Library/Core/inc/usbd_ioreq.h \
      Libraries/STM32_USB_Device_Library/Core/inc/usbd_req.h \
      Libraries/STM32_USB_Device_Library/Core/inc/usbd_usr.h \
      Libraries/STM32_USB_OTG_Driver/inc/usb_bsp.h \
      Libraries/STM32_USB_OTG_Driver/inc/usb_core.h \
      Libraries/STM32_USB_OTG_Driver/inc/usb_dcd.h \
      Libraries/STM32_USB_OTG_Driver/inc/usb_dcd_int.h \
      Libraries/STM32_USB_OTG_Driver/inc/usb_defines.h \
      Libraries/STM32_USB_OTG_Driver/inc/usb_regs.h \
      Project/USB_Device_Examples/HID/inc/usb_conf.h \
      Project/USB_Device_Examples/HID/inc/usbd_conf.h \
      Project/USB_Device_Examples/HID/inc/usbd_desc.h
cd ..
mkdir -p src 
cd src
unzip -j $ar2 \
      Libraries/STM32_USB_Device_Library/Class/hid/src/usbd_hid_core.c \
      Libraries/STM32_USB_Device_Library/Core/src/usbd_core.c \
      Libraries/STM32_USB_Device_Library/Core/src/usbd_ioreq.c \
      Libraries/STM32_USB_Device_Library/Core/src/usbd_req.c \
      Libraries/STM32_USB_OTG_Driver/src/usb_core.c \
      Libraries/STM32_USB_OTG_Driver/src/usb_dcd.c \
      Libraries/STM32_USB_OTG_Driver/src/usb_dcd_int.c \
      Project/USB_Device_Examples/HID/src/usb_bsp.c \
      Project/USB_Device_Examples/HID/src/usbd_desc.c \
      Project/USB_Device_Examples/HID/src/usbd_usr.c

ar='../../ext_src/en.stsw-stm32066.zip'
ver='1.0.0'
path="STM32F4xx_AN3969_V$ver"
cd ../..
cd  stm_lib
cd inc
unzip -j $ar $path/Project/STM32F4xx_EEPROM_Emulation/inc/eeprom.h
cd ..
cd src
unzip -j $ar $path/Project/STM32F4xx_EEPROM_Emulation/src/eeprom.c
cd ../..

#ar='../ext_src/irmp.tar.gz'
ar='../ext_src/IRMP-master.zip'
#path="irmp"
path="IRMP-master"
mkdir -p irmp
cd irmp
#tar -xvf $ar --strip-components=1 \
#    $path/irmp.c \
#    $path/irmp.h \
#    $path/irmpconfig.h \
#    $path/irmpprotocols.h \
#    $path/irmpsystem.h \
#    $path/irsnd.c \
#    $path/irsnd.h \
#    $path/irsndconfig.h \
#    $path/README.txt
unzip -j $ar \
    $path/irmp.c \
    $path/irmp.h \
    $path/irmpconfig.h.max \
    $path/irmpprotocols.h \
    $path/irmpsystem.h \
    $path/irsnd.c \
    $path/irsnd.h \
    $path/irsndconfig.h.max \
    $path/README.txt
mv irmpconfig.h.max irmpconfig.h
mv irsndconfig.h.max irsndconfig.h
cd ..

# patch
patch -d usb_hid -p1 -i ../patches/usb_hid.patch --binary
patch -d stm_lib -p1 -i ../patches/eeprom.patch
patch -d cmsis_boot -p1 -i ../patches/stm32f4xx_conf.patch
patch -d cmsis_boot -p1 -i ../patches/startup.patch
patch -d cmsis_boot -p1 -i ../patches/system.patch
patch -d irmp -p1 -i ../patches/irmp.patch

# mark as prepared
touch ./ext_src/prepared
