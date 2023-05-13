/*
 * Copyright (C) 2014-2023 Joerg Riechardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* MACRO_SLOTS x (MACRO_DEPTH + 1) + WAKE_SLOTS <= 85
 * Eeprom page size = (512 - 1) 8bit variables,
 * IRdata needs 6 variables, 511 / 6 = 85
 */
#define MACRO_SLOTS	8
#define MACRO_DEPTH	8
#define WAKE_SLOTS	8
#define SIZEOF_IR	6


/* TODO GPIO x Logging (irmp.c) */

#define WAKEUP_GPIO		2  /* GPIO 2  (Pin 4)  */
#define RESET_GPIO		3  /* GPIO 3  (Pin 5)  */
#define EXTLED_GPIO		4  /* GPIO 4  (Pin 6)  */
#define IR_IN_GPIO		28 /* GPIO 28 (Pin 34) */
#define IR_OUT_GPIO		6  /* GPIO 6  (Pin 9)  */
#define WAKEUP_RESET_GPIO	7  /* GPIO 7  (Pin 10) */
#define LED_GPIO		8  /* GPIO 8  (Pin 11) */

#endif /* __CONFIG_H */
