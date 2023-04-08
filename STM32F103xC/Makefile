-include Make.config

TARGET ?= F103x8
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
ifdef DEBUG
INCLUDES += -Isemihosting -Istdio
endif
DEFINES = -D$(PLATFORM) -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DSOF_CALLBACK

OBJS = cmsis_boot/startup/startup_stm32f10x_md.o cmsis_boot/system_stm32f10x.o\
       src/irmpmain.o src/st_link_leds.o src/usb_hid.o src/main.o\
       irmp/irmp.o irmp/irsnd.o\
       stm_lib/src/misc.o stm_lib/src/stm32f10x_exti.o\
       stm_lib/src/stm32f10x_flash.o stm_lib/src/stm32f10x_gpio.o\
       stm_lib/src/stm32f10x_pwr.o stm_lib/src/stm32f10x_rcc.o\
       stm_lib/src/stm32f10x_tim.o stm_lib/src/stm32f10x_usart.o\
       stm_lib/src/eeprom.o\
       usb_hid/src/hw_config.o usb_hid/src/stm32_it.o usb_hid/src/usb_desc.o\
       usb_hid/src/usb_endp.o usb_hid/src/usb_istr.o usb_hid/src/usb_prop.o\
       usb_hid/src/usb_pwr.o usb_hid/src/usb_core.o usb_hid/src/usb_init.o\
       usb_hid/src/usb_int.o usb_hid/src/usb_mem.o usb_hid/src/usb_regs.o\
       usb_hid/src/usb_sil.o src/romtable.o

ifdef DEBUG
OBJS += semihosting/semihosting.o semihosting/sh_cmd.o  stdio/printf.o
endif

CFLAGS = -Wall -ffunction-sections -fno-builtin $(ARCH) $(COMMON) $(INCLUDES) $(DEFINES)

ifeq ($(Platform), Blue)
DEFINES += -DBlueLink
TARGET := $(TARGET)_Blue
endif
ifeq ($(Platform), Red)
DEFINES += -DRedLink
TARGET := $(TARGET)_Red
endif
ifeq ($(Platform), RedCrap)
DEFINES += -DRedLinkCrap
TARGET := $(TARGET)_RedCrap
endif
ifeq ($(Platform), Stick)
DEFINES += -DStickLink
TARGET := $(TARGET)_Stick
endif
ifeq ($(Platform), Green)
DEFINES += -DGreenLink
TARGET := $(TARGET)_Green
endif
ifeq ($(Platform), Dev)
DEFINES += -DDeveloperBoard
TARGET := $(TARGET)_Dev
endif
ifeq ($(Platform), blueDev)
DEFINES += -DBlueDeveloperBoard
TARGET := $(TARGET)_blueDev
endif
ifeq ($(Platform), blackDev)
DEFINES += -DBlackDeveloperBoard
TARGET := $(TARGET)_blackDev
endif
ifeq ($(Platform), blackDevTest)
DEFINES += -DBlackDeveloperBoardTest
TARGET := $(TARGET)_blackDevTest
endif
ifeq ($(Platform), MapleMini)
DEFINES += -DMapleMini
TARGET := $(TARGET)_MapleMini
BOOT = Maple_Boot
endif
ifeq ($(Platform), MapleMini_2k)
DEFINES += -DMapleMini_2k
TARGET := $(TARGET)_MapleMini_2k
endif
ifeq ($(Platform), MapleMini_ExtBd)
DEFINES += -DMapleMini_ExtBd
TARGET := $(TARGET)_MapleMini_ExtBd
BOOT = Maple_Boot
endif
ifeq ($(Platform), MapleMini_2k_ExtBd)
DEFINES += -DMapleMini_2k_ExtBd
TARGET := $(TARGET)_MapleMini_2k_ExtBd
endif

DATE := $(shell date +"%F_%H-%M")
TARGET := $(DATE)_$(TARGET)

IRMP_VERSION := $(shell grep -e "Version IRMP" irmp/README.txt | awk '{print $$3}')
DEFINES += -DFW_STR="\"$(TARGET)   IRMP-Version: $(IRMP_VERSION)\""

ifdef Bootloader
DEFINES += -DBootloader
TARGET := $(TARGET)_BL
ifneq ($(BOOT), Maple_Boot)
LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--gc-sections,--entry=main,-Tscripts/arm-gcc-link-bootloader.ld $(ARCH) $(COMMON)
else
LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--gc-sections,--entry=main,-Tscripts/arm-gcc-link-maple.ld $(ARCH) $(COMMON)
endif
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

ifdef Pulldown
DEFINES += -DPulldown
TARGET := $(TARGET)_PD
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
