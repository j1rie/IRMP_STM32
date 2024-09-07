/*
 *  Copyright (c) 2023 Joerg Riechardt
 *
 *  Copyright (c) 2009-2015 Frank Meyer - frank(at)fli4l.de
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include "irmpmain.h"

bool irmp_timer_callback(repeating_timer_t *rt);
repeating_timer_t irmp_timer;

/* initialize timer and interrupt */
void IRMP_Init(void)
{
	stdio_init_all();

	add_repeating_timer_us(-1000000 / F_INTERRUPTS, irmp_timer_callback, NULL, &irmp_timer);

	irmp_init();
}

bool irmp_timer_callback(repeating_timer_t *rt)
{
	if (! irsnd_ISR()) { /* call irsnd ISR */
		irmp_ISR(); /* if not busy call irmp ISR */
	}

	return true;
}