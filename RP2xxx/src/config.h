/*
 * Copyright (C) 2014-2024 Joerg Riechardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* MACRO_SLOTS x (MACRO_DEPTH + 1) + WAKE_SLOTS + 1 <= 85
 * Eeprom page size = (512 - 1) 8bit variables,
 * IRdata needs 6 variables, 511 / 6 = 85
 * Eeprom partition 1: MACRO_SLOTS macros of length MACRO_DEPTH + 1
 * Eeprom partition 2: WAKE_SLOTS wakeups
 * Eeprom partition 3: send_after_wakeup
 */
#define MACRO_SLOTS	8
#define MACRO_DEPTH	8
#define WAKE_SLOTS	8
#define SIZEOF_IR	6


/* TODO GPIO x Logging (irmp.c) */

#define WAKEUP_GPIO		27 /* GPIO 27 (Pico(2) Pin 32, One+Zero Pin 27, Seeed XIAO RP2350 D1) */
#define EXTLED_GPIO		4  /* GPIO 4  (Pico(2) Pin 6,  One+Zero Pin 4, Seeed XIAO RP2350 D9)  */
#define IR_IN_GPIO		28 /* GPIO 28 (Pico(2) Pin 34, One+Zero Pin 28, Seeed XIAO RP2350 D2) */
#define IR_OUT_GPIO		6  /* GPIO 6  (Pico(2) Pin 9,  One+Zero Pin 6, Seeed XIAO RP2350 D4)  */
#define STATUSLED_GPIO		2  /* GPIO 2  (Pico(2) Pin 4,  One+Zero Pin 2, Seeed XIAO RP2350 D8)  */
//#ifndef SEEED_XIAO_RP2350
#define IS_RGBW			false
//#else
//#define IS_RGBW			true
//#endif
#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN		PICO_DEFAULT_WS2812_PIN /* GPIO16 (One+Zero Onboard), GPIO22 (Seeed XIAO RP2350 Onboard) */
#else
// default to pin 16 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN		16 /* GPIO 16 (Pico(2) Pin 21) */
#endif
/* it seems, pio pins need to be consecutive
 * does this work with USB Erratum RP2040-E5? otherwise choose 17 (but that's only a solder point on Zero and One)
 */
#define WS2812_PIN_2		WS2812_PIN - 1  /* GPIO 15 (Pico(2) Pin 20, One+Zero Pin 15), GPIO 21 (Seeed XIAO RP2350 D11) */
#define NUM_PIXELS		64

#endif /* __CONFIG_H */
