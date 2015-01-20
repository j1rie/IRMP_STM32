/*
 * Copyright (C) 2014 Joerg Riechardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* MACRO_SLOTS x (MACRO_DEPTH + 1) + WAKE_SLOTS < 85
 * Eeprom page size = 1kB - 4, 255 pairs of 16bit variable + address,
 * IRdata needs 3 variables, 255 / 3 = 85
 */
#define MACRO_SLOTS	8
#define MACRO_DEPTH	8
#define WAKE_SLOTS	8
#define SIZEOF_IR	6

#define MIN_REPEATS	2  // TODO make configurable & use Eeprom

/* in case you use the bootloader at 0x8000000, and this firmware at 0x8002000 */
#define Bootloader

/* uncomment this, if you use a ST-Link */
//#define ST_Link

/* uncomment this, if you want to use the ST-Link LEDs */
//#define ST_Link_LEDs

#ifdef ST_Link_LEDs
	#define ST_Link
#endif /* ST_Link_LEDs */

/* only if you want to use CLK and DIO on the blue ST-Link Emulator with mistakenly connected Pins
 * WARNING: further firmware updates will become difficult!
 * you have to flash with --reset, and pull RST low until short after flash command
 * better use TMS and TCK instead, and leave this commented out
 * OR use the bootloader and avoid the hassle */
//#define BlueLink_Remap

#ifdef BlueLink_Remap
	#define ST_Link
#endif /* BlueLink_Remap */

/* B11 IRMP (irmpconfig.h), B6 IRSND (irsndconfig.h) , B10 Logging (irmp.c) */

#ifdef BlueLink_Remap
	#define OUT_PORT	GPIOA
	#define LED_PIN		GPIO_Pin_14
	#define WAKEUP_PIN	GPIO_Pin_13
	#define RESET_PORT	GPIOB
	#define WAKEUP_RESET_PIN GPIO_Pin_14
	#define USB_DISC_PORT GPIOB
	#define USB_DISC_RCC_APB2Periph RCC_APB2Periph_GPIOB /* TODO use concat */
	#define USB_DISC_PIN  GPIO_Pin_13
#else /* red ST-Link, blue ST-Link without remap and developer board */
	#define OUT_PORT	GPIOB
	#define LED_PIN		GPIO_Pin_13
	#define WAKEUP_PIN	GPIO_Pin_14
#ifdef ST_Link /* red and blue ST-Link without remap */
	#define RESET_PORT	GPIOA
	#define WAKEUP_RESET_PIN GPIO_Pin_14
	#define USB_DISC_PORT GPIOA
	#define USB_DISC_RCC_APB2Periph RCC_APB2Periph_GPIOA /* TODO use concat */
	#define USB_DISC_PIN  GPIO_Pin_13
#else /* developer board */
	#define RESET_PORT	GPIOB
	#define WAKEUP_RESET_PIN GPIO_Pin_12
	#define USB_DISC_PORT GPIOA
	#define USB_DISC_RCC_APB2Periph RCC_APB2Periph_GPIOA /* TODO use concat */
	#define USB_DISC_PIN  GPIO_Pin_3
#endif /* ST_Link */
#endif /* BlueLink_Remap */

#endif /* __CONFIG_H */
