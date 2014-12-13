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

/* MACRO_SLOTS x (MACRO_DEPTH + 1) + WAKE_SLOTS < 170
 * Eeprom page size = 2kB - 4, 511 pairs of 16bit variable + address,
 * IRdata needs 3 variables, 511 / 3 = 170
 */
#define MACRO_SLOTS	8
#define MACRO_DEPTH	8
#define WAKE_SLOTS	8
#define SIZEOF_IR	6

#define MIN_REPEATS	2  // TODO make configurable & use Eeprom

#endif /* __CONFIG_H */
