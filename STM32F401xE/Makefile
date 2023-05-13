-include Make.config
TARGET ?= F401xE
PLATFORM ?= STM32F4XX
#STM32F401CE
#	
#	max. 84MHz
#	512 KB (0x80000) of Flash memory
#	 96 KB (0x18000) of SRAM
#	eeprom emulation 0x8008000 - 0x8010000
#	firmware for bootloader 0x8010000

CROSS_COMPILE ?= arm-none-eabi-
AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
LD = $(CC)
OBJCP = $(CROSS_COMPILE)objcopy

ARCH = -mcpu=cortex-m4 -mthumb
COMMON = -g -Os -flto
INCLUDES = -Icmsis -Icmsis_boot -Istm_lib/inc -Iusb_hid/inc -Iirmp -Isrc
ifdef DEBUG
INCLUDES += -Isemihosting -Istdio
endif
DEFINES = -D$(PLATFORM) -DSTM32F401xx -DUSE_STDPERIPH_DRIVER

OBJS = cmsis_boot/startup/startup_stm32f401xx.o cmsis_boot/system_stm32f4xx.o\
       src/irmpmain.o src/st_link_leds.o src/usb_hid.o src/main.o\
       irmp/irmp.o irmp/irsnd.o\
       stm_lib/src/misc.o\
       stm_lib/src/stm32f4xx_flash.o stm_lib/src/stm32f4xx_gpio.o\
       stm_lib/src/stm32f4xx_rcc.o\
       stm_lib/src/stm32f4xx_tim.o stm_lib/src/stm32f4xx_usart.o\
       stm_lib/src/eeprom.o\
       usb_hid/src/usb_bsp.o usb_hid/src/usb_core.o usb_hid/src/usb_dcd.o\
       usb_hid/src/usb_dcd_int.o usb_hid/src/usbd_core.o\
       usb_hid/src/usbd_desc.o\
       usb_hid/src/usbd_hid_core.o usb_hid/src/usbd_ioreq.o\
       usb_hid/src/usbd_req.o usb_hid/src/usbd_usr.o
ifdef DEBUG
OBJS += semihosting/semihosting.o semihosting/sh_cmd.o  stdio/printf.o
endif

CFLAGS = -Wall -ffunction-sections -fno-builtin $(ARCH) $(COMMON) $(INCLUDES) $(DEFINES)

ifeq ($(Platform), blueDev)
DEFINES += -DBlueDeveloperBoard
TARGET := $(TARGET)_blueDev
endif

DATE := $(shell date +"%F_%H-%M")
TARGET := $(DATE)_$(TARGET)

IRMP_VERSION := $(shell grep -e "Version IRMP" irmp/README.txt | awk '{print $$3}')
DEFINES += -DFW_STR="\"$(TARGET)   IRMP-Version: $(IRMP_VERSION)*STM32\""

ifdef Bootloader
DEFINES += -DBootloader
TARGET := $(TARGET)_BL
LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--gc-sections,--entry=main,-Tscripts/arm-gcc-link-bootloader.ld $(ARCH) $(COMMON)
else
LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--gc-sections,--entry=main,-Tscripts/arm-gcc-link.ld $(ARCH) $(COMMON)
endif
LDFLAGS += --specs=nano.specs

ifdef SimpleCircuit
DEFINES += -DSimpleCircuit
TARGET := $(TARGET)_SC
endif

ifdef NOEXTLED
DEFINES += -DNOEXTLED
TARGET := $(TARGET)_RS
endif

ifdef TM1637
INCLUDES += -Itm1637
OBJS += tm1637/tm1637.o
DEFINES += -DTM1637
TARGET := $(TARGET)_TM
endif

ifdef DEBUG
DEFINES += -DDEBUG
TARGET := $(TARGET)_DBG
endif

COMPILED_BY = jrie
TARGET := $(TARGET)_$(COMPILED_BY)

$(TARGET).bin: $(TARGET).elf
	$(OBJCP) -O binary $< $@

$(TARGET).elf: prepare $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

.PHONY: prepare flash clean-objects clean distclean

prepare:
	@./scripts/prepare.sh

flash: $(TARGET).bin
	stm32flash -v -w $(TARGET).bin /dev/ttyUSB0

clean-objects:
	rm -f $(OBJS) $(TARGET).{bin,elf,map}

clean: clean-objects
	rm -rf cmsis cmsis_boot irmp stm_lib usb_hid ext_src/prepared tm1637

distclean: clean
	rm -rf ext_src
