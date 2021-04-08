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

/* uncomment below for CooCox */
//#define FW_STR "2015-10-26_00-00_F105_jrie   IRMP-Version: 3.07"

/* uncomment this in order to pull down the "active" pin of the mainboard power button connector directly */
//#define SimpleCircuit_F105

/* uncomment this for an external LED instead of the reset functionality */
#define EXTLED

/* C6 IRMP (irmpconfig.h), B6 IRSND (irsndconfig.h) Yaw-right, B10 Logging (irmp.c) */
#define IR_IN_PORT	C
#define IR_IN_PIN	6
#define IR_OUT_PORT	B
#define IR_OUT_PIN	6

/* On this board activate for pull high the A{0,1,2} pins and for pull low the B{6,7,8} pins, BUT NEVER BOTH,
 * due to the level shifting complementary MOSFETs and the level shifting transistors in the A{0,1,2} paths.
 * Unfortunately 3 resistors need to be changed for A{0,1,2} to work with 5V, so better use B{6,7,8} */
#ifdef SimpleCircuit_F105
#define WAKEUP_PORT	GPIOB
#define WAKEUP_PIN	GPIO_Pin_7 /* Yaw-middle */
#ifndef EXTLED
#define RESET_PORT	GPIOB
#define RESET_PIN	GPIO_Pin_8 /* Yaw-left */
#else
#define EXTLED_PORT	GPIOB
#define EXTLED_PIN	GPIO_Pin_8 /* connect EXTLED to Yaw-left and 3,3V */
#endif
#else
#define WAKEUP_PORT	GPIOA
#define WAKEUP_PIN	GPIO_Pin_1 /* Yaw-middle */
#ifndef EXTLED
#define RESET_PORT	GPIOA
#define RESET_PIN	GPIO_Pin_2 /* Yaw-left */
#else
#define EXTLED_PORT	GPIOA
#define EXTLED_PIN	GPIO_Pin_2 /* connect EXTLED to Yaw-left and GND */
#endif
#endif
#define LED_PORT	GPIOB
#define LED_PIN		GPIO_Pin_12

#endif /* __CONFIG_H */
