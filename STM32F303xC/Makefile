-include Make.config
TARGET ?= F303xC

PLATFORM ?= STM32F303CC
#STM32F303CC
#	MD PL - medium density performance line
#	max. 72MHz
#	256 KB (0x40000) of Flash memory
#	40 KB (0x0A000) of SRAM

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
DEFINES = -D$(PLATFORM) -DSTM32F30X -DUSE_STDPERIPH_DRIVER -DSOF_CALLBACK -DSUSP_CALLBACK

OBJS = cmsis_boot/startup/startup_stm32f30x.o cmsis_boot/system_stm32f30x.o\
       src/irmpmain.o src/st_link_leds.o src/usb_hid.o src/main.o\
       irmp/irmp.o irmp/irsnd.o\
       stm_lib/src/stm32f30x_misc.o stm_lib/src/stm32f30x_exti.o\
       stm_lib/src/stm32f30x_flash.o stm_lib/src/stm32f30x_gpio.o\
       stm_lib/src/stm32f30x_pwr.o stm_lib/src/stm32f30x_rcc.o\
       stm_lib/src/stm32f30x_tim.o stm_lib/src/stm32f30x_usart.o\
       stm_lib/src/eeprom.o\
       usb_hid/src/hw_config.o usb_hid/src/stm32_it.o usb_hid/src/usb_desc.o\
       usb_hid/src/usb_endp.o usb_hid/src/usb_istr.o usb_hid/src/usb_prop.o\
       usb_hid/src/usb_pwr.o usb_hid/src/usb_core.o usb_hid/src/usb_init.o\
       usb_hid/src/usb_int.o usb_hid/src/usb_mem.o usb_hid/src/usb_regs.o\
       usb_hid/src/usb_sil.o
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
LDFLAGS += --specs=nano.specs --specs=nosys.specs

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
