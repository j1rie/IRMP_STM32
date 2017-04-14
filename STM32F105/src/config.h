/*
 * Copyright (C) 2014-2017 Joerg Riechardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* MACRO_SLOTS x (MACRO_DEPTH + 1) + WAKE_SLOTS < 170
 * Eeprom page size = 2kB - 4, 511 pairs of 16bit variable + address,
 * IRdata needs 3 variables, 511 / 3 = 170
 */
#define MACRO_SLOTS	8
#define MACRO_DEPTH	8
#define WAKE_SLOTS	8
#define SIZEOF_IR	6
#define MIN_REPEATS	2  // TODO make configurable & use Eeprom

/* uncomment below for CooCox */
//#define FW_STR "2015-10-26_00-00_F105_jrie"

/* uncomment this in order to pull down the "active" pin of the mainboard power button connector directly */
//#define SimpleCircuit_F105

/* C6 IRMP (irmpconfig.h), B6 IRSND (irsndconfig.h) , B10 Logging (irmp.c) */

#ifdef SimpleCircuit_F105
#define WAKEUP_PORT	GPIOB
#define WAKEUP_PIN	GPIO_Pin_7
#define RESET_PORT	GPIOB
#define RESET_PIN	GPIO_Pin_8
#else
#define WAKEUP_PORT	GPIOA
#define WAKEUP_PIN	GPIO_Pin_1
#define RESET_PORT	GPIOA
#define RESET_PIN	GPIO_Pin_2
#endif
#define LED_PORT	GPIOB
#define LED_PIN		GPIO_Pin_12

#endif /* __CONFIG_H */
