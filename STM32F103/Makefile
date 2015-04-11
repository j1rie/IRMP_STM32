TARGET ?= IR
PLATFORM ?= STM32F103C8
#STM32F103C8
#	MD PL - medium density performance line
#	max. 72MHz
#	64 KB (0x10000) of Flash memory
#	20 KB (0x05000) of SRAM

CROSS_COMPILE ?= arm-none-eabi-
AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
LD = $(CC)
OBJCP = $(CROSS_COMPILE)objcopy

ARCH = -mcpu=cortex-m3 -mthumb
COMMON = -g -Os -flto
INCLUDES = -Icmsis -Icmsis_boot -Istm_lib/inc -Iusb_hid/inc -Iirmp -Isrc
DEFINES = -D$(PLATFORM) -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER

OBJS = src/irmpmain.o src/st_link_leds.o src/usb_hid.o src/main.o\
       irmp/irmp.o irmp/irsnd.o\
       cmsis_boot/startup/startup_stm32f10x_md.o cmsis_boot/system_stm32f10x.o\
       stm_lib/src/misc.o stm_lib/src/stm32f10x_exti.o\
       stm_lib/src/stm32f10x_flash.o stm_lib/src/stm32f10x_gpio.o\
       stm_lib/src/stm32f10x_pwr.o stm_lib/src/stm32f10x_rcc.o\
       stm_lib/src/stm32f10x_tim.o stm_lib/src/stm32f10x_usart.o\
       stm_lib/src/eeprom.o\
       usb_hid/src/hw_config.o usb_hid/src/stm32_it.o usb_hid/src/usb_desc.o\
       usb_hid/src/usb_endp.o usb_hid/src/usb_istr.o usb_hid/src/usb_prop.o\
       usb_hid/src/usb_pwr.o usb_hid/src/usb_core.o usb_hid/src/usb_init.o\
       usb_hid/src/usb_int.o usb_hid/src/usb_mem.o usb_hid/src/usb_regs.o\
       usb_hid/src/usb_sil.o

CFLAGS = -Wall -ffunction-sections -fno-builtin $(ARCH) $(COMMON) $(INCLUDES) $(DEFINES)
ifdef Bootloader
DEFINES += -DBootloader
LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--gc-sections,--entry=main,-Tscripts/arm-gcc-link-bootloader.ld $(ARCH) $(COMMONOC)
else
LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--gc-sections,--entry=main,-Tscripts/arm-gcc-link.ld $(ARCH) $(COMMON)
endif
$(TARGET).bin: $(TARGET).elf
	$(OBJCP) -O binary $< $@

$(TARGET).elf: prepare $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

.PHONY: prepare flash clean distclean
prepare:
	@./scripts/prepare.sh

flash: $(TARGET).bin
	stm32flash -v -w $(TARGET).bin /dev/ttyUSB0

clean:
	rm -f $(OBJS) $(TARGET).{bin,elf,map}
	rm -rf cmsis cmsis_boot irmp stm_lib usb_hid ext_src/prepared

distclean: clean
	rm -rf ext_src
