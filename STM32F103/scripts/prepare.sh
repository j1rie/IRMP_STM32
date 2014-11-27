#!/bin/bash
[[ -e ext/prepared ]] && exit 0
for i in 32010 32121; do
	if [[ ! -s stsw-stm$i.zip ]]; then
		wget "http://www.st.com/st-web-ui/static/active/en/st_prod_software_internet/resource/technical/software/firmware/stsw-stm$i.zip"
	fi
done
if [[ ! -s irmp.tar.gz ]]; then
	wget "http://www.mikrocontroller.net/svnbrowser/irmp/?view=tar" -O irmp.tar.gz
fi

ar='../../stsw-stm32121.zip'
ver='4.0.0'
dst="ext/stm32lib-$ver"
path="STM32_USB-FS-Device_Lib_V$ver"
mkdir -p $dst
cd $dst
unzip -j $ar $path/Libraries/CMSIS/Device/ST/STM32F10x/Include/stm32f10x.h
unzip -j $ar $path/Libraries/CMSIS/Device/ST/STM32F10x/Include/system_stm32f10x.h
unzip -j $ar $path/Libraries/CMSIS/Device/ST/STM32F10x/Source/Templates/gcc_ride7/startup_stm32f10x_md.s
unzip -j $ar $path/Libraries/CMSIS/Include/core_cm3.h
unzip -j $ar $path/Libraries/CMSIS/Include/core_cmFunc.h
unzip -j $ar $path/Libraries/CMSIS/Include/core_cmInstr.h
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/inc/misc.h
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_exti.h
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_flash.h
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_gpio.h
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_pwr.h
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_rcc.h
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_tim.h
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_usart.h
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/src/misc.c
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.c
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c
unzip -j $ar $path/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_core.h
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_def.h
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_init.h
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_int.h
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_lib.h
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_mem.h
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_regs.h
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_sil.h
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/inc/usb_type.h
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/src/usb_core.c
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/src/usb_init.c
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/src/usb_int.c
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/src/usb_mem.c
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/src/usb_regs.c
unzip -j $ar $path/Libraries/STM32_USB-FS-Device_Driver/src/usb_sil.c
unzip -j $ar $path/Projects/Custom_HID/inc/platform_config.h
unzip -j $ar $path/Projects/Custom_HID/inc/hw_config.h
unzip -j $ar $path/Projects/Custom_HID/inc/stm32_it.h
unzip -j $ar $path/Projects/Custom_HID/inc/stm32f10x_conf.h
unzip -j $ar $path/Projects/Custom_HID/inc/usb_conf.h
unzip -j $ar $path/Projects/Custom_HID/inc/usb_desc.h
unzip -j $ar $path/Projects/Custom_HID/inc/usb_istr.h
unzip -j $ar $path/Projects/Custom_HID/inc/usb_prop.h
unzip -j $ar $path/Projects/Custom_HID/inc/usb_pwr.h
unzip -j $ar $path/Projects/Custom_HID/src/hw_config.c
unzip -j $ar $path/Projects/Custom_HID/src/stm32_it.c
unzip -j $ar $path/Projects/Custom_HID/src/system_stm32f10x.c
unzip -j $ar $path/Projects/Custom_HID/src/usb_desc.c
unzip -j $ar $path/Projects/Custom_HID/src/usb_endp.c
unzip -j $ar $path/Projects/Custom_HID/src/usb_istr.c
unzip -j $ar $path/Projects/Custom_HID/src/usb_prop.c
unzip -j $ar $path/Projects/Custom_HID/src/usb_pwr.c

ar='../../stsw-stm32010.zip'
ver='3.1.0'
path="STM32F10x_AN2594_FW_V$ver"
unzip -j $ar $path/Project/EEPROM_Emulation/src/eeprom.c
unzip -j $ar $path/Project/EEPROM_Emulation/inc/eeprom.h
patch -p1 -i ../../scripts/usb_hid.patch
patch -p1 -i ../../scripts/eeprom.patch
patch -p1 -i ../../scripts/stm32f10x_conf.patch
cd ../..

ar='../../irmp.tar.gz'
ver='2.6.7'
dst="ext/irmp-$ver"
path="irmp"
mkdir -p $dst
cd $dst
tar -xvf $ar --strip-components=1 $path/irmp.c
tar -xvf $ar --strip-components=1 $path/irmp.h
tar -xvf $ar --strip-components=1 $path/irmpconfig.h
tar -xvf $ar --strip-components=1 $path/irmpprotocols.h
tar -xvf $ar --strip-components=1 $path/irmpsystem.h
tar -xvf $ar --strip-components=1 $path/irsnd.c
tar -xvf $ar --strip-components=1 $path/irsnd.h
tar -xvf $ar --strip-components=1 $path/irsndconfig.h
patch -p1 -i ../../scripts/irmp.patch

touch ../prepared
