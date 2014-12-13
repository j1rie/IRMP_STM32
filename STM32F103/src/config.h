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
 * Eeprom page size = 1kB, 256 pairs of 16bit variable + address,
 * IRdata needs 3 variables, 256 / 3 = 85
 */
#define MACRO_SLOTS	8
#define MACRO_DEPTH	8
#define WAKE_SLOTS	8
#define SIZEOF_IR	6

#define MIN_REPEATS	2  // TODO make configurable & use Eeprom

/* uncomment this, if you use a ST-Link */ /* atm you can't use WAKEUP_RESET & ST_Link */
//#define ST_Link

/*
 * only if you want to use CLK and DIO on the blue ST-Link Emulator with mistakenly connected Pins
 * WARNING: further firmware updates will become difficult!
 * better use TMS and TCK instead, and leave this commented out
 */
//#define BlueLink_Remap

/* for use of wakeup reset pin */
#define WAKEUP_RESET

#ifdef BlueLink_Remap
	#define OUT_PORT	GPIOA
	#define LED_PIN		GPIO_Pin_14
	#define WAKEUP_PIN	GPIO_Pin_13
#ifdef WAKEUP_RESET
	#define WAKEUP_RESET_PIN	GPIO_Pin_6 /* TODO makes it possible to use another for remap */
#endif /* WAKEUP_RESET */
#else
	#define OUT_PORT	GPIOB
	#define LED_PIN		GPIO_Pin_13
	#define WAKEUP_PIN	GPIO_Pin_14
#ifdef WAKEUP_RESET
#ifndef ST_Link
	#define WAKEUP_RESET_PIN GPIO_Pin_12 /* TODO take out IRsnd for WAKEUP_RESET & ST_Link */
#else
	#define WAKEUP_RESET_PIN GPIO_Pin_6
#endif /* ST_Link */
#endif /* WAKEUP_RESET */
#endif /* BlueLink_Remap */

#endif /* __CONFIG_H */
