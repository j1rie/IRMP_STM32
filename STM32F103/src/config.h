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

/* MACRO_SLOTS x (MACRO_DEPTH + 1) + WAKE_SLOTS < 85
 * Eeprom page size = 1kB - 4, 255 pairs of 16bit variable + address,
 * IRdata needs 3 variables, 255 / 3 = 85
 */
#define MACRO_SLOTS	8
#define MACRO_DEPTH	8
#define WAKE_SLOTS	8
#define SIZEOF_IR	6
#define MIN_REPEATS	2  // TODO make configurable & use Eeprom

/* uncomment below for CooCox */
//#define FW_STR "2017-01-14_00-00_Dev_SC_BL-jrie"

/* uncomment this, if you use the blue ST-Link */
//#define BlueLink

/* uncomment this, if you use the red ST-Link */
//#define RedLink

/* uncomment this, if you use the ST-Link USB stick */
//#define StickLink

/* uncomment this, if you use the developer board */
//#define DeveloperBoard

/* uncomment this, if you use the blue developer board */
//#define BlueDeveloperBoard

/* uncomment this, if you use the Maple Mini */
//#define MapleMini

/* uncomment this in order to pull down the "active" pin of the mainboard power button connector directly */
//#define SimpleCircuit

/* uncomment this for legacy boards with pulldown resistor for USB reset */
//#define PullDown

#if defined(BlueLink) || defined(RedLink) || defined(StickLink)
	#define ST_Link
#endif

/* B6 IRSND (irsndconfig.h) , B10 Logging (irmp.c) */

#if defined(BlueLink) /* blue ST-Link */
	#define OUT_PORT		GPIOA
	#define WAKEUP_PIN		GPIO_Pin_13
	#define RESET_PIN		GPIO_Pin_14
	#define IR_IN_PORT		B
	#define IR_IN_PIN		11
	#define RESET_PORT		GPIOB
	#define WAKEUP_RESET_PIN	GPIO_Pin_14
	#define USB_DISC_PORT		GPIOB
	#define USB_DISC_RCC_APB2Periph	RCC_APB2Periph_GPIOB /* TODO use concat */
	#define USB_DISC_PIN		GPIO_Pin_13
#elif defined(RedLink) /* red ST-Link */
	#define OUT_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_14
	#define RESET_PIN		GPIO_Pin_13
	#define IR_IN_PORT		B
	#define IR_IN_PIN		11
	#define RESET_PORT		GPIOA
	#define WAKEUP_RESET_PIN	GPIO_Pin_14
	#define USB_DISC_PORT		GPIOA
	#define USB_DISC_RCC_APB2Periph	RCC_APB2Periph_GPIOA /* TODO use concat */
	#define USB_DISC_PIN		GPIO_Pin_13
#elif defined(StickLink) /* ST-Link stick, avoid hassle with disabled SWD */
	#define OUT_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_14
	#define RESET_PIN		GPIO_Pin_13
	#define IR_IN_PORT		B
	#define IR_IN_PIN		11
#elif defined(DeveloperBoard) /* classic developer board */
	#define OUT_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_14
	#define RESET_PIN		GPIO_Pin_15
	#define IR_IN_PORT		B
	#define IR_IN_PIN		11
	#define LED_PORT		GPIOB
	#define LED_PIN			GPIO_Pin_13
	#define RESET_PORT		GPIOB
	#define WAKEUP_RESET_PIN	GPIO_Pin_12
#elif defined(BlueDeveloperBoard) /* blue developer board */
	#define OUT_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_14
	#define RESET_PIN		GPIO_Pin_15
	#define IR_IN_PORT		B
	#define IR_IN_PIN		9
	#define LED_PORT		GPIOC
	#define LED_PIN			GPIO_Pin_13
	#define RESET_PORT		GPIOB
	#define WAKEUP_RESET_PIN	GPIO_Pin_12
#elif defined(MapleMini) /* Maple Mini */
	#define OUT_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_6
	#define RESET_PIN		GPIO_Pin_15
	#define IR_IN_PORT		B
	#define IR_IN_PIN		11
	#define LED_PORT		GPIOB
	#define LED_PIN			GPIO_Pin_1
	#define RESET_PORT		GPIOB
	#define WAKEUP_RESET_PIN	GPIO_Pin_8
	#define USB_DISC_PORT		GPIOB
	#define USB_DISC_RCC_APB2Periph	RCC_APB2Periph_GPIOB /* TODO use concat */
	#define USB_DISC_PIN		GPIO_Pin_9
	#define PullDown
#else
	#error "Missing define for board"
#endif /* BlueLink */

#endif /* __CONFIG_H */
