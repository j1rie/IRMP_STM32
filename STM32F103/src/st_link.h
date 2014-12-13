/*
 * Copyright (C) 2014 Joerg Riechardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __LED_H
#define __LED_H

extern volatile unsigned int systicks2;
extern void delay_ms(unsigned int msec);
void LED_init(void);
void LED_deinit(void);
void fast_toggle(void);
void both_on(void);
void yellow_on(void);
void toggle_LED(void);

#endif /* __LED_H */
