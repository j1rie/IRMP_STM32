#!/bin/bash
# go into the parent directory of the directory, in which the script is located
cd "$(dirname "$(readlink -e "$0")")"
cd ..

# download
mkdir -p ext_src
[[ -e ./ext_src/prepared ]] && exit 0
cd ./ext_src
for i in en.stsw-stm32010 en.stsw-stm32121; do
	if [[ ! -s $i.zip ]]; then
		echo 'unfortunately you have to download these files from ST manually and put them into ext_src'
		echo 'http://www.st.com/en/embedded-software/stsw-stm32121.html'
		echo 'http://www.st.com/en/embedded-software/stsw-stm32010.html'
		exit
		#wget "http://www.st.com/st-web-ui/static/active/en/st_prod_software_internet/resource/technical/software/firmware/stsw-stm$i.zip"
	fi
done
if [[ ! -s irmp.tar.gz ]]; then
	wget "http://www.mikrocontroller.net/svnbrowser/irmp/?view=tar" -O irmp.tar.gz
fi

# extract
cd ..
ar2='../../ext_src/en.stsw-stm32121.zip'
ar1='../ext_src/en.stsw-stm32121.zip'
ver='4.1.0'
path="STM32_USB-FS-Device_Lib_V$ver"
mkdir -p cmsis_boot
cd cmsis_boot
unzip -j $ar1 \
      $path/Libraries/CMSIS/Device/ST/STM32F30x/Include/stm32f30x.h \
      $path/Libraries/CMSIS/Device/ST/STM32F30x/Include/system_stm32f30x.h \
      $path/Projects/Custom_HID/src/system_stm32f30x.c \
      $path/Projects/Custom_HID/inc/stm32f30x_conf.h
mkdir -p startup
cd startup
unzip -j $ar2 $path/Libraries/CMSIS/Device/ST/STM32F30x/Source/Templates/gcc/startup_stm32f30x.s
cd ../..
mkdir -p cmsis
cd cmsis
unzip -j $ar1 \
      $path/Libraries/CMSIS/Include/core_cm4.h \
      $path/Libraries/CMSIS/Include/core_cmSimd.h \
      $path/Libraries/CMSIS/Include/core_cmFunc.h \
      $path/Libraries/CMSIS/Include/core_cmInstr.h
cd ..
mkdir -p stm_lib
cd  stm_lib
mkdir -p inc
cd inc
unzip -j $ar2 \
      $path/Libraries/STM32F30x_StdPeriph_Driver/inc/stm32f30x_misc.h \
      $path/Libraries/STM32F30x_StdPeriph_Driver/inc/stm32f30x_exti.h \
      $path/Libraries/STM32F30x_StdPeriph_Driver/inc/stm32f30x_flash.h \
      $path/Libraries/STM32F30x_StdPeriph_Driver/inc/stm32f30x_gpio.h \
      $path/Libraries/STM32F30x_StdPeriph_Driver/inc/stm32f30x_pwr.h \
      $path/Libraries/STM32F30x_StdPeriph_Driver/inc/stm32f30x_rcc.h \
      $path/Libraries/STM32F30x_StdPeriph_Driver/inc/stm32f30x_tim.h \
      $path/Libraries/STM32F30x_StdPeriph_Driver/inc/stm32f30x_usart.h
cd ..
mkdir -p src
cd src
unzip -j $ar2 \
      $path/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_misc.c \
      $path/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_exti.c \
      $path/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_flash.c \
      $path/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_gpio.c \
      $path/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_pwr.c \
      $path/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_rcc.c \
      $path/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_tim.c \
      $path/Libraries/STM32F30x_StdPeriph_Driver/src/stm32f30x_usart.c
cd ../..
mkdir -p usb_hid
cd usb_hid
mkdir -p inc
cd inc
unzip -j $ar2 \
      $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_core.h \
      $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_def.h \
      $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_init.h \
      $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_int.h \
      $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_lib.h \
      $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_mem.h \
      $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_regs.h \
      $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_sil.h \
      $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_type.h \
      $path/Projects/Custom_HID/inc/platform_config.h \
      $path/Projects/Custom_HID/inc/hw_config.h \
      $path/Projects/Custom_HID/inc/stm32_it.h \
      $path/Projects/Custom_HID/inc/usb_conf.h \
      $path/Projects/Custom_HID/inc/usb_desc.h \
      $path/Projects/Custom_HID/inc/usb_istr.h \
      $path/Projects/Custom_HID/inc/usb_prop.h \
      $path/Projects/Custom_HID/inc/usb_pwr.h
cd ..
mkdir -p src
cd src
unzip -j $ar2 \
      $path/Projects/Custom_HID/src/hw_config.c \
      $path/Projects/Custom_HID/src/stm32_it.c \
      $path/Projects/Custom_HID/src/usb_desc.c \
      $path/Projects/Custom_HID/src/usb_endp.c \
      $path/Projects/Custom_HID/src/usb_istr.c \
      $path/Projects/Custom_HID/src/usb_prop.c \
      $path/Projects/Custom_HID/src/usb_pwr.c \
      $path/Libraries/STM32_USB-FS-Device_Driver/src/usb_core.c \
      $path/Libraries/STM32_USB-FS-Device_Driver/src/usb_init.c \
      $path/Libraries/STM32_USB-FS-Device_Driver/src/usb_int.c \
      $path/Libraries/STM32_USB-FS-Device_Driver/src/usb_mem.c \
      $path/Libraries/STM32_USB-FS-Device_Driver/src/usb_regs.c \
      $path/Libraries/STM32_USB-FS-Device_Driver/src/usb_sil.c

ar='../../ext_src/en.stsw-stm32010.zip'
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
patch -d cmsis_boot -p1 -i ../patches/stm32f30x_conf.patch
patch -d cmsis_boot -p1 -i ../patches/startup.patch
patch -d cmsis_boot -p1 -i ../patches/system.patch
patch -d irmp -p1 -i ../patches/irmp.patch

# mark as prepared
touch ./ext_src/prepared
