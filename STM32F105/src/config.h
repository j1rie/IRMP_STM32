/*
 * Copyright (C) 2014-2015 Joerg Riechardt
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

#define FW_STR "2015-10-26_F105-jrie"

#define MIN_REPEATS	2  // TODO make configurable & use Eeprom

/* B11 IRMP (irmpconfig.h), B6 IRSND (irsndconfig.h) , B10 Logging (irmp.c) */

#define OUT_PORT	GPIOB
#define LED_PIN		GPIO_Pin_12 // blue LED
#define WAKEUP_PIN	GPIO_Pin_7
#define RESET_PORT	GPIOB
#define WAKEUP_RESET_PIN GPIO_Pin_8

#endif /* __CONFIG_H */
