/*
 * Copyright (C) 2014-2021 Joerg Riechardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* MACRO_SLOTS x (MACRO_DEPTH + 1) + WAKE_SLOTS <= 1365
 * Eeprom page size = 16kB - 4, 4095 pairs of 16bit variable + address,
 * IRMP_DATA needs 3 variables, 4095 / 3 = 1365
 * Eeprom partition 1: MACRO_SLOTS x (MACRO_DEPTH + 1) IRMP_DATA
 * Eeprom partition 2: WAKE_SLOTS IRMP_DATA
 */
#define MACRO_SLOTS	8
#define MACRO_DEPTH	8
#define WAKE_SLOTS	8
#define SIZEOF_IR	6

/* uncomment below for CooCox */
//#define FW_STR "2017-04-20_00-00_MapleMini_BL_SC-jrie   IRMP-Version: 3.08"

/* uncomment this, if you use the blue ST-Link */
//#define BlueLink

/* uncomment this, if you use the red ST-Link or a ST-Link USB stick with two LEDs */
//#define RedLink

/* uncomment this, if you use a ST-Link USB stick with unconnected PB11 and two LEDs */
//#define RedLinkCrap

/* uncomment this, if you use a ST-Link USB stick with only one LED */
//#define StickLink

/* uncomment this, if you use the green ST-Link USB stick with only one LED */
//#define GreenLink

/* uncomment this, if you use the big developer board */
//#define DeveloperBoard

/* uncomment this, if you use the blue or red developer board */
//#define BlueDeveloperBoard

/* uncomment this, if you use the black developer board */
//#define BlackDeveloperBoard

/* uncomment this, if you use the black developer board test */
//#define BlackDeveloperBoardTest

/* uncomment this, if you use the Maple Mini */
//#define MapleMini

/* uncomment this, if you use the Maple Mini with bootloader jumping to 2k offset (default for Maple Mini is 5k) */
//#define MapleMini_2k

/* uncomment this, if you use the Maple Mini and the extension board */
//#define MapleMini_ExtBd

/* uncomment this, if you use the Maple Mini with bootloader jumping to 2k offset (default for Maple Mini is 5k) and the extension board */
//#define MapleMini_2k_ExtBd

/* uncomment this in order to pull down the "active" pin of the mainboard power button connector directly */
//#define SimpleCircuit

/* uncomment this for an external LED instead of the reset functionality on Blue-, Red-, Green- or StickLink */
#define EXTLED

/* uncomment this for legacy boards with pulldown resistor for USB reset */
//#define PullDown

/* for ST-Link USB sticks with only one LED no extra LED handling is needed */
#if defined(BlueLink) || defined(RedLink) || defined(RedLinkCrap)
	#define ST_Link
#endif
#if defined(MapleMini) || defined(MapleMini_2k) || defined(MapleMini_ExtBd) || defined(MapleMini_2k_ExtBd)
	#define Maple
#endif
#if defined(MapleMini) || defined(MapleMini_ExtBd)
	#define Maple_Boot
#endif


/* B6 IRSND (irsndconfig.h) , B10 Logging (irmp.c) */
#define IR_OUT_PORT B
#define IR_OUT_PIN  6

#if defined(BlueLink) /* blue ST-Link, IRSND = NRST */
	#define WAKEUP_PORT		GPIOA
	#define WAKEUP_PIN		GPIO_Pin_13 /* DIO */
#ifndef EXTLED
	#define RESET_PORT		GPIOA
	#define RESET_PIN		GPIO_Pin_14 /* CLK */
#else
	#define EXTLED_PORT		GPIOA
	#define EXTLED_PIN		GPIO_Pin_14 /* CLK */
#endif
	#define IR_IN_PORT		B
	#define IR_IN_PIN		11 /* SWIM */
	#define WAKEUP_RESET_PORT	GPIOB
	#define WAKEUP_RESET_PIN	GPIO_Pin_14 /* TMS */
	#define USB_DISC_PORT		GPIOB
	#define USB_DISC_RCC_APB2Periph	RCC_APB2Periph_GPIOB
	#define USB_DISC_PIN		GPIO_Pin_13 /* TCK */
#elif defined(RedLink) /* red ST-Link, IRSND = NRST */
	#define WAKEUP_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_14 /* DIO */
#ifndef EXTLED
	#define RESET_PORT		GPIOB
	#define RESET_PIN		GPIO_Pin_13 /* CLK */
#else
	#define EXTLED_PORT		GPIOB
	#define EXTLED_PIN		GPIO_Pin_13 /* CLK */
#endif
	#define IR_IN_PORT		B
	#define IR_IN_PIN		11 /* SWIM */
	#define WAKEUP_RESET_PORT	GPIOA
	#define WAKEUP_RESET_PIN	GPIO_Pin_14
	#define USB_DISC_PORT		GPIOA
	#define USB_DISC_RCC_APB2Periph	RCC_APB2Periph_GPIOA
	#define USB_DISC_PIN		GPIO_Pin_13
#elif defined(RedLinkCrap) /* red ST-Link without PB11 connection */
	#define WAKEUP_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_14 /* DIO */
#ifndef EXTLED
	#define RESET_PORT		GPIOB
	#define RESET_PIN		GPIO_Pin_13 /* CLK */
#else
	#define EXTLED_PORT		GPIOB
	#define EXTLED_PIN		GPIO_Pin_13 /* CLK */
#endif
	#define IR_IN_PORT		B
	#define IR_IN_PIN		8 /* SWIM */
	#define WAKEUP_RESET_PORT	GPIOA
	#define WAKEUP_RESET_PIN	GPIO_Pin_14
	#define USB_DISC_PORT		GPIOA
	#define USB_DISC_RCC_APB2Periph	RCC_APB2Periph_GPIOA
	#define USB_DISC_PIN		GPIO_Pin_13
#elif defined(StickLink) /* ST-Link stick, IRSND = RST */
	#define WAKEUP_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_14 /* DIO */
#ifndef EXTLED
	#define RESET_PORT		GPIOB
	#define RESET_PIN		GPIO_Pin_13 /* CLK */
#else
	#define EXTLED_PORT		GPIOB
	#define EXTLED_PIN		GPIO_Pin_13 /* CLK */
#endif
	#define IR_IN_PORT		B
	#define IR_IN_PIN		11 /* SWIM */
	#define LED_PORT		GPIOA
	#define LED_PIN			GPIO_Pin_9
	#define WAKEUP_RESET_PORT	GPIOA
	#define WAKEUP_RESET_PIN	GPIO_Pin_14
#elif defined(GreenLink) /* green ST-Link stick, IRSND = RST */
	#define WAKEUP_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_14 /* DIO */
#ifndef EXTLED
	#define RESET_PORT		GPIOB
	#define RESET_PIN		GPIO_Pin_13 /* CLK */
#else
	#define EXTLED_PORT		GPIOB
	#define EXTLED_PIN		GPIO_Pin_13 /* CLK */
#endif
	#define IR_IN_PORT		B
	#define IR_IN_PIN		11 /* SWIM */
	#define LED_PORT		GPIOA
	#define LED_PIN			GPIO_Pin_9
	#define WAKEUP_RESET_PORT	GPIOA
	#define WAKEUP_RESET_PIN	GPIO_Pin_14
#elif defined(DeveloperBoard) /* classic developer board */
	#define WAKEUP_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_14
	#define RESET_PORT		GPIOB
	#define RESET_PIN		GPIO_Pin_15
	#define EXTLED_PORT		GPIOB
	#define EXTLED_PIN		GPIO_Pin_10
	#define IR_IN_PORT		B
	#define IR_IN_PIN		11
	#define LED_PORT		GPIOB
	#define LED_PIN			GPIO_Pin_13
	#define WAKEUP_RESET_PORT	GPIOB
	#define WAKEUP_RESET_PIN	GPIO_Pin_12
#elif defined(BlueDeveloperBoard) /* blue developer board */
	#define WAKEUP_PORT		GPIOA
	#define WAKEUP_PIN		GPIO_Pin_14 /* CLK */
	#define RESET_PORT		GPIOB
	#define RESET_PIN		GPIO_Pin_14
	#define EXTLED_PORT		GPIOB
	#define EXTLED_PIN		GPIO_Pin_10
	#define IR_IN_PORT		A
	#define IR_IN_PIN		13 /* IO */
	#define LED_PORT		GPIOC
	#define LED_PIN			GPIO_Pin_13
	#define WAKEUP_RESET_PORT	GPIOB
	#define WAKEUP_RESET_PIN	GPIO_Pin_13
	#define BUTTON_PORT		GPIOA
	#define BUTTON_PIN		GPIO_Pin_0
#elif defined(BlackDeveloperBoard) /* black developer board */
	#define WAKEUP_PORT		GPIOA
	#define WAKEUP_PIN		GPIO_Pin_14 /* CLK */
	#define RESET_PORT		GPIOB
	#define RESET_PIN		GPIO_Pin_14
	#define EXTLED_PORT		GPIOB
	#define EXTLED_PIN		GPIO_Pin_10
	#define IR_IN_PORT		A
	#define IR_IN_PIN		13 /* IO */
	#define LED_PORT		GPIOB
	#define LED_PIN			GPIO_Pin_12
	#define WAKEUP_RESET_PORT	GPIOB
	#define WAKEUP_RESET_PIN	GPIO_Pin_13
#elif defined(BlackDeveloperBoardTest) /* black developer board test */
	#define WAKEUP_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_8
	#define RESET_PORT		GPIOB
	#define RESET_PIN		GPIO_Pin_14
	#define EXTLED_PORT		GPIOB
	#define EXTLED_PIN		GPIO_Pin_10
	#define IR_IN_PORT		B
	#define IR_IN_PIN		7
	#define LED_PORT		GPIOB
	#define LED_PIN			GPIO_Pin_12
	#define WAKEUP_RESET_PORT	GPIOB
	#define WAKEUP_RESET_PIN	GPIO_Pin_13
#elif defined(MapleMini) || defined(MapleMini_2k) /* Maple Mini */
	#define WAKEUP_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_10
	#define RESET_PORT		GPIOB
	#define RESET_PIN		GPIO_Pin_15
	#define EXTLED_PORT		GPIOB
	#define EXTLED_PIN		GPIO_Pin_12
	#define IR_IN_PORT		B
	#define IR_IN_PIN		0
	#define LED_PORT		GPIOB
	#define LED_PIN			GPIO_Pin_1
	#define WAKEUP_RESET_PORT	GPIOB
	#define WAKEUP_RESET_PIN	GPIO_Pin_13
	#define USB_DISC_PORT		GPIOB
	#define USB_DISC_RCC_APB2Periph	RCC_APB2Periph_GPIOB
	#define USB_DISC_PIN		GPIO_Pin_9
#elif defined(MapleMini_ExtBd) || defined(MapleMini_2k_ExtBd) /* Maple Mini with Extension Board */
	#define WAKEUP_PORT		GPIOB
	#define WAKEUP_PIN		GPIO_Pin_7 /* 15 */
	#define RESET_PORT		GPIOB
	#define RESET_PIN		GPIO_Pin_15
	#define EXTLED_PORT		GPIOB
	#define EXTLED_PIN		GPIO_Pin_6 /* 16 */
	#define IR_IN_PORT		C
	#define IR_IN_PIN		13 /* 14 */
	#undef	IR_OUT_PORT
	#undef	IR_OUT_PIN
	#define IR_OUT_PORT		A
	#define IR_OUT_PIN		6
	#define LED_PORT		GPIOB
	#define LED_PIN			GPIO_Pin_1
	#define WAKEUP_RESET_PORT	GPIOB
	#define WAKEUP_RESET_PIN	GPIO_Pin_13
	#define USB_DISC_PORT		GPIOB
	#define USB_DISC_RCC_APB2Periph	RCC_APB2Periph_GPIOB
	#define USB_DISC_PIN		GPIO_Pin_9
#else
	#error "Missing define for board"
#endif /* BlueLink */

#endif /* __CONFIG_H */
